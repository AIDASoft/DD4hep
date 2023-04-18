#=================================================================================
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================

add_library(DDTest INTERFACE)
add_library(DD4hep::DDTest ALIAS DDTest)

dd4hep_use_python_executable()
target_include_directories(DDTest INTERFACE include)

foreach(TEST_NAME
    test_example
    test_bitfield64
    test_bitfieldcoder
    test_DetType
    test_PolarGridRPhi2
    test_cellDimensions
    test_cellDimensionsRPhi2
    test_segmentationHandles
    test_Evaluator
    )
  add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
  target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDTest)
  install(TARGETS ${TEST_NAME} RUNTIME DESTINATION bin)

  set(cmd ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME})
  add_test(NAME t_${TEST_NAME} COMMAND ${cmd} ${TEST_NAME})
  set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endforeach()

foreach(TEST_NAME
    test_units
    test_surface
    )
  add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
  target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDTest)
  install(TARGETS ${TEST_NAME} RUNTIME DESTINATION bin)
  add_test(NAME t_${TEST_NAME}
    COMMAND ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME} file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml)
  set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endforeach()

ADD_TEST( t_test_python_import "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
  pytest ${PROJECT_SOURCE_DIR}/DDTest/python/test_import.py)
SET_TESTS_PROPERTIES( t_test_python_import PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )

if (DD4HEP_USE_GEANT4)

  ADD_TEST( t_test_python_import_ddg4 "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    pytest ${PROJECT_SOURCE_DIR}/DDTest/python/test_import_ddg4.py)
  SET_TESTS_PROPERTIES( t_test_python_import_ddg4 PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )

  if(DD4HEP_USE_HEPMC3)
    set(TEST_HEPMC3_EXTENSIONS)

    if(${DD4HEP_HEPMC3_COMPRESSION_SUPPORT})
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Pythia_output.hepmc.gz
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc
        FORMAT raw
        COMPRESSION GZip)
      list(APPEND TEST_HEPMC3_EXTENSIONS .gz)
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Pythia_output.hepmc.xz
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc
        FORMAT raw
        COMPRESSION XZ)
      list(APPEND TEST_HEPMC3_EXTENSIONS .xz)
      file(ARCHIVE_CREATE
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Pythia_output.hepmc.bz2
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc
        FORMAT raw
        COMPRESSION BZip2)
      list(APPEND TEST_HEPMC3_EXTENSIONS .bz2)
    endif()

    foreach(TEST_HEPMC3_EXTENSION "" ${TEST_HEPMC3_EXTENSIONS})
      string(REPLACE "." "_" TEST_NAME "test_ddsim_ion_hepmc3${TEST_HEPMC3_EXTENSION}")
      if("${TEST_HEPMC3_EXTENSION}" STREQUAL "")
        set(TEST_HEPMC3_INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/inputFiles/Pythia_output.hepmc${TEST_HEPMC3_EXTENSION}")
      else()
        set(TEST_HEPMC3_INPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/Pythia_output.hepmc${TEST_HEPMC3_EXTENSION}")
      endif()

      add_test( t_${TEST_NAME} "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
        ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -N=1
        --outputFile=${TEST_NAME}.root --inputFiles ${TEST_HEPMC3_INPUT_FILE}
        --part.userParticleHandler=)

      # Tests causes G4Exception about: Isomer level 9 may be ambiguous, just a warning
      # space before Exception intentional!!!
      set_tests_properties( t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION  "ERROR;Error; Exception" )
    endforeach()
  endif()

  foreach(TEST_NAME
      test_EventReaders
      )
    add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
    if(DD4HEP_USE_HEPMC3)
      target_compile_definitions(${TEST_NAME} PRIVATE DD4HEP_USE_HEPMC3)
    endif()
    if(DD4HEP_USE_LCIO)
      target_compile_definitions(${TEST_NAME} PRIVATE DD4HEP_USE_LCIO)
    endif()
    target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDG4)
    target_include_directories(${TEST_NAME} PRIVATE ./include)
    install(TARGETS ${TEST_NAME} DESTINATION bin)

    add_test(NAME t_${TEST_NAME} COMMAND ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
  endforeach(TEST_NAME)


  set(DDSIM_OUTPUT_FILES .root)

  if(DD4HEP_USE_LCIO)
    LIST(APPEND DDSIM_OUTPUT_FILES .slcio)
  endif()

  if(DD4HEP_USE_EDM4HEP)
    LIST(APPEND DDSIM_OUTPUT_FILES edm4hep.root)
  endif()

  foreach(OUTPUT_FILE IN LISTS DDSIM_OUTPUT_FILES)
    ADD_TEST( t_test_ddsim_${OUTPUT_FILE} "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
      ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -G -N=2
      --outputFile=testSid${OUTPUT_FILE}
      --gun.position \"0.0 0.0 1.0*cm\" --gun.direction \"1.0 0.0 1.0\" --gun.momentumMax 100*GeV --part.userParticleHandler=)
    SET_TESTS_PROPERTIES( t_test_ddsim_${OUTPUT_FILE} PROPERTIES FAIL_REGULAR_EXPRESSION  " Exception; EXCEPTION;ERROR;Error" )
  endforeach()

endif()
install(DIRECTORY include/DD4hep DESTINATION include)


# Test that GrammarUnparsed and GrammarParsed fail to be compiled
add_executable(test_failToCompile src/test_failToCompileGrammar.cc)
target_link_libraries(test_failToCompile DD4hep::DDCore)
# Avoid building these targets normally
set_target_properties(test_failToCompile PROPERTIES
  EXCLUDE_FROM_ALL TRUE
  EXCLUDE_FROM_DEFAULT_BUILD TRUE)

add_test(NAME TestFailToCompile
  COMMAND ${CMAKE_COMMAND} --build . --target test_failToCompile --config $<CONFIGURATION>
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(TestFailToCompile
  PROPERTIES
  PASS_REGULAR_EXPRESSION "The header files GrammarParsed.h and GrammarUnparsed.h may not be included in the same compilation unit")
