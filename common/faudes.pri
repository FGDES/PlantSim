OA# ##########################################
# std faudes application 
# 
# This project file supports the configuration of Qt projects that link against
# libFAUDES, incl. deployment. It is complemented by the shell scripts "copyfaudes.sh" 
# and "distclean.sh", operational on MacX and Linux platforms. Relevant settings in
# this file are tailored for the development environment used at LRT.
#
# Referenced variables
# - APPNAME                name of application
# - DEVFILE                name of device file (located in ./src), defaults to none 
# - PLANTSIM_VERSION_MAJOR/MINOR  application version number, defaults to x.yz
# - COMMON                 shared resources (e.g. default icons, styles), defaults to ./
#
# Implemented CONFIG features
# - faudes       compile with libFAUDES
#
#
# tmoor 2024
# ##########################################


# set version: rnvironment overrules qmake overrule default 0.8x
isEmpty( PLANTSIM_VERSION_MAJOR ): PLANTSIM_VERSION_MAJOR = $$[PLANTSIM_VERSION_MAJOR]
isEmpty( PLANTSIM_VERSION_MINOR ): PLANTSIM_VERSION_MINOR = $$[PLANTSIM_VERSION_MINOR]
isEmpty( PLANTSIM_VERSION_MAJOR ): PLANTSIM_VERSION_MAJOR = 0
isEmpty( PLANTSIM_VERSION_MINOR ): PLANTSIM_VERSION_MINOR = 8x
PLANTSIM_VERSION = $${PLANTSIM_VERSION_MAJOR}.$${PLANTSIM_VERSION_MINOR}
PLANTSIM_VERSION_US = $${PLANTSIM_VERSION_MAJOR}_$${PLANTSIM_VERSION_MINOR}

# set default common location
isEmpty( COMMON ): COMMON= ./

# say hello
message("=== faudes application project" $${APPNAME})
message("=== current version" $${PLANTSIM_VERSION})
message("=== using Qt at" $$[QT_INSTALL_BINS])

# win library/support locations
WINMINGW = =C:\\qt\\MinGW482r3-sjlj     # required to compile faudes and for deployment
WININNOSU = C:\\InnoSetup5              # required for deployment only

# set debugging output
CONFIG(debug, debug|release) { 
    DEFINES += FF_DEBUG_PLANT
    DEFINES += FF_DEBUG_NETWORK
    DEFINES += FF_DEBUG_VIO
}

# set qt features
QT += core gui widgets xml network svg printsupport network

# set target
TEMPLATE = app
LANGUAGE = C++
unix:TARGET = lib/$${APPNAME}.bin
macx:TARGET = $${APPNAME}
win32:TARGET = $${APPNAME}

# set build destinations
DESTDIR = build
OBJECTS_DIR = obj
MOC_DIR = obj

# application icon (mac/win)
ICON = $${COMMON}/icons/icon_osx.icns
RC_FILE = $${COMMON}/icons/icon_win.rc


# mac deploymant
macx: QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# helper
APPNAME_LC = $$lower( $${APPNAME} )

# pass on config to compiler
DEFINES += PLANTSIM_VERSION='\\"$${PLANTSIM_VERSION}\\"'
DEFINES += PLANTSIM_APPNAME='\\"$${APPNAME}\\"'

# include common sources header
INCLUDEPATH += $${COMMON}/src

# #####################################################
# Compile with libFAUDES
# ####################################################

