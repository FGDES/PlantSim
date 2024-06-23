# ##########################################
# elevator simulation project file 
# tmoor 201304
# ##########################################

# application name
unix:APPNAME = eleplant
macx:APPNAME = ElePlant
win32:APPNAME = ElePlant

# device file
DEVFILE = eleplant.dev

# set features
CONFIG += faudes   # link with libfaudes for simplenet interface

# shared sources directory
COMMON = ../common

# load common configuration
! include( $${COMMON}/faudes.pri ) {
    error("### error: faudes application configuration file not found" )
}

# application source
HEADERS      += $${COMMON}/src/ffstyle.h                 
SOURCES      += $${COMMON}/src/ffstyle.cpp                 
HEADERS      += src/eleplant.h                 
SOURCES      += src/eleplant.cpp

