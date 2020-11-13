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
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================

set(DDG4examples_INSTALL  ${CMAKE_INSTALL_PREFIX}/examples/DDG4)
#
#---Geant4 Testing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #----  Dictionary of classes to be written to the ROOT file   --------------------
  dd4hep_add_dictionary(G__DDG4UserDict
  SOURCES ${DD4hep_DIR}/include/ROOT/Warnings.h src/Dictionary.h
  LINKDEF ${DD4hep_DIR}/include/ROOT/LinkDef.h
  DEFINITIONS DD4HEP_DICTIONARY_MODE
  OPTIONS -rmf ${LIBRARY_OUTPUT_PATH}/G__DDG4UserDict.rootmap -rml libDDG4UserLib.so
  OUTPUT ${LIBRARY_OUTPUT_PATH}
  )
  #----  Example of a client library with user defined plugins  --------------------
  dd4hep_add_plugin( DDG4UserLib
    SOURCES   src/*.cpp
    GENERATED G__DDG4UserDict.cxx
    USES      DD4hep::DDCore DD4hep::DDG4 Geant4::Interface ROOT::Core ROOT::Geom ROOT::GenVector ROOT::RIO
    )
  install(TARGETS DDG4UserLib LIBRARY DESTINATION lib)
  install(FILES ${LIBRARY_OUTPUT_PATH}/G__DDG4UserDict.rootmap DESTINATION lib)
  #
  #
  dd4hep_install_dir(data DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDG4 )
  #
  dd4hep_configure_scripts (DDG4 DEFAULT_SETUP WITH_TESTS)
  #
  # Test HepMC input reader
  dd4hep_add_test_reg( DDG4_HepMC_reader
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/hepmc_geant4.dat
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 10 Error when moving to event -  EOF")
  #
  # Test HepMC input reader with slightly non-standard HEPMC file
  dd4hep_add_test_reg( DDG4_HepMC_reader_minbias
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/LHCb_MinBias_HepMC.txt
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 27 Error when moving to event -  EOF")
endif()
