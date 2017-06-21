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
dd4hep_package ( DDG4 MAJOR 0 MINOR 0 PATCH 1
  USES  [DD4hep REQUIRED COMPONENTS DDCore]
)
#
#---Geant4 Testsing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  dd4hep_install_dir(data DESTINATION ${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/DDG4 )
  #
  dd4hep_configure_scripts (DDG4 DEFAULT_SETUP WITH_TESTS)
  #
  # Test HepMC input reader
  dd4hep_add_test_reg( test_DDG4_HepMC_reader
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  python ${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/DDG4/examples/readHEPMC.py
                      ${CMAKE_INSTALL_PREFIX}/share/DD4hep/examples/DDG4/data/hepmc_geant4.dat
    REQUIRES   DDG4 Geant4
    REGEX_PASS "EventReaderHepMC::moveToEvent INFO  Current event number: 9")
endif()
