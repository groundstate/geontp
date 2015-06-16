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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QHash>
#include <QList>
#include <QWidget>

#include "Globals.h"

class QAction;

class City;
class Client;
class ClientPollRecord;
class MapWidget;
class PowerManager;
class Server;
class ServerPoll;

class MainWindow : public QWidget
{
    Q_OBJECT

public:

    MainWindow(QStringList &);
		
private slots:

    void scheduler();
		void updateClientData(QString,QByteArray);
		void toggleFullScreen();
		void createContextMenu(const QPoint &);
		void saveAliens();
		
private:
  
		void setDefaults();
		void readConfig(QString);
		
		void createActions();

		void readCityDB();
		void readClientDB();

		QAction *toggleGridAction,*toggleIPsAction,*toggleFullScreenAction;
		QAction *toggleNamesAction,*toggleBordersAction,*togglePathsAction;
		QAction *saveAliensAction;
		QAction *byebyeAction;
			
    MapWidget *mapWidget; 
		QHash<QString,Client *> clients;	// client 
		QHash<QString,City *> cities;
		QList<Server *> servers;
		QList<ServerPoll *> pollers;
		QList<Client *> aliens;
		
		QString proxyServer;
		int proxyPort;
		QString proxyUser,proxyPassword;
		
		PowerManager   *powerManager;
		
		int pollInterval; 
		int updateInterval;

		bool fullScreen;

		QString citydb,clientdb;
		
		// image stuff
		QString mapFileName;
		double lat0,lon0,lat1,lon1;
		int projection;
		double xroll,yroll;
	
		// options for the map
		bool showGrid;
		bool showIP;
		bool showTraffic;
		bool showPaths;
		bool showBorders;
		bool showPlaceNames;
	
		bool anonymiseIPs;
		
		// alien tracking
		bool trackAliens;
		QString aliensFile;
		int maxAliens;
};

#endif
