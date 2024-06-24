echo ==================== "select qt release"

export PATH=~/Qt/6.6.2/macos/bin:$PATH
export PATH=~/Qt/6.6.2/macos/libexec:$PATH
echo using "$(which qmake)"

echo ==================== set version in qmake database

. ./VERSION
PLANTSIM_VERSION=$PLANTSIM_VERSION_MAJOR.$PLANTSIM_VERSION_MINOR
qmake -set PLANTSIM_VERSION $PLANTSIM_VERSION 
qmake -set PLANTSIM_VERSION_MAJOR $PLANTSIM_VERSION_MAJOR 
qmake -set PLANTSIM_VERSION_MINOR $PLANTSIM_VERSION_MINOR 

