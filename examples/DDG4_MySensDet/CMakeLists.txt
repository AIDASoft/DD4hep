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
dd4hep_package ( DDG4_MySensDet MAJOR 0 MINOR 0 PATCH 1
  USES  [GEANT4 REQUIRED]
  [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
  [DD4hep REQUIRED COMPONENTS DDCore DDG4]
)
set(DDG4_MySensDet_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/DDG4_MySensDet)
dd4hep_install_dir(scripts  DESTINATION ${DDG4_MySensDet_INSTALL} )
dd4hep_configure_scripts(DDG4_MySensDet DEFAULT_SETUP WITH_TESTS)
#
#---Geant4 Testsing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  dd4hep_set_compiler_flags()
  #----  Example of a client library with user defined plugins  --------------------
  dd4hep_add_plugin( DDG4_MySensDet
    SOURCES   src/*.cpp
    USES      [GEANT4 REQUIRED]
              [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
              [DD4hep REQUIRED COMPONENTS DDCore DDG4]
    )

  # Geant4 material scan. From position=0,0,0 to end-of-world 
  dd4hep_add_test_reg( DDG4_MySensDet_g4material_scan_SiliconBlock_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4_MySensDet.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/python/g4MaterialScan.py
               --compact=file:${CMAKE_INSTALL_PREFIX}/examples/ClientTests/compact/SiliconBlock.xml
               "--position=0,0,0" "--direction=0,1,0"
    REGEX_PASS " Terminate Geant4 and delete associated actions."
  )
             
  dd4hep_add_test_reg( DDG4_MySensDet_sim_SiliconBlock_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4_MySensDet.sh"
    EXEC_ARGS  python ${DDG4_MySensDet_INSTALL}/scripts/MyTrackerSD_sim.py 
               --compact=file:${CMAKE_INSTALL_PREFIX}/examples/ClientTests/compact/SiliconBlock.xml
    REGEX_PASS NONE
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )

endif()
