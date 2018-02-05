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
  USES             [ROOT REQUIRED]
  OPTIONAL         XERCESC
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/Evaluator include/Parsers )

dd4hep_add_package_library ( DDParsers
  INCLUDE_DIRS
  LINK_LIBRARIES
  SOURCES        src/Parsers/*.cpp src/Evaluator/*.cpp
  OPTIONAL       [BOOST SOURCES src/Spirit/*.cpp]
  )

