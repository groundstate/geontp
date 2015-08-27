//
// geonntp - a NTP traffic display program for Linux
//
// The MIT License (MIT)
//
// Copyright (c)  2014  Michael J. Wouters
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

#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include <QtGui>
#include <QDomDocument>
#include <QtDebug>
#include <QAction>
#include <QRegExp>
#include <QFile>
#include <QHBoxLayout>
#include <QMenu>
#include <QTextStream>

#include "City.h"
#include "Client.h"
#include "ClientPollRecord.h"
#include "MapWidget.h"
#include "PowerManager.h"
#include "Server.h"
#include "ServerPoll.h"
#include "MainWindow.h"

#define APP_NAME "geontp"
#define VERSION_INFO "v0.2.0"

//#define NEUTER

MainWindow::MainWindow(QStringList & args){

	setDefaults();
	
	for (int i=1;i<args.size();i++){ // skip the first
		if (args.at(i) == "--nofullscreen")
			fullScreen=false;
		else if (args.at(i) == "--help"){
			std::cout << APP_NAME << std::endl;
			std::cout << "Usage: geontp [options]" << std::endl;
			std::cout << std::endl;
			std::cout << "--help         print this help" << std::endl;
			std::cout << "--license      print license" << std::endl;
			std::cout << "--nofullscreen run in a window" << std::endl;
			std::cout << "--version      display version" << std::endl;
			
			exit(EXIT_SUCCESS);
		}
		else if (args.at(i) == "--license"){
			std::cout << APP_NAME << " - a NTP traffic display program for Linux" << std::endl;
			std::cout <<  std::endl;
			std::cout << " The MIT License (MIT)" << std::endl;
			std::cout <<  std::endl;
			std::cout << " Copyright (c)  2014  Michael J. Wouters" << std::endl;
			std::cout <<  std::endl; 
			std::cout << " Permission is hereby granted, free of charge, to any person obtaining a copy" << std::endl;
			std::cout << " of this software and associated documentation files (the \"Software\"), to deal" << std::endl;
			std::cout << " in the Software without restriction, including without limitation the rights" << std::endl;
			std::cout << " to use, copy, modify, merge, publish, distribute, sublicense, and/or sell" << std::endl;
			std::cout << " copies of the Software, and to permit persons to whom the Software is" << std::endl;
			std::cout << " furnished to do so, subject to the following conditions:" << std::endl;
			std::cout << std::endl; 
			std::cout << " The above copyright notice and this permission notice shall be included in" << std::endl;
			std::cout << " all copies or substantial portions of the Software." << std::endl;
			std::cout << std::endl;
			std::cout << " THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR" << std::endl;
			std::cout << " IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY," << std::endl;
			std::cout << " FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE" << std::endl;
			std::cout << " AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << std::endl;
			std::cout << " LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM," << std::endl;
			std::cout << " OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN" << std::endl;
			std::cout << " THE SOFTWARE." << std::endl;
			
			exit(EXIT_SUCCESS);
		}
		else if (args.at(i) == "--version"){
			std::cout << APP_NAME << " "  << VERSION_INFO << std::endl;
			std::cout << std::endl;
			std::cout << "This ain't no stinkin' Perl script!" << std::endl;
			
			exit(EXIT_SUCCESS);
		}
		else{
			std::cout << "geontp: Unknown option '"<< args.at(i).toStdString() << "'" << std::endl;
			std::cout << "geontp: Use --help to get a list of available command line options"<< std::endl;
			
			exit(EXIT_SUCCESS);
		}
	}
	
	QTime on(9,0,0);
	QTime off(17,0,0);
	
	powerManager=new PowerManager(on,off);
	powerManager->enable(false);
	
	// Look for a configuration file
	// The search path is ./:~/geontp:~/.geontp:/usr/local/share/geontp:/usr/share/geontp
	
	QFileInfo fi;
	QString config;
	QString fconfig = QString(APP_NAME)+QString(".xml");
	QString s("./"+fconfig);
	fi.setFile(s);
	if (fi.isReadable())
		config=s;
	
	if (config.isNull()){
		char *eptr = getenv("HOME");
		QString home("./");
		if (eptr)
			home=eptr;
		s=home+"/"+APP_NAME+"/"+fconfig;
		fi.setFile(s);
		if (fi.isReadable())
			config=s;
		if (config.isNull()){
			s=home+"/."+APP_NAME+"/"+fconfig;
			fi.setFile(s);
			if (fi.isReadable())
				config=s;
		}
	}
	
	if (config.isNull()){
		s="/usr/local/share/geontp/"+fconfig;
		fi.setFile(s);
		if (fi.isReadable())
			config=s;
	}
	
	if (config.isNull()){
		s="/usr/share/geontp/"+fconfig;
		fi.setFile(s);
		if (fi.isReadable())
			config=s;
	}
	
	if (!config.isNull())
		readConfig(config);
	
	readCityDB();
	readClientDB();
	
	setWindowTitle(tr(APP_NAME));
	setMinimumSize(QSize(1920,1080));
	if (fullScreen)
		setWindowState(windowState() ^ Qt::WindowFullScreen);
	
	setMouseTracking(true); // so that mouse movements wake up the display
	QCursor curs;
	curs.setShape(Qt::BlankCursor);
	setCursor(curs);
	cursor().setPos(0,0);
	
	QHBoxLayout *hb = new QHBoxLayout(this);
	hb->setContentsMargins(0,0,0,0);
	mapWidget = new MapWidget(cities,servers,mapFileName,lat0,lon0,lat1,lon1,projection,xroll,yroll,this);
	hb->addWidget(mapWidget);

	mapWidget->setBordersOn(showBorders);
	mapWidget->setIPsOn(showIP);
	mapWidget->setGridOn(showGrid);
	mapWidget->setPlaceNamesOn(showPlaceNames);
	mapWidget->setPathsOn(showPaths);
				
	// do this here since we connect() to mapWidget
	createActions();
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this,SIGNAL(customContextMenuRequested ( const QPoint & )),this,SLOT(createContextMenu(const QPoint &)));
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(scheduler()));
	#ifndef NEUTER
	timer->start(updateInterval);
	#endif
	
}

