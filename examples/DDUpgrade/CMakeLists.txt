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
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#-----------------------------------------------------------------------------------
dd4hep_configure_output ()
dd4hep_package ( DDUpgrade MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
 )
#
#---DDUpgrade plugin library -------------------------------------------------------
dd4hep_add_plugin(DDUpgradePlugins
  SOURCES src/*.cpp
  USES    [ROOT    REQUIRED COMPONENTS Gdml Geom GenVector]
)
#
#
dd4hep_install_dir( data compact python DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDUpgrade )
dd4hep_install_files(FILES Upgrade.root DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDUpgrade )
#--------------------------------------------------------------------------
#
dd4hep_configure_scripts ( DDUpgrade DEFAULT_SETUP WITH_TESTS )
#
#---Testing-------------------------------------------------------------------------
#
