//
// geontp - a program for displaying NTP traffic
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

#ifndef __SERVER_H_
#define __SERVER_H_

#include <sys/time.h>
#include <time.h>

#include <QDateTime>
#include <QList>

class ClientPollRecord;

class Server{
	public:
	
		Server(QString ,QString,double ,double );
		
		QString name; // name is what is shown in the title of the traffic plot
		QString address;  // address is what we plot
		
		double lat,lon;
		QList<ClientPollRecord *> clients;
		
		void setHistoryLength(int);
		void setSummingLength(int);
		
		void appendToHistory(double);
		
		int historyLength;
		int summingLength;
		int pollInterval;
		
		QDateTime lastUpdate;
		QList<double> shortHistory;
		QList<double> longHistory;
		
		int maxTraffic; // for scaling the traffic plot y-axis
		
};

#endif