void MainWindow::scheduler(){
	// run over the queue - if anything is to be displayed within the currentTime
	powerManager->update();
	struct timeval tnow;
	gettimeofday(&tnow,NULL);
	tnow.tv_usec += updateInterval*1000; // don't need to worry about overflow
	//qDebug() << "tnow = " << tnow.tv_sec << " "<< tnow.tv_usec;
	for (int i=0;i<servers.size();i++){
		int j=0;
		
		while  (j<servers[i]->clients.size()){
			ClientPollRecord *cr = servers[i]->clients.at(j);
			if (cr->polltime.tv_sec - tnow.tv_sec + (cr->polltime.tv_usec - tnow.tv_usec)/1.0E6< 0){
				//qDebug() << "Dispatching: " << cr->ip<< ": now " <<servers[i]->clients.size() <<  " " << cr->polltime.tv_sec << " " << cr->polltime.tv_usec;
				mapWidget->addNTPPoller(cr,servers[i]->lat,servers[i]->lon);
				servers[i]->clients.removeAt(j);
				delete cr;
			}
			else
				j++;
		}
		//std::cerr << "queue contains " << plotQueue[i].size() << std::endl;
	}
	
}

void MainWindow::updateClientData(QString server, QByteArray data){
	
	struct timeval tnow;
	gettimeofday(&tnow,NULL);
	qDebug() << "Updating  " <<	server << " at" << tnow.tv_sec << " " << tnow.tv_usec;
	
	int serverID=-1;

	for (int i=0;i<servers.size();i++){
		if (server == servers[i]->name){
			serverID=i;
			break;
		}
	}

	if (serverID==-1){
		qWarning() << "unknown server";
		return;
	}	

	if (data.isEmpty()){
		servers[serverID]->appendToHistory(0);
		return;
	}
	
	// data is delineated by <body> tag
	// binary data format is
	// reference_time_secs reference_time_msecs IP   dt_msecs IP  dt_msecs .....
	// U64								 U32                  U32  U32    U32   U32 
	// nb IP is in network byte order
	
	int bufbegin=data.indexOf("<body>");
	int bufend  =data.indexOf("</body>");
	if (-1==bufbegin || -1==bufend){
		qWarning() << "corrupt reply";
		return;
	}
	
	bufbegin+=6;
	if (bufbegin==bufend){
		qDebug() << "empty reply";
		return;
	}
	bufend--; // point to the last character in the buffer
	//qDebug() << bufbegin << " " << bufend << " " << bufend-bufbegin+1;
	// (bufend-bufbegin+1) % 8 should equal 4
	if (((bufend-bufbegin+1) % 8) != 4){
		qDebug() << "unexpected number of bytes in data block";
		return;
	}
	
	char *csptr = (char *) data.constData();
	time_t tref_s;
	suseconds_t tref_us;
	
	// Extract the reference time
	if (8==sizeof(long int)){
		csptr += bufbegin;
		unsigned long int *trefs_uli = (unsigned long int *) csptr;
		//qDebug() <<  *trefs_uli;
		tref_s = (time_t) *trefs_uli;
	}
	else if (8==sizeof(long long int)){
	}
	
	csptr += 8;
	unsigned  int *trefms_ui = (unsigned  int *) csptr;
	//qDebug() << *trefms_ui;
	tref_us = (*trefms_ui) * 1000;
	
	csptr += 4;
	
	// Extract the client records
	int nrec=((bufend-bufbegin+1)-12)/8;
	for (int i=0;i<nrec;i++){
		struct in_addr addr;
		in_addr_t *ip= (in_addr_t *) csptr;
		addr.s_addr=*ip;
		QString  ipstr(inet_ntoa(addr));
		//qDebug() << ipstr;
		csptr += 4;
		unsigned  int *dtPollms = (unsigned  int *) csptr;
		//qDebug() << *dtPollms;
		csptr += 4;
		
		// first in will be last out - just what we want
	  // look up the location
		Client *client = clients[ipstr];
	
		if (trackAliens && !client){ // unknown client 
			client = new Client(ipstr,0,0,true); // you are an alien
			clients[ipstr]=client;
			aliens.append(client);
			//qDebug() << "alien: " << ipstr;
		}
		
		// Construct the poll time
		// Poll times have a maximum latency of pollInterval
		struct timeval ptime=tnow;
		double delay = pollInterval - ((tnow.tv_sec - tref_s) + (tnow.tv_usec - tref_us)/1.0E6 - (*dtPollms)/1.0E3);
		//qDebug() << tref_s  << tref_us/1.0E6+(*dtPollms)/1.0E3 << delay;
	
		double t = ptime.tv_sec + (double) ptime.tv_usec/1.0E6 + delay;	
		ptime.tv_sec = (int) t;
		ptime.tv_usec = (int) ((t - ptime.tv_sec)*1.0E6);
		
		//qDebug() << ptime.tv_sec << " " << ptime.tv_usec;
		QString displayedIP = client->formattedIP;
		if (anonymiseIPs){
			displayedIP = QString::number((int) ((double)rand()/ (double) RAND_MAX * 128+127))+ "." +
			        QString::number((int)((double)rand()/ (double) RAND_MAX * 255))+ "." +
							QString::number((int)((double)rand()/ (double) RAND_MAX * 255))+ "." +
							QString::number((int)((double)rand()/ (double) RAND_MAX * 255));
		}
		ClientPollRecord *cprec = 
			new ClientPollRecord(displayedIP,client->lat,client->lon,ptime,client->alien);
		servers[serverID]->clients.append(cprec);							
	}
	servers[serverID]->appendToHistory(nrec);
}

