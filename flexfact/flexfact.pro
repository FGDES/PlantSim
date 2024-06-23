# ##########################################
# FlexFact project file (tmoor 201301)
# ##########################################

# application name
unix:APPNAME = flexfact
macx:APPNAME = FlexFact
win32:APPNAME = FlexFact


# set flexfact features
CONFIG += faudes                       # link with libfaudes for simplenet interface
CONFIG += fullscreen                   # have osx-lion full screen mode
CONFIG += winconsole                   # have -d switch for windows console debugging

# effectively no shared common
COMMON = ./

# load generic configuration
! include( $${COMMON}/faudes.pri ) {
    error("### error: faudes application configuration file not found" )
}


# list sources
SOURCES += src/flexfact.cpp \
    src/ffworkpiece.cpp \
    src/ffview.cpp \
    src/ffstackfeeder.cpp \
    src/ffsink.cpp \
    src/fffault.cpp \
    src/ffsignal.cpp \
    src/ffscene.cpp \
    src/ffrotconv.cpp \
    src/ffrailtrans.cpp \
    src/ffprocess.cpp \
    src/ffmultimach.cpp \
    src/ffoperator.cpp \
    src/ffmonitor.cpp \
    src/ffelements.cpp \
    src/ffdistributor.cpp \
    src/ffconveyor.cpp \
    src/ffcomponent.cpp \
    src/ffstyle.cpp
HEADERS += src/flexfact.h \
    src/ffworkpiece.h \
    src/ffview.h \
    src/ffstackfeeder.h \
    src/ffsink.h \
    src/fffault.h \
    src/ffsignal.h \
    src/ffscene.h \
    src/ffrotconv.h \
    src/ffrailtrans.h \
    src/ffprocess.h \
    src/ffmultimach.h \
    src/ffoperator.h \
    src/ffmonitor.h \
    src/ffelements.h \
    src/ffdistributor.h \
    src/ffconveyor.h \
    src/ffcomponent.h \
    src/ffstyle.h
OTHER_FILES += README.txt \
    DEVELOPER.txt \
    copyfaudes.sh \
    icons/icon_win.rc \
    LICENSE.txt \
    flexfact.iss \
    distclean.sh

