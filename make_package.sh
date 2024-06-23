echo ==================== compile and package all applications -- linux

# 1. run "copyfaudes.sh" (no need to set platform lsb)
# 2. edit "VERSION" in application main directory (no need to do so in flexfact/ftcplant)
# 3. set below "CLEAN=true"
# 4. run "make_package.sh"

# rebuild from scratch
#CLEAN=true
CLEAN=false

echo ==================== clean flag -- $CLEAN

# manually set qt: lsb version on destool
export VIOQT=/opt/qt-4.8.7-lsb-4.0
export PATH=$VIOQT/bin:/opt/lsb/bin:$PATH

# manually set qt: lsb version on destool
#export VIOQT=/opt/qt-4.4.2-lsb-3.2
#export PATH=$VIOQT/bin:/opt/lsb/bin:$PATH

# manually set qt: qt on destool
#export VIOQT=/opt/qtsdk-2010.01/qt
#export PATH=$VIOQT/bin:$PATH

# manually set qt: lsb version on wombat
#export VIOQT=/usr/local/Trolltech/Qt-4.4.2-lsb-3.2
#export PATH=$VIOQT/bin:/opt/lsb/bin:$PATH


# record dirs
APPDIR=$(pwd)

# retrieve version and pass to qmake
. $APPDIR/VERSION
FFVERSION=${FFVERSION_MAJOR}.${FFVERSION_MINOR}
FFVERSION_US=${FFVERSION_MAJOR}_${FFVERSION_MINOR}


# set destination
PACKAGE=faudes_applications_${FFVERSION_US}
DEST=$(pwd)/faudes_applications_${FFVERSION_US}


echo ==================== using qt $VIOQT
echo ==================== using dest $DEST

if test $CLEAN = true ; then 
echo ==================== clean all
. ./distclean.sh
fi

echo ==================== qmake
qmake "CONFIG-=debug" \"FFVERSION_MAJOR=${FFVERSION_MAJOR}\" \"FFVERSION_MINOR=${FFVERSION_MINOR}\" applications.pro

echo ==================== make
make

echo ==================== strip
strip -R .debug_aranges -R .debug_pubnames eleplant/build/lib/eleplant.bin
strip -R .debug_aranges -R .debug_pubnames smallfact/build/lib/smallfact.bin
strip -R .debug_aranges -R .debug_pubnames ftcplant/build/lib/ftcplant.bin
strip -R .debug_aranges -R .debug_pubnames flexfact/build/build/lib/flexfact.bin



echo ==================== 0. prepare package
cd $APPDIR
rm -rf $DEST
mkdir -p $DEST
mkdir -p $DEST/lib
mkdir -p $DEST/bin
mkdir -p $DEST/examples


echo ==================== 3. app scripts/bins
cd $APPDIR
cp -Rv $APPDIR/eleplant/build/bin/*  $DEST/bin
cp -Rv $APPDIR/eleplant/build/lib/*  $DEST/lib
cp -Rv $APPDIR/smallfact/build/bin/* $DEST/bin
cp -Rv $APPDIR/smallfact/build/lib/* $DEST/lib
cp -Rv $APPDIR/ftcplant/build/bin/*  $DEST/bin
cp -Rv $APPDIR/ftcplant/build/lib/*  $DEST/lib
cp -Rv $APPDIR/flexfact/build/bin/* $DEST/bin
cp -Rv $APPDIR/flexfact/build/lib/* $DEST/lib
cp -Rv $APPDIR/common/libfaudes/libfaudes.so $DEST/lib
cp -Rv $APPDIR/common/libfaudes/bin/simfaudes $DEST/bin
cp -Rv $APPDIR/common/libfaudes/bin/luafaudes $DEST/bin
cp -Rv $APPDIR/common/libfaudes/bin/iomonitor $DEST/bin
cp -Rv $APPDIR/common/libfaudes/bin/iobridge $DEST/bin


echo ==================== 3. app examples
cd $APPDIR
cp -Rv $APPDIR/eleplant/README.txt $DEST/README_eleplant.txt
cp -Rv $APPDIR/eleplant/examples/distsave/* $DEST/examples
cp -Rv $APPDIR/smallfact/README.txt $DEST/README_smallfact.txt
cp -Rv $APPDIR/smallfact/examples/distsave/* $DEST/examples
cp -Rv $APPDIR/ftcplant/README.txt $DEST/README_ftcplant.txt
cp -Rv $APPDIR/ftcplant/TECHREP.pdf $DEST/TECHREP_ftcplant.pdf
cp -Rv $APPDIR/ftcplant/examples/distsave/* $DEST/examples
cp -Rv $APPDIR/flexfact/README.txt $DEST/README_flexfact.txt
cp -Rv $APPDIR/flexfact/examples/distsave/* $DEST/examples



echo ==================== 4. other vio
cp -v $APPDIR/README.txt  $DEST
cp -v $APPDIR/LICENSE.txt $DEST

echo ==================== 5. Qt
cp -d $VIOQT/lib/libQtCore.so*       $DEST/lib
cp -d $VIOQT/lib/libQtGui.so*        $DEST/lib
cp -d $VIOQT/lib/libQtSvg.so*        $DEST/lib
cp -d $VIOQT/lib/libQtXml.so*        $DEST/lib
cp -d $VIOQT/lib/libQtNetwork.so*    $DEST/lib


echo ==================== 99. compile package
cd $APPDIR
tar -cvzf $DEST.tar.gz --exclude-from=./TAR_EXCLUDES $PACKAGE


echo ======================================== done


