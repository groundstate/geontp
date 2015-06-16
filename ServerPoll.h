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

#ifndef __SERVER_POLL_H_
#define __SERVER_POLL_H_

#include <QString>
#include <QTimer>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class ServerPoll:public QObject{
	Q_OBJECT
	
	public:

		ServerPoll(QString,int,int,int,QString proxyServer="",int proxyPort=-1,
			QString proxyUser="", QString proxyPassword="", QObject *parent=NULL);
		~ServerPoll();
		void setProxy(QString,int,QString, QString);
		
		void start();
		
	signals:

		void clientDataReceived(QString,QByteArray);

	private slots:
		void startQuery();
		void replyFinished(QNetworkReply*);
			
	private:

		int pollInterval_;
		QString server_;
		int port_;
		int startDelay_;
		QString proxyServer_,proxyUser_,proxyPassword_;
		int proxyPort_;
		bool abort_;
		QString httpQuery_;
		
		QNetworkAccessManager *netManager;
		QTimer *timer_;
		bool queryRunning;
};

#endif
