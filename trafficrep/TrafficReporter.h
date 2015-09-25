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

#ifndef __TRAFFIC_REPORTER_H_
#define __TRAFFIC_REPORTER_H_

#include <fstream>
#include <string>
#include <list>

using namespace std;

class PcapThread;
class Client;
class HTTPThread;

class TrafficReporter
{
	public:
		
		TrafficReporter(int,char **);
		~TrafficReporter();
		
		void log(string);
		void debug(string);
		void run();
		void getPage(string &,int);
		
	private:
	
		static void signalHandler(int sig);
		
		void init();
		bool readConfig(string configPath);
		string getConfigPath();
		bool updatePIDFile();
		
		bool getLong(const char *, long *,long,string);
		
		void showHelp();
		void showVersion();
		
		string timestamp();
		
		string logFileName;
		string debugFileName;
		string packetFilter;
		long maxHistory;
		long port;
		string interface;
		
		string pidFile;
		
		ofstream  appLog;
		ofstream  debugLog;
		
		
		PcapThread *sniffer;
		HTTPThread     *web;
		
		bool txtMode;
		
};

#endif
