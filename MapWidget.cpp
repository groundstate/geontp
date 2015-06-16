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

#include <QtGui>
#include <QtOpenGL>

#include <GL/glut.h>

#include <math.h>
#include <iostream>

#include <QAction>
#include <QTimer>

#include "City.h"
#include "ClientPollRecord.h"
#include "GLText.h"
#include "MapWidget.h"
#include "Server.h"

#define CHECK_GLERROR() \
{ \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) \
	{ \
		std::cerr << "[" << __FILE__ << " " << __FUNCTION__<< " " << __LINE__<< "] GL error: " << gluErrorString(err) << std::endl;\
	}\
}\

#define R 500

#define FLASHDURATION 0.5

#define DEFAULT_FONT "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"

float border[]={2,-14.883333333,129.0,-31.7,129.0, // WA
 2,-26.0,129.0,-26.0,141.0, // SA/NT
 2,-16.566666667,138.0,-26.0,138.0, // NT/QLD
 4,-26.0,141.0,-34.016,141.0,-33.983333333,140.966666667,-38.05,140.966666667,
 36,-29.0,141.0,-29.0,148.95,-28.85,149.09, -28.8,149.19,-28.7,149.35, // QLD
	-28.66,149.42,-28.58,149.52,-28.6,149.62,-28.64,149.68,-28.62,149.83,
	-28.61,149.98,-28.56,150.26,-28.64,150.42,-28.66,150.56,-28.65,150.73,
	-28.71,150.93,-28.78,151.02,-28.91,151.2,-28.98,151.28,-29.18,151.37,
	-29.01,151.51,-28.93,151.62,-28.87,151.73,-28.96,151.84,-28.88,152.03,
	-28.66,152.01,-28.53,152.02,-28.43,152.24,-28.37,152.39,-28.26,152.49,
	-28.34,152.58,-28.37,152.76,-28.36,153.11,-28.25,153.19,-28.25,153.38,
	-28.15,153.51,
21,-34.03,141.01,-34.1,141.3,-34.2,141.56,-34.14,141.89,-34.2,142.2,
	-34.53,142.39,-34.79,142.58,-34.6,142.81,-34.8,143.36,-35.2,143.39,
	-35.4,143.76,-35.93,144.43,-36.07,144.87,-35.86,145.59,-36.04,146.24,
	-36.05,147.37,-36.0,147.85,-36.38,148.05,-36.63,148.23,-36.82,148.11,
	-37.52,149.98,
	-999}; 
	

//
//
//

MapWidget::MapWidget(QHash<QString,City *> &c,QList<Server *> &s,
		QString mapFile,double ullat,double ullon,double lrlat,double lrlon,int prj,double rx,double ry,
		QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
	
	mapFileName = mapFile;
	lat0=ullat;
	lon0=ullon;
	lat1=lrlat;
	lon1=lrlon;
	projection = prj;
	rollx=rx;
	rolly=ry;
	
	mapdl =0;
	griddl=0;
	trafficAlpha=0.3;
	gettimeofday(&trafficT,NULL);
	plotx=0.15;
	ploty=0.02;
	plotw=0.4;
	ploth=0.18;
	plotleftmargin=0.033;
	plotbottommargin=0.033;
	
	showGrid=false; 
	showIP=true;
	showTraffic=false;
	showPaths=true;
	showBorders=true;
	showPlaceNames=true;

	cities=c;
	servers=s;
	currServer=0;
	transitionStarted=false;
	
	projectionDirty=true;
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
	timer->start(30);

	
}

MapWidget::~MapWidget()
{

}

QSize MapWidget::minimumSizeHint() const
{
	return QSize(80, 50);
}

QSize MapWidget::sizeHint() const
{
	return QSize(800, 500);
}


void MapWidget::addNTPPoller(ClientPollRecord *cr,double dstlat,double dstlon)
{
	// careful here if this ever runs in a separate thread
	PollEvent *pe = new PollEvent();
	pollers.push_back(pe);
	pe->ip=cr->ip;
	pe->srclat=cr->lat;
	pe->srclon=cr->lon;
	pe->dstlat=dstlat;
	pe->dstlon=dstlon;
	pe->alien=cr->alien;
	struct timeval tv;
	gettimeofday(&tv,0);
	pe->t = tv;

	QFont f;
	f.setPointSize(24);
	if (cr->alien)
		ips.push_back(new GLText(this,cr->ip,f,QColor(0,255,0,255)));
	else
		ips.push_back(new GLText(this,cr->ip,f));
	if (ips.size()>maxips)
	{
		GLText *glt = ips.takeAt(0);
		delete glt;
	}
}

