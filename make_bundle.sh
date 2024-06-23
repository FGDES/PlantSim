# compile macx bundles, tmoor 201310

echo ==================== compile and package all applications for  mac osx
echo ==================== libfaudes in common/libfaudes required !! 


# do all from scratch
CLEAN=true
#CLEAN=false

# manually set qt
export QINSTALL=/usr/local/qt-4.8.7-osx11
export QTOOLS=$QINSTALL/bin
export QAPPS=$QINSTALL/bin
export QFRAMEWORKS=$QINSTALL/lib
export QPLUGINS=$QINSTALL/plugins

# record dirs
APPDIR=$(pwd)

# retrieve version 
. $APPDIR/VERSION
FFVERSION=${FFVERSION_MAJOR}.${FFVERSION_MINOR}
FFVERSION_US=${FFVERSION_MAJOR}_${FFVERSION_MINOR}

# set version in qmake
$QTOOLS/qmake -set FFVERSION_MAJOR $FFVERSION_MAJOR 
$QTOOLS/qmake -set FFVERSION_MINOR $FFVERSION_MINOR 

# set destination
DEST=faudes_applications_${FFVERSION_US}_macx

echo ==================== using version $FFVERSION
echo ==================== using qt $QTOOLS
echo ==================== using applications $APPDIR


#####################
#### clean 

echo ==================== do clean

if test $CLEAN = true ; then  
. ./distclean.sh
fi

rm -rf $DEST
mkdir $DEST

#####################
#### common files and dirs

echo ==================== do common
cp -v $APPDIR/README.txt  $DEST
cp -v $APPDIR/LICENSE.txt $DEST/LICENSE.txt
mkdir  $DEST/Examples

#####################
#### build

echo ==================== do build

$QTOOLS/qmake CONFIG-=debug FFVERSION_MAJOR=${FFVERSION_MAJOR} FFVERSION_MINOR=${FFVERSION_MINOR} applications.pro
make 
make -C flexfact deploy
make -C ftcplant deploy
make -C eleplant deploy
make -C smallfact deploy


#####################
#### copy flexfact

echo ==================== copy flexfact

BUNDLE=$(pwd)/$DEST/FlexFact.app
cd $APPDIR
cp -R flexfact/build/FlexFact.app $BUNDLE
cp $APPDIR/flexfact/libfaudes/bin/* $BUNDLE/Contents/MacOS/

echo ==================== copy examples
cp -R $APPDIR/flexfact/examples/distsave/* $DEST/Examples
cp -R $APPDIR/flexfact/README.txt $DEST/README_flexfact.txt

#####################
#### copy ftcplant

echo ==================== copy ftcplant

BUNDLE=$(pwd)/$DEST/FtcPlant.app
cd $APPDIR
cp -R ftcplant/build/FtcPlant.app $BUNDLE
cp $APPDIR/ftcplant/libfaudes/bin/* $BUNDLE/Contents/MacOS/

echo ==================== copy examples
cp -R $APPDIR/ftcplant/examples/distsave/* $DEST/Examples
cp -R $APPDIR/ftcplant/README.txt $DEST/README_ftcplant.txt
cp -R $APPDIR/ftcplant/TECHREP.pdf $DEST/TECHREP_ftcplant.pdf

#####################
#### copy eleplant

echo ==================== copy eleplant

BUNDLE=$(pwd)/$DEST/ElePlant.app
cd $APPDIR
cp -R eleplant/build/ElePlant.app $BUNDLE
cp $APPDIR/common/libfaudes/bin/* $BUNDLE/Contents/MacOS/

echo ==================== copy examples
cp -R $APPDIR/eleplant/examples/distsave/* $DEST/Examples
cp -R $APPDIR/eleplant/README.txt $DEST/README_eleplant.txt

#####################
#### copy smallfact

echo ==================== copy smallfact

BUNDLE=$(pwd)/$DEST/SmallFact.app
cd $APPDIR
cp -R smallfact/build/SmallFact.app $BUNDLE
cp $APPDIR/common/libfaudes/bin/* $BUNDLE/Contents/MacOS/

echo ==================== copy examples
cp -R $APPDIR/smallfact/examples/distsave/* $DEST/Examples
cp -R $APPDIR/smallfact/README.txt $DEST/README_smallfact.txt


#####################
#### copy libfaudes tools
mkdir $DEST/tools
cp -R $APPDIR/common/libfaudes/libfaudes.dylib $DEST/tools
cp -R $APPDIR/common/libfaudes/bin/luafaudes $DEST/tools
cp -R $APPDIR/common/libfaudes/bin/simfaudes $DEST/tools
cp -R $APPDIR/common/libfaudes/bin/iomonitor $DEST/tools
cp -R $APPDIR/common/libfaudes/bin/iobridge $DEST/tools

#####################
### image

echo ===================== 7. make disk image
cd $APPDIR
rm $DEST.dmg
hdiutil create -srcfolder $DEST -volname $DEST $DEST.dmg


echo ======================================== done
cd $APPDIR


