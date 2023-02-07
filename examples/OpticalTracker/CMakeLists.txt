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
dd4hep_use_python_executable()

#==========================================================================
dd4hep_print("|++> OpticalTracker: ROOT version: ${ROOT_VERSION}")

if(NOT ${ROOT_VERSION} VERSION_GREATER_EQUAL 6.18.00)
  dd4hep_print("|++> Not building OpticalTracker test")
  return()
endif()
dd4hep_print("|++> Building OpticalTracker test")

#--------------------------------------------------------------------------
dd4hep_configure_output()

set(OpticalTracker_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/OpticalTracker)
dd4hep_add_plugin(OpticalTrackerExample SOURCES src/*.cpp
  USES DD4hep::DDCore DD4hep::DDRec DD4hep::DDCond ROOT::Core ROOT::Geom ROOT::GenVector ROOT::MathCore)
install(TARGETS OpticalTrackerExample LIBRARY DESTINATION lib)
install(DIRECTORY compact scripts DESTINATION ${OpticalTracker_INSTALL} )
dd4hep_configure_scripts( OpticalTracker DEFAULT_SETUP WITH_TESTS)

# ---Test: run simulation
dd4hep_add_test_reg( OpticalTracker_simulation
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalTracker.sh"
  EXEC_ARGS  ${Python_EXECUTABLE} ${OpticalTracker_INSTALL}/scripts/richsim.py
             --outputFile "${OpticalTracker_INSTALL}/sim.root"
  REGEX_PASS "TEST: passed"
  REGEX_FAIL " Exception; EXCEPTION;ERROR;Error;FATAL"
  )

# ---Test: Number of raw photon hits
dd4hep_add_test_reg( OpticalTracker_number_of_hits
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalTracker.sh"
  EXEC_ARGS  root.exe -b -x -n -q -l
             "${OpticalTracker_INSTALL}/scripts/test_number_of_hits.C(\"${OpticalTracker_INSTALL}/sim.root\")"
  REGEX_PASS "TEST: passed"
  REGEX_FAIL "TEST: failed"
  DEPENDS OpticalTracker_simulation
  )
