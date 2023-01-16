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
#
IF(NOT TARGET DD4hep::DDDigi)
  MESSAGE(STATUS "+++> DDDigi examples shall not be built! [DDDigi not enabled]")
  return()
ENDIF()
#
dd4hep_set_compiler_flags()
dd4hep_use_python_executable()
#==========================================================================
dd4hep_configure_output ()
#
set(DDDigiexamples_INSTALL  ${CMAKE_INSTALL_PREFIX}/examples/DDDigi)
#
dd4hep_add_plugin(DDDigiExampleLib SOURCES src/*.cpp )
target_link_libraries(DDDigiExampleLib DD4hep::DDDigi Boost::boost ROOT::Geom ROOT::GenVector ROOT::RIO ROOT::Gui ROOT::Hist)
install(TARGETS DDDigiExampleLib LIBRARY DESTINATION lib)
install(DIRECTORY scripts DESTINATION ${DDDigiexamples_INSTALL})
#
dd4hep_configure_scripts (DDDigi DEFAULT_SETUP WITH_TESTS)
#
# Test basic processing chain
dd4hep_add_test_reg(DDDigi_framework
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
  EXEC_ARGS  ${Python_EXECUTABLE} ${DDDigiexamples_INSTALL}/scripts/TestFramework.py
  REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed."
  REGEX_FAIL "Error;ERROR;Exception"
  )
#
# Test colored noise factory
dd4hep_add_test_reg(DDDigi_colored_noise
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
  EXEC_ARGS  geoPluginRun -ui -plugin DD4hep_FalphaNoise -shots 1000000 -variance 1 -alpha 0.5
  DEPENDS    DDDigi_framework
  REGEX_PASS "FalphaNoise      INFO  Distribution  RMS                 1.0"
  REGEX_FAIL "Error;ERROR;Exception"
  )
#
# Test new properties
dd4hep_add_test_reg(DDDigi_properties
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
  EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestProperties.py
  REGEX_PASS "roperty test PASSED"
  REGEX_FAIL "Error;ERROR;Exception"
)
#
if (DD4HEP_USE_GEANT4)
  # Generate test data
  dd4hep_add_test_reg(DDDigi_generate_ddg4_data
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/ClientTests/scripts/MiniTelGenerate.py
         -batch -events 30 -runs 8
    REGEX_PASS "\\+\\+\\+ Finished run 7 after 30 events \\(240 events in total\\)."
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test basic input reading from DDG4
  dd4hep_add_test_reg(DDDigi_test_input_reading
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestInput.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test DDDigi exception while processing
  dd4hep_add_test_reg(DDDigi_test_processing_exception
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestInput.py
               -num_events 1000
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+ Terminate Digi and delete associated actions."
  )
  # Test signal attenuation for spillover
  dd4hep_add_test_reg(DDDigi_test_attenuate
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestAttenuate.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test moving IP
  dd4hep_add_test_reg(DDDigi_test_move_IP
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestIPMove.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test deposit count
  dd4hep_add_test_reg(DDDigi_test_deposit_count
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestDepositCount.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test weighted deposit overlay
  dd4hep_add_test_reg(DDDigi_test_weighted_deposit_overlay
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestDepositWeighted.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test deposit energy smearing
  dd4hep_add_test_reg(DDDigi_test_deposit_smear_energy
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestDepositSmearEnergy.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test deposit time resolution smearing
  dd4hep_add_test_reg(DDDigi_test_deposit_smear_time
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestDepositSmearTime.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test deposit position resolution smearing
  dd4hep_add_test_reg(DDDigi_test_smear_position
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestPositionSmearResolution.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test deposit track position resolution smearing
  dd4hep_add_test_reg(DDDigi_test_smear_track
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestPositionSmearTrack.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test multiple interaction input
  dd4hep_add_test_reg(DDDigi_test_multi_interactions
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestMultiInteractions.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test spillover input (multi interactions with attenuation)
  dd4hep_add_test_reg(DDDigi_test_spillover
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestSpillover.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test container parellization
  dd4hep_add_test_reg(DDDigi_test_containers_parallel
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestMultiContainerParallel.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test hit resegmentation
  dd4hep_add_test_reg(DDDigi_test_detector_resegmentation
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestResegmentation.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test work splitting by segmentation
  dd4hep_add_test_reg(DDDigi_test_segmentation_split_1
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestSegmentationSplit.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test work splitting by segmentation (2)
  dd4hep_add_test_reg(DDDigi_test_segmentation_split_2
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestSegmentationSplit2.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  # Test simple ADC response
  dd4hep_add_test_reg(DDDigi_test_simple_adc_response
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestSimpleADCResponse.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  #
  # Test raw digi write
  dd4hep_add_test_reg(DDDigi_test_digi_root_write
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
    EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestWriteDigi.py
    DEPENDS    DDDigi_generate_ddg4_data
    REGEX_PASS "\\+\\+\\+ Closing ROOT output file dddigi_write_digi_00000000.root after 5 events"
    REGEX_FAIL "Error;ERROR;Exception"
  )
  #
  # Test EDM4HEP output module
  if (DD4HEP_USE_EDM4HEP)
    # Generate test data
    dd4hep_add_test_reg(DDDigi_generate_edm4hep_data
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
      EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/ClientTests/scripts/MiniTelGenerate.py
      -batch -events 30 -runs 3 -output MiniTel_edm4hep_DDG4_data.root
      REGEX_PASS "\\+\\+\\+ Finished run 2 after 30 events \\(90 events in total\\)."
      REGEX_FAIL "Error;ERROR;Exception"
    )
    # Test EDM4HEP write from ddg4 input (needs to be expanded)
    dd4hep_add_test_reg(DDDigi_test_edm4hep_write
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDigi.sh"
      EXEC_ARGS  ${Python_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/examples/DDDigi/scripts/TestWriteEdm4hep.py
                   -num_events 5 -num_threads 10 -events_parallel 4
      DEPENDS    DDDigi_generate_ddg4_data
      REGEX_PASS "\\+\\+\\+ 5 Events out of 5 processed."
      REGEX_FAIL "Error;ERROR;Exception"
    )
  endif()
#
endif()
#
