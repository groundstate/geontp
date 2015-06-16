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

#include <iostream>

#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QGLWidget>

#include "GLText.h"

GLText::GLText(QGLWidget *glx,QString s,QFont f,QColor color){
	QFontMetrics fm(f);
	w = fm.width(s);
	h = fm.height();
	baseline = fm.descent(); // in OpenGL window coordinates
	
	QImage im(w,h,QImage::Format_ARGB32);
	im.fill(0);
	QPainter p;
	p.begin(&im);
	p.setPen(color);
	p.setFont(f);
	p.drawText(0,h-fm.descent(),s);
	
	
	QImage glim = glx->convertToGLFormat(im);

	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,texture);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, glim.width(), glim.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, glim.bits());
			
	//CHECK_GLERROR();
}

GLText::~GLText(){
	if (texture) {glDeleteTextures(1,&texture);}
}

void GLText::paint(int x,int y,int z){
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,texture);
	
	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0, 0.0);
	glVertex3f(x,y-baseline,z);
	
	glTexCoord2f(w,0.0);
	glVertex3f(x+w,y-baseline,z);
	
	glTexCoord2f(w, h);
	glVertex3f(x+w,y+h-baseline,z);
	
	glTexCoord2f(0, h);
	glVertex3f(x,y+h-baseline,z);
	
	glEnd();
}
