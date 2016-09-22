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
#==========================================================================
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#-----------------------------------------------------------------------------------
dd4hep_configure_output ()
dd4hep_package ( CLICSiD MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
)
dd4hep_install_dir( xml DESTINATION ${DD4hep_DIR}/examples/Conditions )
#--------------------------------------------------------------------------
#if (DD4HEP_USE_GEANT4)
#  dd4hep_add_executable(CLICSiDXML SOURCES scripts/CLICSiDXML.C
#    USES  [DD4hep REQUIRED COMPONENTS DDG4] GEANT4 )
#  #--------------------------------------------------------------------------
#  dd4hep_add_executable(CLICSiDAClick SOURCES scripts/CLICSiDAClick.C
#    USES  [DD4hep REQUIRED COMPONENTS DDG4] GEANT4 )
#endif()
#
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
