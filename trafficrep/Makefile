SHELL = /bin/bash
BINDIR = /usr/local/bin
ETCDIR  = /usr/local/etc
TARGET = trafficrep
CONFIG = trafficrep.sample.xml
GROUP = root
CPP = g++ -O2 -Wall   
CC = gcc -O2 -Wall 
INCLUDES = -I. 
RM = rm -f

#DEFINES= -DDEBUG -DLINUX_PRE_2_6_27
#LIBS= -lpthread -lpcap  -lboost_thread

DEFINES = -DDEBUG
LIBS= -lpthread -lpcap

#DEFINES=

OBJS=  Main.o TrafficReporter.o PcapThread.o HTTPThread.o \
	tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o

all: mkversion $(TARGET) 

.cpp.o:
	$(CPP) $(DEFINES) $(INCLUDES) -c  $*.cpp

mkversion:
	./mkversion.pl

$(TARGET): $(OBJS)
	$(CPP) -o $(TARGET) $(DEFINES) $(INCLUDES) $(OBJS) $(LIBS)

Main.o:Main.cpp TrafficReporter.h

TrafficReporter.o:TrafficReporter.cpp TrafficReporter.h Client.h PcapThread.h Version.h HTTPThread.h Debug.h

PcapThread.o: PcapThread.cpp PcapThread.h TrafficReporter.h Thread.h Debug.h

HTTPThread.o: HTTPThread.cpp HTTPThread.h TrafficReporter.h Thread.h Debug.h

tinystr.o: tinystr.cpp
tinyxml.o:tinyxml.cpp
tinyxmlerror.o:tinyxmlerror.cpp
tinyxmlparser.o:tinyxmlparser.cpp

install:
	cp ./$(TARGET) $(BINDIR)
	cp ./$(CONFIG) $(ETCDIR)
	@ if [[ `systemctl` =~ -\.mount ]]; then \
                echo "systemd detected"; \
                cp trafficrep.service /lib/systemd/system; \
                systemctl enable trafficrep.service; \
                echo "Edit config and start the service"; \
          else\
                echo "unsupported init system";\
          fi

clean:
	$(RM) *.o $(TARGET)
