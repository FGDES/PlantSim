# ##########################################
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
# - FFVERSION_MAJOR/MINOR  application version number, defaults to x.yz
# - COMMON                 shared resources (e.g. libfaudes, icons), defaults to ./
#
# Implemented CONFIG features
# - faudes       compile with libFAUDES
# - fullscreen   support osx-lion fullsceen mode
#
#
# tmoor 20160823
# ##########################################


# set version: rnvironment overrules qmake overrule default 0.8x
isEmpty( FFVERSION_MAJOR ): FFVERSION_MAJOR = $$[FFVERSION_MAJOR]
isEmpty( FFVERSION_MINOR ): FFVERSION_MINOR = $$[FFVERSION_MINOR]
isEmpty( FFVERSION_MAJOR ): FFVERSION_MAJOR = 0
isEmpty( FFVERSION_MINOR ): FFVERSION_MINOR = 8x
FFVERSION = $${FFVERSION_MAJOR}.$${FFVERSION_MINOR}
FFVERSION_US = $${FFVERSION_MAJOR}_$${FFVERSION_MINOR}

# set default common location
isEmpty( COMMON ): COMMON= ./

# say hello
message("=== faudes application project" $${APPNAME})
message("=== current version" $${FFVERSION})
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
QT += core gui xml network svg

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

# mac fullscreen
mac:fullscreen { 
    DEFINES += FF_OSXFULLSCREEN
    HEADERS += $${COMMON}/src/osxfullscreen.h
    OBJECTIVE_SOURCES += $${COMMON}/src/osxfullscreen.mm
    LIBS += -framework Cocoa
}

# mac deploymant
macx: QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

# helper
APPNAME_LC = $$lower( $${APPNAME} )

# pass on config to compiler
DEFINES += FF_VERSION='\\"$${FFVERSION}\\"'
DEFINES += FF_APPNAME='\\"$${APPNAME}\\"'

# include common sources header
INCLUDEPATH += $${COMMON}/src

# #####################################################
# Compile with libFAUDES
# ####################################################

faudes { 
    # libfaudes location
    LIBFAUDES = $${COMMON}/libfaudes

    # use libfaudes headers
    INCLUDEPATH += $${LIBFAUDES}/include
    DEPENDPATH += $${LIBFAUDES}/include
    
    # have targets to compile/copy libFAUDES
    QMAKE_EXTRA_TARGETS += FaudesMake FaudesFiles
    
    # set compiletime flag
    DEFINES += FF_LIBFAUDES
    
    # linux: compile/copy libFAUDES
    unix:!macx { 
        # compile
        FaudesMake.target = $${LIBFAUDES}/libfaudes.so
        FaudesMake.commands += rm -f $${LIBFAUDES}/obj/* &&
        FaudesMake.commands += make -C $${LIBFAUDES} libfaudes default
        linux-lsb-g++:FaudesMake.commands += 'FAUDES_PLATFORM=lsb_linux'
        
        # copy
        FaudesFiles.target = $${DESTDIR}/lib/libfaudes.so
        FaudesFiles.commands += cp $${LIBFAUDES}/libfaudes.so $${DESTDIR}/lib/ &&
        FaudesFiles.commands += cp $${LIBFAUDES}/include/libfaudes.rti $${DESTDIR}/lib/
        
        # lsb compiler options (deployment only)
        linux-lsb-g++:LIBS += --lsb-shared-libs=faudes

        # dynamically link to libfaudes
        LIBS += -L$${LIBFAUDES} -lfaudes
    
    }
    
    # mac: compile/copy libFAUDES
    macx { 
        # compile
        FaudesMake.target = $${LIBFAUDES}/libfaudes.dylib
        FaudesMake.commands += rm -f $${LIBFAUDES}/obj/* &&
        FaudesMake.commands += make -C $${LIBFAUDES} libfaudes default

        #copy
        FaudesFiles.files = $${LIBFAUDES}/libfaudes.dylib
        FaudesFiles.files += $${LIBFAUDES}/include/libfaudes.rti
        FaudesFiles.path = Contents/MacOS
        QMAKE_BUNDLE_DATA += FaudesFiles

        # dynamically link to libfaudes
        LIBS += -L$${LIBFAUDES} -lfaudes
    
    }
    
    # win: compile/copy libFAUDES
    win32 { 

        # needed for copy commands
        LIBFAUDES_WINDIR = $$replace(LIBFAUDES, "/", "\\")

        # compile
        FaudesMake.target = $${LIBFAUDES}/faudes.dll
        FaudesMake.commands += $${QMAKE_DEL_FILE} /Q $${LIBFAUDES_WINDIR}\\obj\\*.* &&
        FaudesMake.commands += $${WINMINGW}\\bin\\mingw32-make.exe -C $${LIBFAUDES} 'FAUDES_PLATFORM=cl_win' libfaudes default
        
        # copy (need to fix the dir sep issue)
        FaudesFiles.target = FaudesFiles
        FaudesFiles.commands += $${QMAKE_COPY} $${LIBFAUDES_WINDIR}\\faudes.dll $${DESTDIR} &&
        FaudesFiles.commands += $${QMAKE_COPY} $${LIBFAUDES_WINDIR}\\include\\libfaudes.rti $${DESTDIR}
        
        # extra libs for on win
        LIBS += -lwsock32

        # dynamically link to libfaudes
        LIBS += $${LIBFAUDES_WINDIR}\\faudes.lib
    
    }
    
    # trigger libFAUDES targets pre FlxcFact
    FaudesFiles.depend = $${FaudesMake.target}
    PRE_TARGETDEPS += $${FaudesMake.target} $${FaudesFiles.target}
}


# #####################################################
# install device file (optional)
# ####################################################

!isEmpty( DEVFILE ) {

  QMAKE_EXTRA_TARGETS += DeviceFile
  unix:!macx { 
    DeviceFile.target = $${DESTDIR}/lib/$${DEVFILE}
    DeviceFile.commands += cp src/$${DEVFILE} $${DESTDIR}/lib/
  }
  macx { 
    DeviceFile.target = $${DESTDIR}/$${TARGET}.app/Contents/MacOS/$${DEVFILE}
    DeviceFile.commands += cp src/$${DEVFILE} $${DESTDIR}/$${TARGET}.app/Contents/MacOS
  }
  win32 { 
    DeviceFile.target = $${DESTDIR}/$${DEVFILE}
    DeviceFile.commands += $${QMAKE_COPY} src\\$${DEVFILE} $${DESTDIR}
  }
  POST_TARGETDEPS += $${DeviceFile.target}

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