void MainWindow::toggleFullScreen(){
	fullScreen=!fullScreen;
	setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::createContextMenu(const QPoint &){

	toggleGridAction->setChecked(mapWidget->gridOn());
	toggleIPsAction->setChecked(mapWidget->IPsOn());
	toggleFullScreenAction->setChecked(fullScreen);
	togglePathsAction->setChecked(mapWidget->pathsOn());
	toggleNamesAction->setChecked(mapWidget->placeNamesOn());
	toggleBordersAction->setChecked(mapWidget->bordersOn());

	QMenu *cm = new QMenu(this);
	cm->addAction(toggleGridAction);
	cm->addAction(toggleIPsAction);
	cm->addAction(togglePathsAction);
	cm->addAction(toggleNamesAction);
	cm->addAction(toggleBordersAction);
	cm->addAction(toggleFullScreenAction);
	cm->addAction(saveAliensAction);
	cm->addAction(byebyeAction);
	cm->exec(QCursor::pos());
	delete cm;
}

void MainWindow::saveAliens(){

	// intended to create a list of IPs that can be examined more closely to guess their location
	QFile file( aliensFile );
	if ( !file.open( QIODevice::WriteOnly | QIODevice::Text) ){
		qWarning() << "Couldn't save aliens";
		return;
	}
	QTextStream out( &file );

	for (int i=0;i<aliens.size();i++){
		out << aliens.at(i)->ip << "\n";
	}
	
	file.close();
	
}

//
//
//

void MainWindow::setDefaults(){
	pollInterval=131; // in seconds - best if this is a power of 2 plus a bit since ntpd
										// polling intervals are a power of 2
	updateInterval=50; // in msecs

	mapFileName="textures/freeaus2.png";
	lat0 = 7.4;
	lon0 = 89.8667;
	lat1 = -47.4;
	lon1=180.0;
	projection=MapWidget::Mercator;
	xroll=7.0;
	yroll=0.0;
	
	fullScreen=true;
	showGrid=false; 
	showIP=true;
	showPaths=true;
	showBorders=true;
	showPlaceNames=true;
	
	citydb="auscities.db";
	clientdb = "client.db";
	
	trackAliens=true;
	aliensFile="aliens.txt";
	maxAliens=1000;
	
	anonymiseIPs=false;
}

void MainWindow::readConfig(QString s){
	QDomDocument doc;
	
	qDebug() << "Using configuration file " << s;
	
	QFile f(s);
	if ( !f.open( QIODevice::ReadOnly) ){
		qWarning() << "Can't open " << s;
		return;
	}
	
	QString err;
	int errlineno,errcolno;
	if ( !doc.setContent( &f,true,&err,&errlineno,&errcolno ) ){	
		qWarning() << "PARSE ERROR " << err << " line=" << errlineno;
		f.close();
		return ;
	}
	f.close();
	
	QDomElement elem = doc.documentElement().firstChildElement();
	QString lc;

	int startDelay=1;
	while (!elem.isNull()){
		if (elem.tagName()=="pollinterval"){
			pollInterval = elem.text().toInt();
		}
		else if (elem.tagName()=="citydb"){
			citydb = elem.text().trimmed();
		}
		else if (elem.tagName()=="clientdb"){
			clientdb = elem.text().trimmed();
		}
		else if (elem.tagName()=="image"){
			QDomElement cel=elem.firstChildElement();
			
			while(!cel.isNull()){
				if (cel.tagName() == "file"){
					mapFileName = cel.text().trimmed();
				}
				else if (cel.tagName() == "latupperleft"){
					lat0=cel.text().toDouble();
				}
				else if (cel.tagName() == "lonupperleft"){
					lon0=cel.text().toDouble();
				}
				else if (cel.tagName() == "latlowerright"){
					lat1=cel.text().toDouble();
					qDebug() << lat1;
				}
				else if (cel.tagName() == "lonlowerright"){
					lon1=cel.text().toDouble();
				}
				else if (cel.tagName() == "projection"){
					QString p=cel.text().trimmed().toLower();
					if (p=="equirectangular")
						projection=MapWidget::Equirectangular;
					else if (p=="mercator")
						projection=MapWidget::Mercator;
				}
				else if (cel.tagName() == "xroll"){
					xroll=cel.text().toDouble();
				}
				else if (cel.tagName() == "yroll"){
					yroll=cel.text().toDouble();
				}
				cel=cel.nextSiblingElement();
			}
		}
		else if (elem.tagName()=="display"){
			QDomElement cel=elem.firstChildElement();
			
			while(!cel.isNull()){
				if (cel.tagName() == "borders")
					showBorders = cel.text().trimmed() == "yes";
				else if (cel.tagName() == "ips")
					showIP = cel.text().trimmed() == "yes";
				else if (cel.tagName() == "grid")
					showGrid=cel.text().trimmed() == "yes";
				else if (cel.tagName() == "placenames")
					showPlaceNames=cel.text().trimmed() == "yes";
				else if (cel.tagName() == "paths")
					showPaths=cel.text().trimmed() == "yes";
				else if (cel.tagName() == "anonymise")
					anonymiseIPs=cel.text().trimmed() == "yes";
				cel=cel.nextSiblingElement();
			}
		}
		else if (elem.tagName()=="server"){
			QString name;
			double latitude=999,longitude=999;
			int port=80;
			QDomElement cel=elem.firstChildElement();
			while(!cel.isNull()){
				if (cel.tagName() == "name")
					name=cel.text().trimmed();
				else if (cel.tagName() == "port")
					port=cel.text().toInt();
				else if (cel.tagName() == "latitude")
					latitude=cel.text().toDouble();
				else if (cel.tagName() == "longitude")
					longitude=cel.text().toDouble();
				cel=cel.nextSiblingElement();
			}
			if (!name.isNull() && (fabs(latitude) <= 90.0) && (fabs(longitude) <= 360.0)){
				servers.append(new Server(name,latitude,longitude));
				ServerPoll *poller = new ServerPoll(name,port,pollInterval,startDelay,"",-1,"","",this);
				connect(poller,SIGNAL(clientDataReceived(QString,QByteArray )),this,SLOT(updateClientData(QString,QByteArray )));
				pollers.append(poller);
				startDelay+=2;
				#ifndef NEUTER
				poller->start(); 
				#endif
			}
		}
		else if (elem.tagName()=="aliens"){
			QDomElement cel=elem.firstChildElement();
			
			while(!cel.isNull()){
				if (cel.tagName() == "track")
					trackAliens = cel.text().trimmed() == "yes";
				else if (cel.tagName() == "file")
					aliensFile = cel.text().trimmed();
				else if (cel.tagName() == "max")
					maxAliens=cel.text().toInt();
				cel=cel.nextSiblingElement();
			}
		}
		else if (elem.tagName()=="proxy"){
			QDomElement cel=elem.firstChildElement();
			proxyServer="";
			proxyPort=-1;
			proxyUser="";
			proxyPassword="";
			while(!cel.isNull()){
				if (cel.tagName() == "server")
					proxyServer = cel.text().trimmed();
				else if (cel.tagName() == "port")
					proxyPort = cel.text().toInt();
				else if (cel.tagName() == "user")
					proxyUser=cel.text().trimmed();
				else if (cel.tagName() == "password")
					proxyPassword=cel.text().trimmed();
				cel=cel.nextSiblingElement();
			}
		}
		else if (elem.tagName()=="power"){
			QDomElement celem=elem.firstChildElement();
			while(!celem.isNull()){
				lc=celem.text().toLower();
				lc=lc.simplified();
				lc=lc.remove('"');
				if (celem.tagName() == "conserve"){
					powerManager->enable(lc == "yes");
					qDebug() << "power::conserve=" << lc;
				}
				else if (celem.tagName() == "weekends"){
					if (lc=="yes") 
						powerManager->setPolicy(PowerManager::NightTime | PowerManager::Weekends);
					else
						powerManager->setPolicy(PowerManager::NightTime);
					qDebug() << "power::weekends=" << lc;
				}
				else if (celem.tagName() == "on"){
					QTime t=QTime::fromString(lc,"hh:mm:ss");
					if (t.isValid())
						powerManager->setOnTime(t);
					else
						qWarning() << "Invalid power on time: " << lc;
				}
				else if (celem.tagName() == "off"){
					QTime t=QTime::fromString(lc,"hh:mm:ss");
					if (t.isValid())
						powerManager->setOffTime(t);
					else
						qWarning() << "Invalid power off time: " << lc;
				}
				else if (celem.tagName() == "overridetime"){
					powerManager->setOverrideTime(celem.text().toInt());
					qDebug() << "power::overridetime=" << lc;
				}
				celem=celem.nextSiblingElement();
			}
		}
		elem=elem.nextSiblingElement();
	}
	
	// post-config cleanups
	for (int i=0;i<pollers.size();i++)
		pollers.at(i)->setProxy(proxyServer,proxyPort,proxyUser,proxyPassword);
	for (int i=0;i<servers.size();i++)
		servers.at(i)->pollInterval=pollInterval;
	
}

void MainWindow::createActions()
{
	toggleGridAction = new QAction(QIcon(), tr("Lat/lon grid"), this);
	toggleGridAction->setStatusTip(tr("Show latitude/longitude grid"));
	toggleGridAction->setCheckable(true);
	addAction(toggleGridAction);
	connect(toggleGridAction, SIGNAL(triggered()), mapWidget, SLOT(toggleGrid()));

	toggleIPsAction = new QAction(QIcon(), tr("IP display"), this);
	toggleIPsAction->setStatusTip(tr("Show client IPs"));
	addAction(toggleIPsAction);
	connect(toggleIPsAction, SIGNAL(triggered()), mapWidget, SLOT(toggleIPs()));
	toggleIPsAction->setCheckable(true);

	toggleFullScreenAction = new QAction(QIcon(), tr("Full screen"), this);
	toggleFullScreenAction->setStatusTip(tr("Display full screen"));
	addAction(toggleFullScreenAction);
	connect(toggleFullScreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
	toggleFullScreenAction->setCheckable(true);

	toggleNamesAction = new QAction(QIcon(), tr("City names"), this);
	toggleNamesAction->setStatusTip(tr("Show city names"));
	toggleNamesAction->setCheckable(true);
	addAction(toggleNamesAction);
	connect(toggleNamesAction, SIGNAL(triggered()), mapWidget, SLOT(togglePlaceNames()));

	toggleBordersAction = new QAction(QIcon(), tr("Borders"), this);
	toggleBordersAction->setStatusTip(tr("Show borders"));
	toggleBordersAction->setCheckable(true);
	addAction(toggleBordersAction);
	connect(toggleBordersAction, SIGNAL(triggered()), mapWidget, SLOT(toggleBorders()));

	togglePathsAction = new QAction(QIcon(), tr("NTP paths"), this);
	togglePathsAction->setStatusTip(tr("Show NTP query paths"));
	togglePathsAction->setCheckable(true);
	addAction(togglePathsAction);
	connect(togglePathsAction, SIGNAL(triggered()), mapWidget, SLOT(togglePaths()));

	saveAliensAction = new QAction(QIcon(), tr("Save aliens"), this);
	saveAliensAction->setStatusTip(tr("Save aliens to file"));
	addAction(saveAliensAction);
	connect(saveAliensAction, SIGNAL(triggered()), this, SLOT(saveAliens()));
	
	byebyeAction = new QAction(QIcon(), tr("Quit"), this);
	byebyeAction->setStatusTip(tr("Quit"));
	byebyeAction->setCheckable(true);
	addAction(byebyeAction);
	connect(byebyeAction, SIGNAL(triggered()), this, SLOT(close()));
	
}

void MainWindow::readCityDB(){

	QFile f(citydb);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)){
		qFatal("Couldn't find city db");
		return;
	}

	QTextStream in(&f);
	int lineno=0;
	while (!in.atEnd()) {
		lineno++;
		QString line = in.readLine();
		line = line.simplified();
   	// Format is place_name, lat_dd lat_mm, lon_dd lon_mm, 
		QStringList cs = line.split(',');
		for (int i=0;i<cs.size();i++)
			cs[i]=cs.at(i).trimmed();
		if (cs.size() >= 5) //
		{
			double l = cs.at(1).toDouble() ;
			int sgn= (l>=0)?1:-1;
			double lat = l + sgn*cs.at(2).toDouble()/60.0;
			l = cs.at(3).toDouble() ;
			sgn= (l>=0)?1:-1;
			double lon = l + sgn*cs.at(4).toDouble()/60.0;
			City *c = new City(cs.at(0),lat,lon);
			cities[cs.at(0)]=c;
			
			if (cs.size() > 5){
				for (int i= 5;i<cs.size();i++){
					// FIXME this is very sloppy
					if (cs.at(i).contains("R"))
						c->align=Qt::AlignRight;
					if (cs.at(i).contains("V"))
						c->align|=Qt::AlignVCenter;
					if (cs.at(i).contains("X"))
						c->visible=false;
					if (cs.at(i).contains("Capital"))
						c->capital=true;
				}
			}
		}
		else{
			qWarning() << "City database error at line " << lineno ;
		}
	}

}

void MainWindow::readClientDB(){
	QFile f(clientdb);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text)){
		qFatal("Can't open client DB");
		return;
	}
	
	QTextStream in(&f);
	int lineno=0;
	
	while (!in.atEnd()) {
		lineno++;
		QString line = in.readLine();
		line = line.simplified();
   	// Format is ip placename
		QStringList cs = line.split(QRegExp("\\s+"));
		if (cs.size() <=1){
			qWarning() << "Client DB error at line " << lineno;
		}
		else{
			QString cityname(cs.at(1));
			for (int i=2;i<cs.size();i++)
				cityname += " " + cs.at(i);
			City *city = cities[cityname];
			if (city){
				clients[cs.at(0)]=new Client(cs.at(0),city->lat,city->lon);
			}
			else{ // if the city is not in the DB, the client is dropped
				qWarning() << "Couldn't find " << cityname << " at line " << lineno << " in client DB";
			}
		}
	}
	
}
