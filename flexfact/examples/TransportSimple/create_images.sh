#!/bin/sh

# simple script to run tutorials, create images, and to 
# installs them in the libfaudes doxygen tree

# configure
LIBFAUDES=../../../libfaudes

DOTWRITE=$LIBFAUDES/bin/gen2dot
DOTEXEC=dot
CONVERT=convert

# advertise
echo ======================================================
echo ===  running tutorials ===============================
echo ======================================================

rm tmp_*

$LIBFAUDES/bin/luafaudes synthesis.lua


# advertise
echo ======================================================
echo ===  converting gen to png/svg/html ==================
echo ======================================================


# loop all .gen files for graphics
for FILE in tmp_*.gen ; do
  BASE=$(basename $FILE .gen)
  echo ============= processing $BASE
  $DOTWRITE $FILE
  $DOTEXEC -Tsvg  -Gsize=10,10 $BASE.dot -o $BASE.svg
  $CONVERT -background none $BASE.svg $BASE.png
done;

