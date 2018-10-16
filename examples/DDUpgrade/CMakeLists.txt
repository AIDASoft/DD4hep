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

find_package ( DD4hep REQUIRED )

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
include(ExternalProject)
ExternalProject_Add(
    DDUpgrade_data
    PREFIX "data"
    URL http://dd4hep.web.cern.ch/dd4hep/repo_data/DDUpgrade_data.tgz
    URL_HASH SHA512=f3aeb50b104dc8e088904427403c4da11b75faf701d677ac5fe61bad206406104d92f71d6aeb69c04f470885a117b515306445fdee968c73e95a97ae37e27bbc
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    PATCH_COMMAND tar -xzf ${CMAKE_BINARY_DIR}/data/src/DDUpgrade_data.tgz -C ${CMAKE_SOURCE_DIR}
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
