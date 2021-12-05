HEADERS       = MapWidget.h \
								City.h \
								Client.h \
								ClientPollRecord.h \
								PowerManager.h \
								Server.h \
								ServerPoll.h \
								MainWindow.h
SOURCES       = GLText.cpp \
								Client.cpp \
								ClientPollRecord.cpp \
								MapWidget.cpp \
                Main.cpp \
								PowerManager.cpp \
								Server.cpp \
								ServerPoll.cpp \
								MainWindow.cpp
QT           += core gui opengl xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES      += QT_NO_DEBUG_OUTPUT
LIBS         +=  -lglut -lGLU
