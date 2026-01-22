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
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
dd4hep_use_python_executable()
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
  set(DD4HEP_INSTALL_LIBRARY lib)
  if(CMAKE_INSTALL_LIBDIR)
    set(DD4HEP_INSTALL_LIBRARY ${CMAKE_INSTALL_LIBDIR})
  endif()
  install(FILES ${LIBRARY_OUTPUT_PATH}/G__DDG4UserDict.rootmap DESTINATION ${DD4HEP_INSTALL_LIBRARY} COMPONENT dev)
  unset(DD4HEP_INSTALL_LIBRARY)
  #
  #
  dd4hep_install_dir(data    DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDG4 )
  dd4hep_install_dir(compact DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDG4 )
  dd4hep_install_dir(scripts DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDG4 )
  #
  dd4hep_configure_scripts (DDG4 DEFAULT_SETUP WITH_TESTS)
  #
  # Test HepMC input reader
  dd4hep_add_test_reg( DDG4_sim_HepMC_reader
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DD4hep_ROOT}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/hepmc_geant4.dat
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 10 Error when moving to event -  EOF")
  #
  # Test HepMC input reader with slightly non-standard HEPMC file
  dd4hep_add_test_reg( DDG4_sim_HepMC_reader_minbias
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DD4hep_ROOT}/examples/DDG4/examples/readHEPMC.py
                      ${DDG4examples_INSTALL}/data/LHCb_MinBias_HepMC.txt
    REGEX_PASS "Geant4InputAction\\[Input\\]: Event 27 Error when moving to event -  EOF")
  #
  # Test property types with specialized action
  dd4hep_add_test_reg( DDG4_sim_Test_property_types
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/TestProperties.py
    REGEX_PASS "Test PASSED"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test material and volume properties
  dd4hep_add_test_reg( DDG4_material_volume_string_properties
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  geoDisplay file:${DDG4examples_INSTALL}/compact/Channeling.xml -load -destroy
    REGEX_PASS "7 nodes/ 3 volume UID's in Detector Geometry"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test G4 creation of G4ExtendedMaterial and G4LogicalCrystalVolume
  dd4hep_add_test_reg( DDG4_sim_G4ExtendedMaterial_G4LogicalCrystalVolume
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/Channeling.py -batch -events 3
    REGEX_PASS "Created specialize logical volume \\[G4LogicalCrystalVolume\\]: ChannelingDevice_vol"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test G4 stacking action
  dd4hep_add_test_reg( DDG4_sim_TestStackingAction
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/TestStacking.py -batch -events 3
    REGEX_PASS " \\[2\\] Calling classifyNewTrack. StackManager"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test G4 stepping action
  dd4hep_add_test_reg( DDG4_sim_TestSteppingAction
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/TestStepping.py -batch -events 3
    REGEX_PASS " Track Calls Suspended: [1-9][0-9]*"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test G4 command UI
  dd4hep_add_test_reg( DDG4_sim_UIManager
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/TestUserCommands.py
    REGEX_PASS " Parameter value at call 13 is 'terminate-command-2'"
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
  #
  # Test G4 SIGINT handler
  dd4hep_add_test_reg( DDG4_sim_SIGINT_handler
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${DDG4examples_INSTALL}/scripts/TestSIGINT.py
    REGEX_PASS "Event loop STOP signalled. Processing stops"
    REGEX_FAIL " ERROR ;EXCEPTION"
  )
  #
endif()
