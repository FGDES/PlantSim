# get a local copy of libFAUDES and configure
#
# note: we get one copy of libfaudes for applications/common
# and two extra copies for flexfact and ftcplant

# assume libfaudes to be located next to the application directory
FAUDES_SRC=../libfaudes
FAUDES_DST=./common/libfaudes

#record dir
DIR=$(pwd)

echo ==================== copy libfaudes
rm -rf $FAUDES_DST
cp -R $FAUDES_SRC $FAUDES_DST 

echo ==================== configure libfaudes

#configure sensible libfaudes
export OSTYPE
export FAUDES_PLUGINS="synthesis observer timed iodevice simulator luabindings"
export FAUDES_DEBUG="core_checked core_exceptions sim_sync"
export FAUDES_OPTIONS="core_systime core_network core_threads"

# build from scratch
make -C $FAUDES_DST dist-clean
make -C $FAUDES_DST configure FAUDES_DEBUG=\\"$FAUDES_DEBUG\\"
make -C $FAUDES_DST clean

echo ==================== remove binaries/objects

rm -rf ${FAUDES_DST}/obj/*
rm -rf ${FAUDES_DST}/bin/*
rm -rf ${FAUDES_DST}/libfaudes*.a
rm -rf ${FAUDES_DST}/libfaudes*.so
rm -rf ${FAUDES_DST}/libfaudes*.dylib
rm -rf ${FAUDES_DST}/minfaudes.*
rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.o
rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.a
cp ${FAUDES_DST}/doc/refsrc/tmp_flx/luafaudes.flx ${FAUDES_DST}/bin


echo ==================== get extra copy for ftcplant
cd ftcplant
. ./copyfaudes.sh
cd $DIR

echo ==================== get extra copy for flexfact
cd flexfact
. ./copyfaudes.sh
cd $DIR