void MapWidget::setGridOn(bool on){
	showGrid=on;
}

void MapWidget::setIPsOn(bool on){
	showIP=on;
}

void MapWidget::setPathsOn(bool on){
	showPaths=on;
}

void MapWidget:: setBordersOn(bool on){
	showBorders=on;
}

void MapWidget::setPlaceNamesOn(bool on){
	showPlaceNames=on;
}

	
//
//
	
void MapWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	QImage im(mapFileName);
	QImage glim = convertToGLFormat(im);
	wtex = glim.width();
	htex=glim.height();
	
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glGenTextures(1,&maptex);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,maptex);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, glim.width(), glim.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, glim.bits());
	CHECK_GLERROR();

	initTextures();
	glClearColor(56.0/255.0,101.0/255.0,152.0/255.0,0);

}

void MapWidget::paintGL()
{
 
	if (!mapdl) makeDisplayList();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glViewport(0,0,width(),height()); // set physical size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(27.0,(GLfloat) width()/(GLfloat) height(),200,-500);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
	glTranslatef(0,0,-1100);

	glRotatef(-(90 - (lat0-(lat0-lat1)/2.0))-rollx,1,0,0);
	glRotatef(270-((lon1+lon0)/2.0)-rolly,0,0,1);
	
	glPolygonMode(GL_FRONT,GL_FILL);
	glCallList(mapdl);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);

	if (showGrid)
		glCallList(griddl);

	if (showBorders)
		showStateBorders();

	if (projectionDirty){
		QHash<QString, City*>::const_iterator i = cities.constBegin();
		while (i != cities.constEnd())
		{
			City *c = i.value();
			latlon2xyz(c->lat,c->lon,&(c->x),&(c->y),&(c->z));
			++i;
		}
		projectionDirty=false;
	}
	
	glColor3f(0,0,0);
	glPointSize(5.0);
	glBegin(GL_POINTS);

	QHash<QString, City*>::const_iterator i = cities.constBegin();
	while (i != cities.constEnd()){
		City *c = i.value();
		if (c->visible)
			glVertex3f(1.001*(c->x),1.001*(c->y),1.001*(c->z));
		++i;
	}
	glEnd();
	

	showPollers();

	showText();
	
	
	
}

void MapWidget::resizeGL(int width, int height)
{
	
	projectionDirty=true;
	
	QFont font;
 	font.setPointSize(24);
	setFont(font);
	maxips = height/(fontMetrics().ascent()+5)+1;
}

void MapWidget::animate()
{
	// clean up old events first
	int j=0;
	struct timeval tnow;
	gettimeofday(&tnow,0);
	while  (j<pollers.size()){
		PollEvent *pe = pollers.at(j);
		if (tnow.tv_sec-pe->t.tv_sec   + (tnow.tv_usec - pe->t.tv_usec)/1.0E6 > FLASHDURATION){
			pollers.removeAt(j);
			delete pe;
		}
		else
			j++;
	}
	updateGL();
}

void MapWidget::toggleGrid(){
	setGridOn(!showGrid);
}

void MapWidget::toggleIPs(){
	setIPsOn(!showIP);
}

void MapWidget::togglePaths(){
	setPathsOn(!showPaths);
}

void MapWidget::toggleBorders(){
	setBordersOn(!showBorders);
}

void MapWidget::togglePlaceNames(){
	setPlaceNamesOn(!showPlaceNames);
}

//
//
//

void MapWidget::initTextures()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	for (int i=0;i<=9;i++){
		QString f=QString("textures/shine")+QString::number(i)+QString(".png");
		QImage im(f);
		setupTexture(im,&(shinetex[i]));
	}

	QFont f;
	f.setPointSize(12);
	f.setBold(true);
	
	QHash<QString, City*>::const_iterator i = cities.constBegin();
	
	while (i != cities.constEnd()){
	
		City *c = i.value();
		if (c == NULL)
			qDebug() << "NULL";
		if (c->visible){
			QString cname=c->name;
			if (c->capital)
				cname=cname.toUpper();
			c->gltext= new GLText(this,cname,f);
		}
		//std::cerr << c->gltext->w << " " << c->gltext->h << std::endl;
		++i;
	}
	
	for (int i=0;i<servers.size();i++)
			serverNames.push_back(new GLText(this,servers.at(i)->name,f));
	
	for (int i=0;i<=9;i++){
		units_.push_back(new GLText(this,QString::number(i),f));
		tens_.push_back(new GLText(this,QString::number(i*10),f)); // don't care about duplicate zero
		hundreds_.push_back(new GLText(this,QString::number(i*100),f));
	}
	plotXAxisLabel_=new GLText(this,"time (hours prior to now)",f);
	plotYAxisLabel_=new GLText(this,"requests/min",f);
	
 	f.setPointSize(36);
	f.setBold(false);
	
	titleText = new GLText(this,"NTP Server Traffic",f);

	CHECK_GLERROR();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
}

