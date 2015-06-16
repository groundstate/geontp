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

#include <sys/time.h>
#include <time.h>

#include <QtDebug>
#include <QtNetwork>
#include <QTimer>

#include "ServerPoll.h"

ServerPoll::ServerPoll(QString server,int port,
	int pollInterval,int startDelay,
	QString proxyServer,int proxyPort,QString proxyUser, QString proxyPassword,
	QObject *parent):QObject(parent){
		
	server_=server;
	port_=port;
	pollInterval_=pollInterval; // in seconds
	startDelay_=startDelay; // in seconds
	
	proxyServer_=proxyServer;
	proxyPort_=proxyPort;
	proxyUser_=proxyUser;
	proxyPassword_=proxyPassword;
	
	queryRunning=false;
	
	httpQuery_= "http://"+server_+":"+ QString::number(port_)+"/?last="+QString::number(pollInterval_);
	qDebug() << httpQuery_;
	
	abort_=false;

	netManager = new QNetworkAccessManager();
	if (proxyServer_ != "" && proxyPort_ != -1)
		netManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,proxyServer_,proxyPort_,proxyUser_,proxyPassword_)); // UNTESTED
	
	connect(netManager, SIGNAL(finished(QNetworkReply*)),
         this, SLOT(replyFinished(QNetworkReply*)));
	
	timer_=new QTimer(this);
	connect(timer_,SIGNAL(timeout()),this,SLOT(startQuery()));
	
}

ServerPoll::~ServerPoll(){
	// Should destroy query here but this is not called anyway until
	// program exit
}

void ServerPoll::setProxy(QString proxyServer,int proxyPort,QString proxyUser, QString proxyPassword){
	proxyServer_=proxyServer;
	proxyPort_=proxyPort;
	proxyUser_=proxyUser;
	proxyPassword_=proxyPassword;
	if (proxyServer_ != "" && proxyPort_ != -1)
		netManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,proxyServer_,proxyPort_,proxyUser_,proxyPassword_));
}

			
//
// private slots
//

void ServerPoll::startQuery(){
	if (queryRunning){
		qDebug() << server_ << ":nothing received";
		QByteArray ba;
		emit clientDataReceived(server_,ba);
	}
	queryRunning=false;
	netManager->get(QNetworkRequest(QUrl(httpQuery_)));
	timer_->start(pollInterval_*1000);
}

void ServerPoll::replyFinished(QNetworkReply *reply){

	QNetworkReply::NetworkError err = reply->error();
	queryRunning=false; // whatever happened, it's over
	if (err == QNetworkReply::NoError){
			qDebug() << "Got something from " << server_;
		QByteArray ba = reply->readAll();
		emit clientDataReceived(server_,ba);
	}
	else
	{
		qDebug() << server_ << ":network error";
		QByteArray ba;
		emit clientDataReceived(server_,ba);
	}
	// FIXME do i have to delete the reply with deleteLater
	reply->deleteLater();
}
		
void ServerPoll::start(){
	// Start time is delayed so that poll times are not simultaneous for differenet servers
	timer_->start(startDelay_*1000);
}

