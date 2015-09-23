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

#include <iostream>
#include <cstdlib>
#include <sstream>

#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Version.h"
#include "Debug.h"
#include "Client.h"
#include "PcapThread.h"
#include "TrafficReporter.h"
#include "HTTPThread.h"
#include "tinyxml.h"

using namespace std;

#define APP_NAME "trafficrep"
#define APP_VERSION "1.0"
#define APP_AUTHOR "Michael Wouters"
#define PRETTIFIER "*******************************************************"

extern TrafficReporter* app;
extern ostream *debugStream;

TrafficReporter::TrafficReporter(int argc,char **argv)
{
	debugStream= NULL;
	logFileName = string(APP_NAME) + ".log";
	txtMode=false;;
	char c;
	while ((c=getopt(argc,argv,"hvl:d:t")) != -1)
	{
		switch(c)
  	{
			case 'h':showHelp(); exit(EXIT_SUCCESS);
			case 'v':showVersion();exit(EXIT_SUCCESS);
			case 'd':
			{
				string dbgout = optarg;
				if ((string::npos != dbgout.find("stderr"))){
					debugStream = & std::cerr;
				}
				else{
					debugFileName = dbgout;
					debugLog.open(debugFileName.c_str(),ios_base::app);
					if (!debugLog.is_open()){
						cerr << "Unable to open " << dbgout << endl;
						exit(EXIT_FAILURE);
					}
					debugStream = & debugLog;
				}
				break;
			}
			case 'l':
			{
				logFileName = optarg;
				break;
			}
			case 't':txtMode=true;break;
		}
	}
	
  init( argc, argv );

}

TrafficReporter::~TrafficReporter()
{
	log("exiting");
	sniffer->stop();
	web->stop();
	appLog.close();
	debugLog.close();
	unlink(pidFile.c_str());
	exit(EXIT_SUCCESS);
}

void TrafficReporter::log(string msg)
{
	appLog <<timestamp() << " " << msg << endl;
	appLog.flush();
	DBGMSG(debugStream,"LOG:" << msg);
}

void TrafficReporter::run()
{
	sniffer->go();
	web->go();
	// not much to do in the main thread
	while (1)
		sleep(1);
}

void TrafficReporter::getPage(string &buf,int lastNSecs)
{
	
	list<Client *> clients;
	sniffer->getClients(clients,lastNSecs);
	
	buf  = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n";
	buf += "<html><head>\r\n";
	buf += "<title>traffic</title>\r\n";
	buf += "</head><body>";
	
	
	if (clients.empty()){
		buf += "</body></html>\r\n";
		return;
	}
	
	// The first line is the reference time: UNIX time with 1 ms resolution
	// This is taken from the oldest client record (which is at the end)
	Client *client = clients.back();
	struct timeval tvref = client->rcvtime;
	
	if (txtMode){
		ostringstream ss;
		ss << tvref.tv_sec << " " << tvref.tv_usec/1000 << "<br>";
		buf += ss.str(); 
	}
	else{ // binary
		// tv_sec is a time_t and this is 32 bits on older systems and 64 bits on newer systems
		// we'll just cast it to a 64 bit integer 
		if (sizeof(unsigned long int) == 8){
			unsigned long int li_tvsec = tvref.tv_sec;
			char *chptr1 = (char *) (&li_tvsec);
			std::string s1(chptr1,chptr1+sizeof(li_tvsec));
			buf+=s1;
		}
		else if (sizeof(long long int)==8){
			unsigned long long int lli_tvsec = tvref.tv_sec;
			char *chptr1 = (char *) (&lli_tvsec);
			std::string s1(chptr1,chptr1+sizeof(lli_tvsec));
			buf+=s1;
		}
		unsigned int ui_msec=(unsigned int) (tvref.tv_usec/1000); 
		char *chptr2 = (char *) (&ui_msec);
		std::string s2(chptr2,chptr2+sizeof(ui_msec));
		buf+=s2;
	}
	
	list<Client *>::iterator curr = clients.begin();
	while (curr != clients.end()){
		client = *curr;
		unsigned int idt = (int) (1000 *(( client->rcvtime.tv_sec - tvref.tv_sec )+(client->rcvtime.tv_usec-tvref.tv_usec)/1.0E6));
		
		if (txtMode){
			ostringstream ss;
			ss << inet_ntoa(client->addr) << " "  << idt << " " << "<br>";
			buf += ss.str();
		}
		else{
			char *chptr1 = (char *) (&(client->addr.s_addr));
			std::string s1(chptr1,chptr1+sizeof(client->addr.s_addr));
			buf+=s1;
			char *chptr2 = (char *) (&(idt));
			std::string s2(chptr2,chptr2+sizeof(idt));
			buf+=s2;
		}	
		curr++;
	}
	buf += "</body></html>\r\n";
	
	while(! clients.empty()){
		Client *tmp= clients.back();
		delete tmp;
		clients.pop_back();
	}
	
}

		
//
//
//