void MapWidget::setupTexture(QImage &im,GLuint *h)
{
	glGenTextures(1,h);
	glBindTexture(GL_TEXTURE_2D,*h);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, im.width(), im.height(), 0,
  //   GL_LUMINANCE, GL_UNSIGNED_BYTE, im.bits());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, im.width(), im.height(), 0,
      GL_ALPHA, GL_UNSIGNED_BYTE, im.bits());
	CHECK_GLERROR();
	
}

void MapWidget::makeDisplayList()
{
	// the texture map is lat-lon linear
	float dlat  = lat0-lat1;
	float dlon  = lon1-lon0;
	
	int nlat = 30; // n mesh pts
	int nlon = 30;

	// vertical limits of the map, in Mercator co-ordinates
	double y0=log(tan(M_PI/4.0+lat0/2.0*M_PI/180.0));
	double y1=log(tan(M_PI/4.0+lat1/2.0*M_PI/180.0));

	mapdl = glGenLists(1);
	if (mapdl)
	{
			
		glNewList(mapdl,GL_COMPILE);

		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,maptex);
		//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

		for (int i=0;i<nlat-1;i++)
		{

			double theta=M_PI/2.0 - (lat0-i*dlat/(nlat-1.0))*M_PI/180.0;
			double theta1=M_PI/2.0 - (lat0-(i+1)*dlat/(nlat-1.0))*M_PI/180.0;
			
			// texture co-ordinates to use if the map image uses the Mercator projection
			double itex,itex1;
			if (projection == Mercator){
				double ym = log(tan(M_PI/4.0 + (lat0-i*dlat/(nlat-1.0))*M_PI/180.0/2.0));
				itex = htex - (ym-y0)*htex/(y1-y0);
				double ym1 = log(tan(M_PI/4.0 + (lat0-(i+1)*dlat/(nlat-1.0))*M_PI/180.0/2.0));
				itex1 = htex - (ym1-y0)*htex/(y1-y0);
			}
			else if (projection == Equirectangular){
				itex = htex-(i*htex)/(nlat-1);
				itex1 = htex-((i+1)*htex)/(nlat-1);
			}
			glBegin(GL_TRIANGLE_STRIP);
			for (int j=0;j<nlon;j++)
			{
				double phi = (lon0 + j*dlon/(nlon-1))*M_PI/180.0;
				double x = sin(theta)*cos(phi);
				double y = sin(theta)*sin(phi);
				double z = cos(theta);
				
				glNormal3f(x,y,z); // already normalized
				glTexCoord2f((j*wtex)/(nlon-1),itex);
				glVertex3f(R*x,R*y,R*z);
				
				x = sin(theta1)*cos(phi);
				y = sin(theta1)*sin(phi);
				z = cos(theta1);
				glNormal3f(x,y,z); 
				glTexCoord2f((j*wtex)/(nlon-1),itex1);
				glVertex3f(R*x,R*y,R*z);
				
			}
			glEnd();
		}
		
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glEndList();
	}

	griddl= glGenLists(1);
	if (griddl)
	{
		int nsegs=100;
		glNewList(griddl,GL_COMPILE);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor3f(0.7,0.7,0.7);
		glLineWidth(1.0);
		double theta0 =M_PI/2.0 - lat0*M_PI/180.0;
		double dtheta=dlat/nsegs*M_PI/180.0;
		for (int lon=90;lon <=180;lon+=5)
		{
			glBegin(GL_LINE_STRIP);
			double phi = lon*M_PI/180.0;
			for (int j=0;j<=nsegs;j++)
			{
				double theta =theta0 + j*dtheta;
				double x = sin(theta)*cos(phi);
				double y = sin(theta)*sin(phi);
				double z = cos(theta);
				glVertex3f(1.001*R*x,1.001*R*y,1.001*R*z);
			}
			glEnd();
		}

		double phi0 = M_PI/2;
		double dphi= M_PI/2.0/nsegs;
		for (int lat=0;lat <= 50;lat+=5)
		{
			double theta =M_PI/2.0 + lat*M_PI/180.0;
			glBegin(GL_LINE_STRIP);
			for (int j=0;j<=nsegs;j++)
			{
				double phi =phi0 + j*dphi;
				double x = sin(theta)*cos(phi);
				double y = sin(theta)*sin(phi);
				double z = cos(theta);
				glVertex3f(1.001*R*x,1.001*R*y,1.001*R*z);
			}
			glEnd();
		}

		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glEndList();
	} 
}

