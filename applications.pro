# ##########################################
# libFAUDES applications project file 
# tmoor 20130402
# ##########################################

# say hello
message("=== libFAUDES applications project file")
message("=== using Qt at " $${QMAKE_LIBDIR_QT})
message("=== application version "$${FFVERSION_MAJOR}"."$${FFVERSION_MINOR})

# my template: ordered subdirs
CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS  = ./eleplant  \
           ./smallfact \
           ./ftcplant  \
           ./flexfact

