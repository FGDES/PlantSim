echo ========= make clean
if [ -f eleplant/Makefile ]; then
  make -C eleplant clean
fi
if [ -f smallfact/Makefile ]; then
make -C smallfact clean
fi
if [ -f ftcplant/Makefile ]; then
make -C ftcplant clean
fi

echo ========= del build and obj
rm -rf eleplant/build/*
rm -rf smallfact/build/*
rm -rf ftcplant/build/*
rm -rf eleplant/obj/*
rm -rf smallfact/obj/*
rm -rf ftcplant/obj/*

echo ========= del makefiles
rm -f Makefile*
rm -f eleplant/Makefile*
rm -f smallfact/Makefile*
rm -f ftcplant/Makefile*

echo ========= clean libfaudes
rm -rf ./common/libfaudes/obj/*
rm -rf ./common/libfaudes/bin/*
rm -rf ./common/libfaudes/lib*faudes.a
rm -rf ./common/libfaudes/lib*faudes.so
rm -rf ./common/libfaudes/lib*faudes.dylib
rm -rf ./common/libfaudes/plugins/luabindings/lib/lua-5*/src/*.o
rm -rf ./common/libfaudes/plugins/luabindings/lib/lua-5*/src/*.a


echo ========= pass-on flexfact
cd flexfact
. ./distclean.sh
cd ..

echo ========= pass-on ftcplant
cd ftcplant
. ./distclean.sh
cd ..


