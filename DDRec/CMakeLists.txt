# $Id: $
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# Author: F.Gaede (Desy)
#
#==========================================================================
dd4hep_package(DDRec
  USES             DDCore DDSurfaces
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDRec)

#---Add Library-------------------------------------------------------------
if(DD4HEP_USE_GEAR)
  add_definitions("-D DD4HEP_USE_GEAR")
endif()

dd4hep_add_package_library(DDRec 
  SOURCES        src/*.cpp 
  OPTIONAL       [GEAR COMPONENTS geartgeo SOURCES src/gear/DDGear.cpp DEFINITIONS DD4HEP_USE_GEAR ]
)
dd4hep_add_plugin(DDRecPlugins 
  SOURCES  src/plugins/*.cpp 
  OPTIONAL [GEAR SOURCES src/gear/createGearForILD.cpp src/gear/createGearForCLIC.cpp DEFINITIONS DD4HEP_USE_GEAR]
)

if(DD4HEP_USE_GEAR)
  dd4hep_add_executable( convertToGear
    SOURCES src/convertToGear.cc 
    USES    [GEAR REQUIRED COMPONENTS geartgeo] )
else()
  dd4hep_add_executable( convertToGear
    OPTIONAL [GEAR COMPONENTS geartgeo SOURCES src/convertToGear.cc DEFINITIONS DD4HEP_USE_GEAR] )
endif()


