echo ==================== "select qt release and matching toolchain"

export PATH=/C/Qt/Tools/mingw1120_64/bin:$PATH
echo using MinGW "$(which g++)"
export PATH=/C/Qt/6.6.2/mingw_64/bin:$PATH
echo using Qt "$(which qmake)"

echo ==================== set version in qmake database

. ./VERSION
VIODES_VERSION=$VIODES_VERSION_MAJOR.$VIODES_VERSION_MINOR
qmake -set VIODES_VERSION $VIODES_VERSION 
qmake -set VIODES_VERSION_MAJOR $VIODES_VERSION_MAJOR 
qmake -set VIODES_VERSION_MINOR $VIODES_VERSION_MINOR 

