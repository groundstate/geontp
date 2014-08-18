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
QT           += opengl xml network
CONFIG       += debug
LIBS +=  -lglut -lGLU