void MapWidget::showPollers()
{
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glLineWidth(4.0);
	glEnable(GL_BLEND);
  //glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);

	double sz=15.0;

	struct timeval tnow;
	gettimeofday(&tnow,0);
	
	//glColor3f(1,0.549,0);
	glColor3f(1,1,0);
	for (int i=0;i<pollers.size();i++)
	{
		PollEvent *pe = pollers.at(i);
		if (pe->alien) continue;
		
		double x,y,z;
		double x2,y2,z2;

		latlon2xyz(pe->srclat,pe->srclon,&x,&y,&z);
		latlon2xyz(pe->dstlat,pe->dstlon,&x2,&y2,&z2);
		int tstep = rint((tnow.tv_sec - pe->t.tv_sec + (tnow.tv_usec - pe->t.tv_usec)/1.0E6)*FLASHDURATION*30);
	
		glPushMatrix();
		glTranslatef(x,y,z);
		double theta = acos(z/R)*180.0/M_PI;
		glRotatef(theta,-y/R,x/R,0);
	
    glBindTexture(GL_TEXTURE_2D, shinetex[pe->shinecnt]);
    pe->shinecnt = (pe->shinecnt + 1) % 10;
		
		glNormal3f(0,0,1);
		glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(sz,sz,0);
				glTexCoord2f(1.0,0.0);
				glVertex3f(-sz,sz,0);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(-sz,-sz,0);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(sz,-sz,0);
			glEnd();
		glPopMatrix();
  
	}

	glBindTexture(GL_TEXTURE_2D,0);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i=0;i<pollers.size();i++)
	{
		PollEvent *pe = pollers.at(i);
		if (pe->alien) continue;
		if ((fabs(pe->dstlat-pe->srclat)>0.1) && (fabs(pe->dstlon-pe->srclon)>0.1)) // 10 km
		{
			
			glColor4f(1,1,0,0.4);
			if (! pe->arcMade)
			{
				createArc(pe->srclat,pe->srclon,pe->dstlat,pe->dstlon,pe->arcx,pe->arcy,pe->arcz,MAXARCPTS);
				pe->arcMade=true;
			}
			glBegin(GL_LINE_STRIP);
			for (int l=0;l<MAXARCPTS;l++)
				glVertex3f(pe->arcx[l],pe->arcy[l],pe->arcz[l]);
			glEnd();
		}
	}

	glDisable(GL_BLEND);
	
}

