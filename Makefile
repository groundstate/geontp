#############################################################################
# Makefile for building: geontp
# Generated by qmake (2.01a) (Qt 4.8.6) on: Wed Sep 9 16:26:43 2015
# Project:  geontp.pro
# Template: app
# Command: /usr/bin/qmake-qt4 -o Makefile geontp.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG_OUTPUT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtOpenGL -I/usr/include/qt4/QtXml -I/usr/include/qt4 -I/usr/X11R6/include -I.
LINK          = g++
LFLAGS        = -m64 -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -L/usr/X11R6/lib64 -lglut -lGLU -lQtXml -lQtOpenGL -lQtGui -lQtNetwork -lQtCore -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake-qt4
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = GLText.cpp \
		Client.cpp \
		ClientPollRecord.cpp \
		MapWidget.cpp \
		Main.cpp \
		PowerManager.cpp \
		Server.cpp \
		ServerPoll.cpp \
		MainWindow.cpp moc_MapWidget.cpp \
		moc_ServerPoll.cpp \
		moc_MainWindow.cpp
OBJECTS       = GLText.o \
		Client.o \
		ClientPollRecord.o \
		MapWidget.o \
		Main.o \
		PowerManager.o \
		Server.o \
		ServerPoll.o \
		MainWindow.o \
		moc_MapWidget.o \
		moc_ServerPoll.o \
		moc_MainWindow.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		geontp.pro
QMAKE_TARGET  = geontp
DESTDIR       = 
TARGET        = geontp

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: geontp.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQtXml.prl \
		/usr/lib/x86_64-linux-gnu/libQtOpenGL.prl \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtNetwork.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -o Makefile geontp.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQtXml.prl:
/usr/lib/x86_64-linux-gnu/libQtOpenGL.prl:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtNetwork.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile geontp.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/geontp1.0.0 || $(MKDIR) .tmp/geontp1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/geontp1.0.0/ && $(COPY_FILE) --parents MapWidget.h City.h Client.h ClientPollRecord.h PowerManager.h Server.h ServerPoll.h MainWindow.h .tmp/geontp1.0.0/ && $(COPY_FILE) --parents GLText.cpp Client.cpp ClientPollRecord.cpp MapWidget.cpp Main.cpp PowerManager.cpp Server.cpp ServerPoll.cpp MainWindow.cpp .tmp/geontp1.0.0/ && (cd `dirname .tmp/geontp1.0.0` && $(TAR) geontp1.0.0.tar geontp1.0.0 && $(COMPRESS) geontp1.0.0.tar) && $(MOVE) `dirname .tmp/geontp1.0.0`/geontp1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/geontp1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_MapWidget.cpp moc_ServerPoll.cpp moc_MainWindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_MapWidget.cpp moc_ServerPoll.cpp moc_MainWindow.cpp
moc_MapWidget.cpp: Globals.h \
		MapWidget.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) MapWidget.h -o moc_MapWidget.cpp

moc_ServerPoll.cpp: ServerPoll.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) ServerPoll.h -o moc_ServerPoll.cpp

moc_MainWindow.cpp: Globals.h \
		MainWindow.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) MainWindow.h -o moc_MainWindow.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

GLText.o: GLText.cpp GLText.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o GLText.o GLText.cpp

Client.o: Client.cpp Client.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Client.o Client.cpp

ClientPollRecord.o: ClientPollRecord.cpp ClientPollRecord.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ClientPollRecord.o ClientPollRecord.cpp

MapWidget.o: MapWidget.cpp City.h \
		ClientPollRecord.h \
		GLText.h \
		MapWidget.h \
		Globals.h \
		Server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MapWidget.o MapWidget.cpp

Main.o: Main.cpp MainWindow.h \
		Globals.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Main.o Main.cpp

PowerManager.o: PowerManager.cpp PowerManager.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o PowerManager.o PowerManager.cpp

Server.o: Server.cpp Server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Server.o Server.cpp

ServerPoll.o: ServerPoll.cpp ServerPoll.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ServerPoll.o ServerPoll.cpp

MainWindow.o: MainWindow.cpp City.h \
		Client.h \
		ClientPollRecord.h \
		MapWidget.h \
		Globals.h \
		PowerManager.h \
		Server.h \
		ServerPoll.h \
		MainWindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o MainWindow.o MainWindow.cpp

moc_MapWidget.o: moc_MapWidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_MapWidget.o moc_MapWidget.cpp

moc_ServerPoll.o: moc_ServerPoll.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_ServerPoll.o moc_ServerPoll.cpp

moc_MainWindow.o: moc_MainWindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_MainWindow.o moc_MainWindow.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

