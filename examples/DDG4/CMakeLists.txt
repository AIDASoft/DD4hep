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
set(DDG4examples_INSTALL  ${CMAKE_INSTALL_PREFIX}/examples/DDG4)
#
#---Geant4 Testsing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  dd4hep_set_compiler_flags()
  #
  #----  Dictionary of classes to be written to the ROOT file   --------------------
  dd4hep_add_dictionary(G__DDG4UserDict
  SOURCES ${DD4hep_DIR}/include/ROOT/Warnings.h src/Dictionary.h
  LINKDEF ${DD4hep_DIR}/include/ROOT/LinkDef.h
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/lib
  )
  #----  Example of a client library with user defined plugins  --------------------
  dd4hep_add_plugin( DDG4UserLib
    GENERATED G__DDG4UserDict.cxx
    SOURCES   src/*.cpp
    USES      [GEANT4 REQUIRED]
              [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
              [DD4hep REQUIRED COMPONENTS DDCore DDG4]
    )
  #
  #
  dd4hep_install_dir(data DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDG4 )
  #
  dd4hep_configure_scripts (DDG4 DEFAULT_SETUP WITH_TESTS)
  #
  # Test HepMC input reader
  dd4hep_add_test_reg( test_DDG4_HepMC_reader
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/hepmc_geant4.dat
    REQUIRES   DDG4 Geant4
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 10 Error when moving to event -  EOF")
  # Test HepMC input reader
  dd4hep_add_test_reg( test_DDG4_HepMC_reader_minbias
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/LHCb_MinBias_HepMC.txt
    REQUIRES   DDG4 Geant4
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 27 Error when moving to event -  EOF")
endif()
