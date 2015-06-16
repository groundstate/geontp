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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QHash>
#include <QImage>
#include <QList>

#include "Globals.h"

#define MAXARCPTS 30

class QImage;

class City;
class ClientPollRecord;
class GLText;
class Server;

class PollEvent{

	public:

		PollEvent(){shinecnt=0;arcMade=false;}

		QString ip;
		double srclat,srclon;
		double dstlat,dstlon;
		struct timeval t;
		bool alien;

		int shinecnt; // GL stuff - current shiny texture
		bool arcMade;
		double arcx[MAXARCPTS],arcy[MAXARCPTS],arcz[MAXARCPTS];
};

class MapWidget : public QGLWidget{
	Q_OBJECT

public:

	MapWidget(QHash<QString,City *> &,QList<Server *> &,
		QString mapFile,double ullat,double ullon,double lrlat,double lrlon,int prj,double rx,double ry,
		QWidget *parent = 0, QGLWidget *shareWidget = 0);
	~MapWidget();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	enum Projection {Equirectangular,Mercator};
	
	void addNTPPoller(ClientPollRecord *,double,double);

	bool gridOn(){return showGrid;}
	bool IPsOn(){return showIP;}
	bool pathsOn(){return showPaths;}
	bool bordersOn(){return showBorders;}
	bool placeNamesOn(){return showPlaceNames;}

	void setGridOn(bool);
	void setIPsOn(bool);
	void setPathsOn(bool);
	void setBordersOn(bool);
	void setPlaceNamesOn(bool);
	
public slots:

	void toggleGrid();
	void toggleIPs();
	void togglePaths();
	void toggleBorders();
	void togglePlaceNames();

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int , int );

private slots:

	void animate();

private:

	void initTextures();;
	void setupTexture(QImage &,GLuint *);

	void makeDisplayList();
	void showPollers();
	void showTrafficPlot();
	void showStateBorders();
	void showText();

	void createArc(double,double,double,double,double *,double *,double *,int);

	void latlon2xyz(double,double,double *,double *,double *);
	
	

	// options
	bool showGrid;
	bool showIP;
	bool showTraffic;
	bool showPaths;
	bool showBorders;
	bool showPlaceNames;

	// lat/lon coordinate system
	QString mapFileName;
	int wtex,htex;
	double lat0,lon0,lat1,lon1;
	int projection;
	double rollx,rolly;
	
	GLuint mapdl; 
	GLuint griddl;
	GLuint maptex;
	
	GLText *titleText;
	
	GLuint shinetex[10]; // textures
	

	float trafficAlpha;
	struct timeval trafficT;
	float plotx,ploty,plotw,ploth,plotleftmargin,plotbottommargin; // as fractions of the viewport
	// data

	QHash<QString,City *> cities;
	QList<Server *> servers;
	QList<PollEvent *> pollers;
	QList<GLText *> ips;
	QList<GLText *> serverNames;
	
	QList<GLText *> units_;
	QList<GLText *> tens_;
	QList<GLText *> hundreds_;
	
	GLText * plotXAxisLabel_,*plotYAxisLabel_;
	
	QList<int> serverTraffic[NSERVERS];
	int currServer;
	bool transitionStarted;
	int maxips;

	//
	bool projectionDirty;
};

#endif

