# ##########################################
# ftc demo simulation project file 
# tmoor 201210
# ##########################################

# application name
unix:APPNAME = ftcplant
macx:APPNAME = FtcPlant
win32:APPNAME = FtcPlant

# set version 0.7x
isEmpty( FFVERSION_MAJOR ): FFVERSION_MAJOR = 0
isEmpty( FFVERSION_MINOR ): FFVERSION_MINOR = 8x

# device file
DEVFILE = ftcplant.dev

# set features
CONFIG += faudes                        # link with libfaudes for simplenet interface
CONFIG += fullscreen                    # support osx-lion full screen mode

# shared sources directory (local copy)
COMMON = .
COMMON_WINDIR = .

# load common configuration
! include( $${COMMON}/faudes.pri ) {
    error( " ==== error: faudes application configuration file not found" )
}

# application source
HEADERS      += $${COMMON}/src/ffstyle.h                 
HEADERS      += src/ftcplant.h                 
HEADERS      += src/ftcitems.h                 
SOURCES      += $${COMMON}/src/ffstyle.cpp
SOURCES      += src/ftcplant.cpp
SOURCES      += src/ftcitems.cpp


