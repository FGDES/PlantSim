# get a local copy of libFAUDES and configure
#
# note: this script is functional for linux/mac; it is
# meant to update FlexFact to a new version of libFAUDES;
# this script is not functional with Windows.
#

# assume libfaudes to be located next to "flexfact" in directory "common"
FAUDES_SRC=../common/libfaudes
FAUDES_DST=./libfaudes


# test for flexfact sources
if [ ! -f flexfact.pro ]; then
  echo "copyfaudes.sh: this is not a flexfact source directory ... refusing to delete files"
  return
fi

echo ==================== copy libfaudes
rm -rf $FAUDES_DST
cp -R $FAUDES_SRC $FAUDES_DST 

echo ==================== configure libfaudes

# clean
make -C $FAUDES_DST dist-clean

# configure minimal libfaudes
export OSTYPE
export FAUDES_PLUGINS="timed iodevice simulator"
export FAUDES_DEBUG="core_checked core_exceptions sim_sync core_progress iop_threads"
export FAUDES_OPTIONS="core_systime core_network core_threads"

# remove other plugins
rm -rf ${FAUDES_DST}/plugins/pushdown
rm -rf ${FAUDES_DST}/plugins/hybrid
rm -rf ${FAUDES_DST}/plugins/observability
rm -rf ${FAUDES_DST}/plugins/synthesis
rm -rf ${FAUDES_DST}/plugins/luabindings
rm -rf ${FAUDES_DST}/plugins/observer
rm -rf ${FAUDES_DST}/plugins/hiosys
rm -rf ${FAUDES_DST}/plugins/iosystem
rm -rf ${FAUDES_DST}/plugins/example
rm -rf ${FAUDES_DST}/plugins/multitasking
rm -rf ${FAUDES_DST}/plugins/diagnosis
rm -rf ${FAUDES_DST}/plugins/coordinationcontrol


# build from scratch
make -C $FAUDES_DST dist-clean
make -C $FAUDES_DST configure FAUDES_DEBUG=\\"$FAUDES_DEBUG\\"
make -C $FAUDES_DST clean

echo ==================== remove binaries/objects

rm -rf ${FAUDES_DST}/obj/*
rm -rf ${FAUDES_DST}/bin/*
rm -rf ${FAUDES_DST}/lib*faudes.a
rm -rf ${FAUDES_DST}/lib*faudes.so
rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.o
rm -rf ${FAUDES_DST}/plugins/luabindings/lib/lua-5*/src/*.a
cp ${FAUDES_DST}/doc/refsrc/tmp_flx/luafaudes.flx ${FAUDES_DST}/bin