faudes { 
    # libfaudes location
    LIBFAUDES = ../libFAUDES_for_PlantSim

    # use libfaudes headers
    INCLUDEPATH += $${LIBFAUDES}/include
    DEPENDPATH += $${LIBFAUDES}/include
    
    # set compiletime flag
    DEFINES += FF_LIBFAUDES

    # dynamically link to libfaudes: mac os
    unix:!macx:  LIBS += -L$${LIBFAUDES} -lfaudes
    macx:        LIBS += -L$${LIBFAUDES} -lfaudes
    win32:       LIBS += $${LIBFAUDES_WINDIR}\\faudes.lib


    # mac os: care about bundle
    macx {
      # 1: copy libFAUDES
      FaudesFiles.files = $${LIBFAUDES}/libfaudes.dylib
      FaudesFiles.files += $${LIBFAUDES}/include/libfaudes.rti
      FaudesFiles.path = Contents/MacOS
      QMAKE_BUNDLE_DATA += FaudesFiles
      # 2: fix loader
      FaudesLoader.target = .fixldfaudes
      FaudesLoader.depends += $${DESTDIR}/$${APPNAME}.app/Contents/MacOS/$${APPNAME}
      FaudesLoader.commands += install_name_tool -change libfaudes.dylib @executable_path/libfaudes.dylib \
            $${DESTDIR}/$${APPNAME}.app/Contents/MacOS/$${APPNAME} &&
      FaudesLoader.commands += touch .fixldfaudes
      QMAKE_EXTRA_TARGETS += FaudesLoader
    }

}

# #####################################################
# install device file (optional)
# ####################################################

!isEmpty( DEVFILE ) {

  unix:!macx { 
    DeviceFile.target = $${DESTDIR}/lib/$${DEVFILE}
    DeviceFile.commands += cp src/$${DEVFILE} $${DESTDIR}/lib/
    QMAKE_EXTRA_TARGETS += DeviceFile
    POST_TARGETDEPS += $${DeviceFile.target}
  }
  macx { 
    DeviceFile.files = src/$${DEVFILE}
    DeviceFile.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += DeviceFile
  }
  win32 { 
    DeviceFile.target = $${DESTDIR}/$${DEVFILE}
    DeviceFile.commands += $${QMAKE_COPY} src\\$${DEVFILE} $${DESTDIR}
    QMAKE_EXTRA_TARGETS += DeviceFile
    POST_TARGETDEPS += $${DeviceFile.target}
  }

}

# #####################################################
# Deployment
# ####################################################

# extra deploy target, use std install for moving files
QMAKE_EXTRA_TARGETS += deploy
deploy.depends += install

