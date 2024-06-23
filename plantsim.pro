# ##########################################
# libFAUDES PlantSim project file 
# tmoor 20130402
# ##########################################

# say hello
message("=== libFAUDES PlantSim project file")
message("=== using Qt at "$$[QT_INSTALL_BINS])
message("=== application version "$$[PLANTSIM_VERSION])

# my template: ordered subdirs
CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS  = ./eleplant  \
#           ./ftcplant  \
#           ./flexfact


