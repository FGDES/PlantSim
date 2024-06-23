# remove non-source files
#
# note: this script is functional for linux/mac; it is
# meant to remove all intermeduiate files for e.g.
# source distribution or when moving platforms
#
# must be run with ftcplant as current dir (!!)

# need to manually edit faudes.pri to set version (!) 
FVERSION=0_80

TARGET=ftcplant_sources_${FVERSION}

# test for flexfact sources
if [ ! -f faudes.pri ]; then
  echo "srcpackage.sh: this is not an application source directory ... refusing to delete files"
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

