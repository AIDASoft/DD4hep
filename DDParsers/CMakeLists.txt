#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
dd4hep_package(    DDParsers
  USES             [ROOT REQUIRED COMPONENTS Geom GenVector]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDParsers )

dd4hep_add_package_library ( DDParsers
  INCLUDE_DIRS
  LINK_LIBRARIES
  SOURCES        src/*.cpp src/Evaluator/*.cpp 
  OPTIONAL       [BOOST SOURCES src/parsers/*.cpp]
  )