void TrafficReporter::init(int argc, char *argv[])
{
	
	//set all defaults
	
	packetFilter="dst port 123";
	port=7777;
	interface="eth0";
	maxHistory=1000;
	pidFile="trafficrep.pid";
	
	// This need to be open before we do anything so that startup problems can be logged
	appLog.open(logFileName.c_str(),ios_base::app);
	if (!appLog.is_open()){
		cerr << "Unable to open " << logFileName << endl;
	}
	log(PRETTIFIER);
	log(string(APP_NAME) +  string(" v") + 
		string(APP_VERSION) + string(", last modified ") + string(LAST_MODIFIED));
	log(PRETTIFIER);
	
	string cfgPath=getConfigPath();
	if (cfgPath.size() > 0)
		readConfig(cfgPath);
	
	if (!updatePIDFile()){
		log("Unable to make pid file - already running?");
		exit(EXIT_FAILURE);
	}
	
	struct sigaction sa;
	sigset_t sigioset;
	
	sa.sa_handler = signalHandler;
	
	sigemptyset(&(sa.sa_mask)); // we block nothing 
	sa.sa_flags=0;    
	
	sigaddset(&sigioset,SIGTERM);
	sigaction(SIGTERM,&sa,NULL);
	
	sigaddset(&sigioset,SIGQUIT);
	sigaction(SIGQUIT,&sa,NULL);
	
	sigaddset(&sigioset,SIGINT);
	sigaction(SIGINT,&sa,NULL);
	
	sigaddset(&sigioset,SIGHUP);
	sigaction(SIGHUP,&sa,NULL);
  
  sniffer = new PcapThread(this,interface.c_str(),packetFilter.c_str(),maxHistory); 
	web = new HTTPThread(this,port);
	
} 

bool TrafficReporter::readConfig(std::string configPath)
{

	TiXmlDocument doc( configPath.c_str() );
  if (!doc.LoadFile()){ 
		return false;
	}
	
	log(string("using config file ") + configPath);
		
  TiXmlElement* root = doc.RootElement( );

  if (0 != strcmp(root->Value(), "trafficrep") ) {
		return false;
  }
	
	for ( TiXmlElement* elem = root->FirstChildElement();elem;elem = elem->NextSiblingElement() ){
		if (0 == strcmp(elem->Value(),"pcapfilter")){
			const char *text = elem->GetText();
			if (text)
				packetFilter = text;
		}
		else if (0 == strcmp(elem->Value(),"interface")){
			const char *text = elem->GetText();
			if (text)
				interface= text;
		}
		else if (0 == strcmp(elem->Value(),"maxhistory")){
			getLong(elem->GetText(),&maxHistory,maxHistory,"Invalid value for maxhistory - using 1000");
		}
		else if (0 == strcmp(elem->Value(),"port")){
			getLong(elem->GetText(),&port,port,"Invalid value for port");
		}
		else if (0 == strcmp(elem->Value(),"pidfile")){
			const char *text = elem->GetText();
			if (text)
				pidFile = text;
		}
	}	

	return true;
}

string TrafficReporter::getConfigPath()
{

	struct stat statbuf;
	 
	std::string cfgPath("./trafficrep.xml");// working directory first
	if ((0 == stat(cfgPath.c_str(),&statbuf)))
		return cfgPath;
	
	cfgPath= "/usr/local/share/trafficrep/trafficrep.xml";
	if ((0 == stat(cfgPath.c_str(),&statbuf)))
		return cfgPath;
	
	cfgPath= "/usr/share/trafficrep/trafficrep.xml";
	if ((0 == stat(cfgPath.c_str(),&statbuf)))
		return cfgPath;
	
	cfgPath="";
	
	return cfgPath;
}

bool TrafficReporter::updatePIDFile()
{
	struct stat statbuf;
	FILE *fpid;
	if ((0==stat(pidFile.c_str(),&statbuf))){
		// is it still running
		fpid=fopen(pidFile.c_str(),"r");
		int ipid;
		if ((1!=fscanf(fpid,"%i",&ipid))) 
			return false;
		int ret=kill((pid_t) ipid,0);
		if (ret == 0 || (ret==-1 && errno != ESRCH)) // not completely robust ...
			return false; 
		fclose(fpid);
	}
	
	fpid=fopen(pidFile.c_str(),"w");
	if (NULL!=fpid){
		pid_t pid=getpid();
		fprintf(fpid,"%i\n",(int) pid);
		fclose(fpid);
	}
	return true;
}

bool TrafficReporter::getLong(const char *txt, long *val,long defaultValue,string msg){
	
	if (txt){
		errno=0;
		char *endptr;
		*val =  strtol(txt,&endptr,10); 
		if ((endptr==txt) || (errno == ERANGE && (*val == LONG_MAX || *val == LONG_MIN)) || (errno != 0 && *val == 0)) {
			*val=defaultValue;
			log(msg);
			return false;
		}
		return true;
	}
	return false;
}

void TrafficReporter::signalHandler(int sig)
{
	switch (sig){
		case SIGTERM:
		case SIGQUIT:
		case SIGINT:
			delete app;
			break;
		case SIGHUP:
			//clearClients();
			break;
		default:
			cerr << "Unhandled signal " << sig << endl;
			break;
	}
}


void TrafficReporter::showHelp()
{
	cout << "Usage: " << APP_NAME << " [options]" << endl;
	cout << "Available options are" << endl;
	cout << "\t-d <file> Turn on debugging to <file> (use 'stderr' for output to stderr)" << endl;
	cout << "\t-h  Show this help" << endl;
	cout << "\t-l  <file> Specify an alternate application log file" << endl;
	cout << "\t-t  Set HTML data block to text mode, rather than binary" << endl;
	cout << "\t-v  Show version" << endl;
}

void TrafficReporter::showVersion()
{
	cout << APP_NAME << " v" <<APP_VERSION << ", last modified " << LAST_MODIFIED << endl;
	cout << "Written by " << APP_AUTHOR << endl;
	cout << "This ain't no stinkin' Perl script!" << endl;

}

string TrafficReporter::timestamp()
{
	time_t tt =  time(0);
	struct tm *gmt = gmtime(&tt);
	char tc[128];
	
	strftime(tc,128,"%F %T",gmt);
	return std::string(tc);
}
