//
// trafficrep - a program for reporting network traffic
//
// The MIT License (MIT)
//
// Copyright (c) 2014  Michael J. Wouters
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <cstdlib>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <iostream>

#include "Debug.h"
#include "Client.h"
#include "PcapThread.h"
#include "TrafficReporter.h"

extern ostream *debugStream;

// default snapshot length (maximum bytes per packet to capture)
#define SNAP_LEN 1518

// ethernet headers are always exactly 14 bytes
#define SIZE_ETHERNET 14

struct udp_header {      // Transport Control Protocol header
	u_short th_sport;      // source port
	u_short th_dport;      // destination port
	u_short th_len;      // length
	u_short th_sum;      // checksum
};

typedef struct{
	PcapThread *pPcapThread;
}pcapdata;

//
// public
//

PcapThread::PcapThread(TrafficReporter *a,const char *d,const char *f,long mh)
{
	threadID = "pcap";
	
	app=a; // icky but convenient
	filter = f;
	dev = d;
	numPackets=5;
	pcaphandle=NULL;
	maxHistory=mh; // retain history for this time
	initPacketCapture();
}

PcapThread::~PcapThread()
{
	
}

void PcapThread::stop() 
{
	assert(thread);
	// explicitly break the loop since if no capture data is being received, pcap_loop
	// doesn't return
	if (pcaphandle) pcap_breakloop(pcaphandle); 
	stopRequested = true;
	pthread_join(*thread,NULL);
}
		
void PcapThread::setMaxHistory(int mh)
{
	pthread_mutex_lock(&mutex);
	maxHistory = mh;
	pthread_mutex_unlock(&mutex);
}

void PcapThread::clearClients()
{
	pthread_mutex_lock(&mutex);
	while(! clients.empty()){
		Client *tmp= clients.back();
		delete tmp;
		clients.pop_back();
	}
	pthread_mutex_unlock(&mutex);
}

void PcapThread::getClients(list<Client *>&cl ,int nlastSecs)
{
	// Copy the data - this way it doesn't get pulled from under our feet
	// (but could use shared pointers ...)
	pthread_mutex_lock(&mutex);
	struct timeval tv;
	gettimeofday(&tv,NULL);
	
	list<Client *>::reverse_iterator curr = clients.rbegin(); // start with the newest
	while (curr != clients.rend())
	{
		if (nlastSecs >=0){
			Client *client = *curr;
			double dt = ( tv.tv_sec-client->rcvtime.tv_sec)+(tv.tv_usec-client->rcvtime.tv_usec)/1.0E6;
			if (dt <= nlastSecs)
				cl.push_back(new Client(client));
			else
				break; // time ordered, so we got the last one
		}
		curr++;
	}
	pthread_mutex_unlock(&mutex);
}

void PcapThread::addClient(const struct in_addr *a,const struct timeval *rt)
{
	pthread_mutex_lock(&mutex);
	
	clients.push_back(new Client(a,rt));
	
	// clean up stinky entries
	struct timeval tv;
	gettimeofday(&tv,NULL);
			
	DBGMSG(debugStream,clients.size()<< " clients before");
	Client *c = clients.front();
	while (c!=NULL){
		int dt = tv.tv_sec - c->rcvtime.tv_sec;
		if (dt > maxHistory){
			clients.pop_front();
			delete c;
		}
		else{
			DBGMSG(debugStream,clients.size()<< " clients after" );
			break; // in time order so should be safe
		}
		c = clients.front();	
	}
	pthread_mutex_unlock(&mutex);
}

//
// Protected
//

void PcapThread::doWork()
{
	DBGMSG(debugStream,"working");
	pcapdata pcd;
	pcd.pPcapThread=this;
	
	while (!stopRequested){
		// this returns when data is received
		pcap_loop(pcaphandle, numPackets, processPacket, (u_char *) (&pcd));
	}
	
	if (pcaphandle) pcap_close(pcaphandle);
	
	pthread_exit(NULL);
}


//
// Private
//

void PcapThread::initPacketCapture()
{
	
	const int PCAP_FAILURE = -1;
	
   //  get network number and mask associated with capture device 
   if( pcap_lookupnet(dev.c_str(), &net, &mask, errbuf) == PCAP_FAILURE ){
			app->log(string("Couldn't get netmask for device ") + 
				string(dev) +  string(" because of: ") + string( errbuf ));
			net = 0;
			mask = 0;
   }

   // open capture device 
   pcaphandle = pcap_open_live(dev.c_str(), SNAP_LEN, 1, 1000, errbuf);
   if (pcaphandle == NULL) {
       app->log(string("Couldn't open device ") + 
				dev +  string(" because of: ") + string(errbuf) );
       exit(EXIT_FAILURE);
   }

   // make sure we're capturing on an Ethernet device [2] 
   if( pcap_datalink(pcaphandle) != DLT_EN10MB ) {
			app->log(dev + string(" is not an Ethernet device - exiting"));
       exit(EXIT_FAILURE);
   }

   // compile the filter expression
   if( pcap_compile(pcaphandle, &fp, (char *) filter.c_str(), 0, net) == PCAP_FAILURE ) {
			app->log(string("Couldn't parse filter ") + 
				filter + string(" because of: ") + string(pcap_geterr(pcaphandle)));
      exit(EXIT_FAILURE);
   }

   // apply the compiled filter 
   if( pcap_setfilter(pcaphandle, &fp) == PCAP_FAILURE ) {
     app->log(string("Couldn't install filter  ") + filter + " because of: "
			+ string(pcap_geterr(pcaphandle)));
     exit(EXIT_FAILURE);
   }
}

void PcapThread::processPacket(u_char *args, const struct pcap_pkthdr *header, 
															const u_char *packet){
   
	const struct ip *iphdr;                       // The IP header, BSD-style 
	
	iphdr = (struct ip*)(packet + SIZE_ETHERNET);
	
	DBGMSG(debugStream, header->ts.tv_sec << " " << inet_ntoa(iphdr->ip_src));
	
	pcapdata *pcdPtr = (pcapdata *) args;
	pcdPtr->pPcapThread->addClient(&(iphdr->ip_src),&(header->ts));
	
}