void MapWidget::showTrafficPlot()
{
	struct timeval tnow;
	gettimeofday(&tnow,NULL);
	double dt = tnow.tv_sec - trafficT.tv_sec + (tnow.tv_usec - trafficT.tv_usec)/1.0E6;
	float rot=0.0;
	float sf=1.0;
	
	if (dt >=13 && dt < 14){
		rot=90.0*(dt-13)/1.0; // 0 to 90
	}
	else if (dt >= 14 && dt < 15){
		if (!transitionStarted){
			currServer = currServer+1;
			if (currServer == servers.size())
				currServer=0;
			transitionStarted=true;
		}
		sf=-1.0;
		rot=90.0*(dt-13.0)/1.0; // 90 to 180
	}
	else if (dt >= 15){
		trafficT=tnow;
		transitionStarted=false;
	}

	
	float x0 = plotx*width();
	float y0 = ploty*height();
	
	float pw = plotw*width();
	float ph = ploth*height();
	float plm = plotleftmargin*width();
	float pbm = plotbottommargin*height();
	
	glLineWidth(1.0);
	
	glPushMatrix();
	
	float dy=-200;
	
	glTranslatef(x0+pw/2,y0+ph/2.0,dy);
	glRotatef(rot,1,0,0);
	glScalef(1.0,sf,1.0);
	glEnable(GL_BLEND);
 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glColor4f(0.9,0.9,0.9,1.0);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex3f(-pw/2.0+plm,-ph/2.0+pbm,-1*sf);
	glVertex3f(pw/2.0,-ph/2.0+pbm,-1*sf);
	glVertex3f(-pw/2.0+plm,-ph/2.0+pbm,-1*sf);
	glVertex3f(-pw/2.0+plm,ph/2.0,-1*sf);
	glEnd();
	
	Server *srv = servers.at(currServer);
	// FIXME - completely adhoc axis scaling here
	// x tics
	double historyLength = srv->pollInterval*srv->historyLength*srv->summingLength; 
	int ntenhours = trunc((historyLength/3600.0)/10);
	if (ntenhours > 9) ntenhours=9;
	double xsf = (pw-plm)*3600.0/historyLength; // in units of hours
	double ysf = (ph - pbm)/400.0; // FIXME
	double ticlen=7.0;
	glBegin(GL_LINES);
	for (int i=0;i<=ntenhours;i++){
		float xtic=pw/2.0-i*10.0*xsf;
		glVertex3f(xtic,-ph/2.0+pbm,-1*sf);
		glVertex3f(xtic,-ph/2.0+pbm-ticlen,-1*sf);
	}
	for (int i=0;i<=4;i+=2){
		float ytic=-ph/2.0+pbm+i*100*ysf;
		glVertex3f(-pw/2.0+plm,ytic,-1*sf);
		glVertex3f(-pw/2.0+plm-ticlen,ytic,-1*sf);
	}
	glEnd();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	for (int i=0;i<=ntenhours;i++){
		float xtic=pw/2.0-i*10.0*xsf;	
		tens_[i]->paint(xtic-tens_[i]->w/2.0,-ph/2.0+pbm-tens_[i]->h,-1*sf);
	}
	for (int i=0;i<=4;i+=2){
		float xtic=-pw/2.0+plm-ticlen-hundreds_[i]->w;
		float ytic=-ph/2.0+pbm+i*100*ysf;
		hundreds_[i]->paint(xtic,ytic,-1*sf);
	}
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
	glPushMatrix();
	glTranslatef(-pw/2+plm,-ph/2+pbm,0);
	glColor4f(1,1,0,0.8);
	
	xsf = (pw - plm)/srv->historyLength; 
	
	if (srv->longHistory.size() > 1){
		glLineWidth(1.0);
		glBegin(GL_LINE_STRIP);
		for (int i=srv->longHistory.size()-1;i>=0;i--){
			//glVertex3f(i*xsf,0.0,-1*sf);
			glVertex3f((srv->historyLength-(srv->longHistory.size()-1-i))*xsf,srv->longHistory[i]*ysf,-1*sf);
		}
		glEnd();
	}
	glPopMatrix();
	
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	serverNames.at(currServer)->paint(-serverNames.at(currServer)->w/2,ph/2.0-serverNames.at(currServer)->h,-1*sf);
	plotXAxisLabel_->paint(-plotXAxisLabel_->w/2,-ph/2.0,-1*sf);
	glPushMatrix();
	glRotatef(90,0,0,1);
	plotYAxisLabel_->paint(-plotYAxisLabel_->w/2,pw/2-plotYAxisLabel_->h,-1*sf);
	glPopMatrix();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glDisable(GL_BLEND);
	
	glPopMatrix();
	//printf("%i %i %i %i\n",(int) x0,(int) y0,(int) x1,(int) y1);
}

void MapWidget::showStateBorders()
{
	int i=0;
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.0);
	glColor3f(0.3,0.3,0.3);
	double rs = 1.0001;
	while (border[i] != -999)
	{
		int npts = border[i];
		//std::cerr << "npts = " << npts << std::endl;
		if (npts == -999) break;
		i++;
		
		glBegin(GL_LINE_STRIP);
		for (int j=0;j<npts;j++)
		{
			double x,y,z;
			latlon2xyz(border[i],border[i+1],&x,&y,&z);
			if (j < npts -1)
			{
				if (fabs(border[i] - border[i+2]) > 0.5 || fabs(border[i+1] - border[i+3]) > 0.5)
				{
					glVertex3f(rs*x,rs*y,rs*z);
					float lat0=border[i];
					float lon0=border[i+1];
					float dlat = border[i+2] - border[i];
					float dlon = border[i+3] - border[i+1];
					float dl = sqrt(dlat*dlat+dlon*dlon); // length of arc
					int nsegs= (int) dl/0.1;
					float ddlat = dlat/nsegs;
					float ddlon = dlon/nsegs;
					for (int l=1;l<=nsegs;l++)
					{
						latlon2xyz(lat0+l*ddlat,lon0+l*ddlon,&x,&y,&z);
						glVertex3f(rs*x,rs*y,rs*z);
					}
					i+=2;
					if (j==npts-2) {i+=2;break;}
				}
				else
				{
					glVertex3f(rs*x,rs*y,rs*z);
					i+=2;
				}
			}
			else
			{
				glVertex3f(rs*x,rs*y,rs*z);
				i+=2;
			}
		
		}
		glEnd();
	}
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
}

