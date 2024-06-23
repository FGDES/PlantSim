#!/bin/bash

############################################################################
# convenience script to copy, configure and compile libfaudes from source

# this script must be invoked with ./PlantSimulations as the current directory
# hence, the source and destination specification are relative to ./PlantSimulations
# a typical setup will have ./libFAUDES next to ./PlantSimulations

# edit this line to select the libfaudes source location
#FAUDES_SRC=../libfaudes-2_31h
FAUDES_SRC=../libFAUDES

# do not change the libfaudes destination
FAUDES_DST=./libFAUDES_for_PlantSim


############################################################################
# report to user
echo ==================== copyfaudes.sh
echo "copy, configure and compile libFAUDES for PlantSimulations"
echo "current directory: " $(pwd)
echo "importing libFAUDES from " $FAUDES_SRC 

# use lsb compilers on linux
#if [ $(uname -s) == "Linux" ]; then
#export FAUDES_PLATFORM="lsb_linux"
#echo override FAUDES_PLATFORM to use lsb-compilers
#fi

echo "press return to proceed or ctrl-c to bail out"
read

############################################################################
# some consistency tests
if [ ! -d flexfact ]; then
    echo "error: the current directory appears not to be a plantsimulations distribution: abort"
    return
fi
if [ ! -f plantsim.pro ]; then
    echo "error: the current directory appears not to be a plantsimulations distribution: abort"
    return
fi
if [ ! -f VERSION ]; then
    echo "error: the current directory appears not to be a plantsimulations distribution: abort"
    return
fi


############################################################################
# do it

# configure libFAUDES to go with libVIODES/DESTool 
# - minimum plug-ins: luabindings, timed, simulator, iodevice)
# - minimum debug: core_checked core_progress
# - minimum options: core_systime core_network core_thread
# - note: shared linkage is strictly required
export FAUDES_PLUGINS="synthesis observer timed iodevice simulator luabindings"
export FAUDES_DEBUG="core_checked core_exceptions sim_sync"
export FAUDES_OPTIONS="core_systime core_network core_threads"
export FAUDES_LINKING=shared


# do copy/clean/configure/compile
echo ==================== copy source tree
rm -rf $FAUDES_DST
cp -R $FAUDES_SRC $FAUDES_DST 
echo ==================== clean libFAUDES
make -C $FAUDES_DST dist-clean
echo ==================== configure libFAUDES
make -C $FAUDES_DST configure 
echo ==================== build libFAUDES shared object
make -C $FAUDES_DST -j20
echo ==================== clean libFAUDES
make -C $FAUDES_DST clean


# clean environment
unset FAUDES_PLUGINS
unset FAUDES_DEBUG
unset FAUDES_OPTIONS
unset FAUDES_LINKING
unset FAUDES_PLATFROM

#echo ==================== remove binaries/objects

#rm -rf ${FAUDES_DST}/obj/*
#rm -rf ${FAUDES_DST}/bin/*
#rm -rf ${FAUDES_DST}/libfaudes*.a
#rm -rf ${FAUDES_DST}/libfaudes*.so
#rm -rf ${FAUDES_DST}/libfaudes*.dylib
#rm -rf ${FAUDES_DST}/minfaudes.*
#rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.o
#rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.a
#cp ${FAUDES_DST}/doc/refsrc/tmp_flx/luafaudes.flx ${FAUDES_DST}/bin

#echo ==================== get extra copy for ftcplant
#cd ftcplant
#. ./copyfaudes.sh
#cd $DIR

#echo ==================== get extra copy for flexfact
#cd flexfact
#. ./copyfaudes.sh
#cd $DIR


############################################################################
# done

if [ ! -f ${FAUDES_DST}/bin/luafaudes ]; then
    echo "error: something went wrong when compiling libFAUDES: abort"
    return
fi

# update version
. ./VERSION
qmake -set PLANTSIMU_VERSION_MAJOR $VIODES_VERSION_MAJOR 
qmake -set PLANTSIMU__VERSION_MINOR $VIODES_VERSION_MINOR 


echo "===== verify versions for qt by \"qmake -query\""
echo "===== you may proceed with \"qmake plantsimu.pro\""
echo "===== copy/configure/compile libFAUDES done"

