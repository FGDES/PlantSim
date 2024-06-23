# remove non-source files
#
# note: this script is functional for linux/mac; it is
# meant to remove all intermeduiate files for e.g.
# distribution or when moving platforms
#
# must be run with flexfact as current dir (!!)

# test for flexfact sources
if [ ! -f flexfact.pro ]; then
  echo "distclean.sh: this is not a flexfact source directory ... refusing to delete files"
  return
fi

echo "distclean.sh: deleting non-source files"

# libFAUDES
rm -f obj/*
rm -f libfaudes/obj/*
rm -f libfaudes/libfaudes.*
rm -f libfaudes/bin/*

# qt related
rm -rf build
rm -f object_script*
rm -f Makefile*

