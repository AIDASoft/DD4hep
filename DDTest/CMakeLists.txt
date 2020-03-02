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

if (DD4HEP_USE_GEANT4)
  foreach(TEST_NAME
      test_EventReaders
      )
    add_executable(${TEST_NAME} src/${TEST_NAME}.cc)
    target_link_libraries(${TEST_NAME} DD4hep::DDCore DD4hep::DDRec DD4hep::DDG4)
    target_include_directories(${TEST_NAME} PRIVATE ./include)
    install(TARGETS ${TEST_NAME} DESTINATION bin)

    add_test(NAME t_${TEST_NAME} COMMAND ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${TEST_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(t_${TEST_NAME} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
  endforeach(TEST_NAME)

  ADD_TEST( test_ddsim "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -G -N=2 --outputFile=testSid.root
    --gun.position \"0.0 0.0 1.0*cm\" --gun.direction \"1.0 0.0 1.0\" --gun.energy 100*GeV --part.userParticleHandler=)
  SET_TESTS_PROPERTIES( t_test_ddsim PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )

endif()
install(DIRECTORY include/DD4hep DESTINATION include)
