#==========================================================================
#  AIDA Detector description implementation 
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
  USES             DDCore boost
                  [ROOT REQUIRED COMPONENTS Geom]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDRec)

#---Add Library-------------------------------------------------------------
if(DD4HEP_USE_GEAR)
  add_definitions("-D DD4HEP_USE_GEAR")
endif()

dd4hep_add_dictionary( G__DDRec
  SOURCES ${DDCore_INCLUDE_DIRS}/include/ROOT/Warnings.h src/RecDictionary.h
  LINKDEF include/ROOT/LinkDef.h
)

dd4hep_add_package_library(DDRec 
  SOURCES include/ROOT/Warnings.h
  SOURCES        src/*.cpp
  GENERATED      G__DDRec.cxx
  OPTIONAL       [GEAR COMPONENTS geartgeo SOURCES src/gear/DDGear.cpp DEFINITIONS DD4HEP_USE_GEAR ]
)
dd4hep_add_plugin(DDRecPlugins 
  SOURCES  src/plugins/*.cpp 
  OPTIONAL [GEAR SOURCES src/gear/createGearForILD.cpp src/gear/createGearForCLIC.cpp src/gear/createGearForSiD.cpp DEFINITIONS DD4HEP_USE_GEAR]
)

if(DD4HEP_USE_GEAR)
  dd4hep_add_executable( convertToGear
    SOURCES src/convertToGear.cc 
    USES    [GEAR REQUIRED COMPONENTS geartgeo] )
else()
  dd4hep_add_executable( convertToGear
    OPTIONAL [GEAR COMPONENTS geartgeo SOURCES src/convertToGear.cc DEFINITIONS DD4HEP_USE_GEAR] )
endif()

dd4hep_install_files(FILES python/*.py DESTINATION python)

