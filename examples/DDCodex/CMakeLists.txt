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
dd4hep_package ( DDCodex MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
 )
#
#---DDCodex plugin library -------------------------------------------------------
dd4hep_add_plugin(DDCodexPlugins
  SOURCES src/*.cpp
  USES    [ROOT    REQUIRED COMPONENTS Geom GenVector]
)
#
#
dd4hep_install_dir( compact python DESTINATION ${DD4hep_DIR}/examples/DDCodex )
#--------------------------------------------------------------------------
#
dd4hep_configure_scripts ( DDCodex DEFAULT_SETUP WITH_TESTS )
#
#---Testing-------------------------------------------------------------------------
#
