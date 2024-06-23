# remove non-source files
#
# note: this script is functional for linux/mac; it is
# meant to remove all intermeduiate files for e.g.
# distribution or when moving platforms
#
# must be run with flexfact as current dir (!!)

# need to manually edit common/common.pri to set version (!) 
FVERSION=0_81

TARGET=flexfact_sources_${FVERSION}

# test for flexfact sources
if [ ! -f flexfact.pro ]; then
  echo "srcpackage.sh: this is not a flexfact source directory ... refusing to delete files"
  return
fi

. ./distclean.sh

echo "creating source archieve"

rm -rf ${TARGET}*
mkdir ${TARGET}
for f in * ; do
if [ $f != $TARGET ]; then
if [ $f != ${TARGET}.tar.gz ]; then
cp -R $f  ${TARGET}
fi
fi
done
tar  -cvzf ${TARGET}.tar.gz --exclude-from=./TAR_EXCLUDES ${TARGET}