# copy examples
InsExamples.path = $${DESTDIR}/Examples
InsExamples.files = examples/distsave/*
INSTALLS += InsExamples

# copy readme etc
InsOthers.path = $${DESTDIR}
InsOthers.files += README.txt
InsOthers.files += LICENSE.txt
INSTALLS += InsOthers

# linux deploy
unix:!macx { 
    # have a copy of qt in build/lib
    InsQtLibFiles.path = $${DESTDIR}/lib
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/libQtCore.so.$${QT_VERSION}
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/libQtGui.so.$${QT_VERSION}
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/libQtNetwork.so.$${QT_VERSION}
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/libQtSvg.so.$${QT_VERSION}
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/libQtXml.so.$${QT_VERSION}
    INSTALLS += InsQtLibFiles
    
    # cosmetic: not caps
    InsExamples.path = $${DESTDIR}/examples
    
    # have a copy of ldrun in build/bin
    QMAKE_EXTRA_TARGETS += LddrunFiles
    LddrunFiles.target = $${DESTDIR}/bin/$${APPNAME}
    LddrunFiles.commands += mkdir -p $${DESTDIR}/bin &&
    LddrunFiles.commands += mkdir -p $${DESTDIR}/lib &&
    LddrunFiles.commands += cp $${COMMON}/lddrun $${DESTDIR}/bin/$${APPNAME}
    PRE_TARGETDEPS += $${LddrunFiles.target}
    
    # have a copy of faudes executables in build/bin before creating archive
    faudes { 
        QMAKE_EXTRA_TARGETS += FaudesExeFiles
        FaudesExeFiles.target = FaudesExeFiles
        FaudesExeFiles.commands += $${QMAKE_COPY} $${LIBFAUDES}/bin/simfaudes $${DESTDIR}/bin &&
        FaudesExeFiles.commands += $${QMAKE_COPY} $${LIBFAUDES}/bin/iomonitor $${DESTDIR}/bin
    }

    # build archive
    QMAKE_EXTRA_TARGETS += lxdeployA
    LxDeployDir = $${APPNAME}_$${FFVERSION_US}
    lxdeployA.depends += install FaudesExeFiles
    lxdeployA.commands += mkdir -p $${LxDeployDir} &&
    lxdeployA.commands += cp -a $${DESTDIR}/* $${LxDeployDir} &&
    lxdeployA.commands += tar -czvf $${LxDeployDir}.tar.gz $${LxDeployDir} &&
    lxdeployA.commands += rm -rf $${LxDeployDir}
    deploy.depends += lxdeployA
}

# mac deploy
macx { 
    # fix loader to find local libfaudes
    faudes { 
        QMAKE_EXTRA_TARGETS += macdeployA
        macdeployA.depends += install
        macdeployA.commands += install_name_tool -change libfaudes.dylib @executable_path/libfaudes.dylib \
            $${DESTDIR}/$${APPNAME}.app/Contents/MacOS/$${APPNAME}
        deploy.depends += macdeployA
    }
    
    # use macdeployqt to copy qt frameworks
    QMAKE_EXTRA_TARGETS += macdeployB
    macdeployB.commands += $$[QT_INSTALL_BINS]/macdeployqt $${DESTDIR}/$${APPNAME}.app -verbose=0
    deploy.depends += macdeployB
    
    # build disk image
    QMAKE_EXTRA_TARGETS += macdeployC
    macdeployC.depends = macdeployA macdeployB
    macdeployC.commands += rm -f $${APPNAME_LC}_$${FFVERSION_US}.dmg &&
    macdeployC.commands += hdiutil create -srcfolder $${DESTDIR} -volname \
        $${APPNAME} $${APPNAME_LC}_$${FFVERSION_US}.dmg
    deploy.depends += macdeployC
}

# win deploy
win32 { 
    # have a copy of mingw libs in build
    # InsMingwFiles.path = $${DESTDIR}
    # InsMingwFiles.files += $${WINMINGW}/bin/mingwm10.dll
    # InsMingwFiles.files += $${WINMINGW}/bin/libgcc_s_dw2-1.dll
    # INSTALLS += InsMingwFiles

    # have a copy of qt in build
    InsQtLibFiles.path = $${DESTDIR}
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/../bin/QtCore4.dll
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/../bin/QtGui4.dll
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/../bin/QtNetwork4.dll
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/../bin/QtSvg4.dll
    InsQtLibFiles.files += $${QMAKE_LIBDIR_QT}/../bin/QtXml4.dll
    INSTALLS += InsQtLibFiles

    # have a copy of faudes executables in build
    faudes { 
        QMAKE_EXTRA_TARGETS += FaudesExeFiles
        FaudesExeFiles.target = FaudesExeFiles
        FaudesExeFiles.commands += $${QMAKE_COPY} $${LIBFAUDES_WINDIR}\\bin\\simfaudes.exe $${DESTDIR} &&
        FaudesExeFiles.commands += $${QMAKE_COPY} $${LIBFAUDES_WINDIR}\\bin\\iomonitor.exe $${DESTDIR}
    }
    
    # run innosetup
    exists( $${APPNAME_LC}.iss ) {
      QMAKE_EXTRA_TARGETS += windeployA
      windeployA.depends += install FaudesExeFiles
      windeployA.commands += $${WININNOSU}\\iscc $${APPNAME_LC}.iss &&
      windeployA.commands += $${QMAKE_COPY} setup.exe $${APPNAME_LC}_$${FFVERSION_US}_setup.exe &&
      windeployA.commands += $${QMAKE_DEL_FILE} setup.exe
      deploy.depends += windeployA
   }
}

