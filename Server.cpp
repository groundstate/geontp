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

#include <QDebug>

#include "Server.h"

Server::Server(QString n,double latitude,double longitude)
{
	name =n;
	lat=latitude;
	lon=longitude;
	summingLength=2;
	historyLength=500;
	lastUpdate=QDateTime::currentDateTime();
}

void Server::setHistoryLength(int l)
{
	historyLength=l;
}

void Server::setSummingLength(int l)
{
	summingLength = l;
}

void Server::appendToHistory(double d)
{
	shortHistory.push_back(d);
	double sum=0.0;
	if (shortHistory.size()==summingLength){
		for (int i=0;i<shortHistory.size();i++)
			sum += shortHistory.at(i);
		shortHistory.clear();
		int dt = lastUpdate.secsTo(QDateTime::currentDateTime());
		lastUpdate=QDateTime::currentDateTime();
		if (longHistory.size()==0){ // first point - lastUpdate time is wrong so fudge it
			dt=dt*(summingLength/(summingLength-1.0)); // approximately
		}
		longHistory.push_back(60.0*sum/dt); // per minute
		if (longHistory.size()==historyLength)
			longHistory.removeFirst();
		qDebug() << name << ":sum="<<sum/dt << ", dt=" << dt;
	}
}

		