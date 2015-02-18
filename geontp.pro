HEADERS       = MapWidget.h \
								City.h \
								Client.h \
								ClientPollRecord.h \
								PowerManager.h \
								ServerPoll.h \
								MainWindow.h
SOURCES       = GLText.cpp \
								Client.cpp \
								ClientPollRecord.cpp \
								MapWidget.cpp \
                Main.cpp \
								PowerManager.cpp \
								ServerPoll.cpp \
								MainWindow.cpp
QT           += core gui opengl xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG       += 
#CONFIG       += debug
LIBS +=  -lglut -lGLU
