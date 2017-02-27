#=================================================================================
#
#  AIDA Detector description implementation for LCD
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
dd4hep_package(    DetSegmentation
  USES             [ROOT REQUIRED COMPONENTS Geom GenVector]
                   DDSegmentation
                   DDCore
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DetSegmentation)

#dd4hep_add_package_library ( DetSegmentation
#  INCLUDE_DIRS  
#  LINK_LIBRARIES
#  SOURCES        src/*.cpp 
#  )

dd4hep_add_plugin(DetSegmentation SOURCES src/*.cpp )
