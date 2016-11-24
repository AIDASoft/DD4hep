#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
dd4hep_package(    DDCond
  USES             BOOST DDCore
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDCond)

#---DDCond library --------------------------------------------------------------
dd4hep_add_package_library(DDCond SOURCES src/*.cpp )

#---DDCond components -----------------------------------------------------------
dd4hep_add_plugin(DDCondPlugins  SOURCES  src/plugins/*.cpp )
