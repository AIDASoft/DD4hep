#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
dd4hep_package(    DDAlign
  USES             DDCore
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDAlign)

#---DDCond library --------------------------------------------------------------
dd4hep_add_package_library(DDAlign SOURCES src/*.cpp )

#---DDAlign plugins -------------------------------------------------------------
dd4hep_add_plugin(DDAlignPlugins SOURCES src/plugins/*.cpp )