void MapWidget::showText()
{
	
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	
	glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width(), 0, height(),400,-400);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (showPlaceNames)
	{
		QHash<QString, City*>::const_iterator i = cities.constBegin();
	
		while (i != cities.constEnd())
		{
			City *c = i.value();
			if (!c->visible)
			{
				++i;
				continue;
			}
			GLdouble winx,winy,winz;
			gluProject(c->x,c->y,c->z,modelMatrix,projMatrix,viewport,&winx,&winy,&winz);
			//std::cerr << winx << " " << winy << std::endl;
			int yoffset=0;
		
			
			if (c->align & Qt::AlignVCenter)
				yoffset = -(c->gltext->h - c->gltext->baseline)/2;
			
			if (c->align & Qt::AlignRight)
				c->gltext->paint(winx-c->gltext->w,winy+yoffset,-200);
			else
				c->gltext->paint(winx,winy+yoffset,-200);
			++i;
		}
	}
	
	if (showIP)
	{
		QFont font;
		font.setPointSize(24);
		QFontMetrics fm(font);

		int fh = fm.ascent();
	
		for (int i=0;i<ips.size();i++)
		{
			ips.at(i)->paint(20,height()-i*(fh+5),-200);
		}

	
	}
	
	titleText->paint(width()-titleText->w-30,height()-titleText->h,-200);
	
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	
	showTrafficPlot();
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(modelMatrix);
	

}

void  MapWidget::createArc(double lat1,double lon1,double lat2,double lon2,
	double *arcx,double *arcy,double *arcz,int npts)
{
	double x1,y1,z1; // components of R1
	double x2,y2,z2; // components of R2
	latlon2xyz(lat1,lon1,&x1,&y1,&z1);
	latlon2xyz(lat2,lon2,&x2,&y2,&z2);
	// r2-r1
	double r2r1x = x2 -x1;
	double r2r1y = y2 -y1;
	double r2r1z = z2 -z1;
	
	// r1.r2
	double r1r2 = x1*x2+y1*y2+z1*z2;
	// angle between r1 and r2
	double mr1 = sqrt(x1*x1+y1*y1+z1*z1); // ||R1||
	double mr2 = sqrt(x2*x2+y2*y2+z2*z2); // ||R2||
	double cost = r1r2/(mr1*mr2);
	double theta = acos(cost);
	
	double d = sqrt(r2r1x*r2r1x + r2r1y*r2r1y + r2r1z*r2r1z)/2.0;
	// Parabola extends from -d to +d
	// Parabola height must be greater than R*(1.0-cos(theta/2.0))
	double h = 20.0+R*(1.0-cos(theta/2.0));
	double a = h/d/d;

	// vector which passes through the middle of r2-r1
	double rcx = x1+r2r1x/2.0;
	double rcy = y1+r2r1y/2.0;
	double rcz = z1+r2r1z/2.0;
	double mrc=sqrt(rcx*rcx+rcy*rcy+rcz*rcz);
	rcx /= mrc;
	rcy /= mrc;
	rcz /= mrc; // normalize

	for (int i=0;i<npts;i++)
	{
		double t = (double) i/(double) (npts-1.0);
		double rtx = t*r2r1x;
		double rty = t*r2r1y;
		double rtz = t*r2r1z;
		
		double xi = -d + 2.0*t*d;
		double yi = -a*xi*xi+h;
		
		arcx[i] = x1 + rtx + yi* rcx;
		arcy[i] = y1 + rty + yi* rcy;
		arcz[i] = z1 + rtz + yi* rcz;
	}
	
}

void MapWidget::latlon2xyz(double lat,double lon,double *x,double *y,double *z)
{
	double theta = M_PI/2.0 - lat*M_PI/180.0;
	double phi = lon*M_PI/180.0;
	*x = R*sin(theta)*cos(phi);
	*y = R*sin(theta)*sin(phi);
	*z = R*cos(theta);
}



