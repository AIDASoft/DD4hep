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
if (DD4HEP_USE_GEANT4)
  dd4hep_package(    DDTest
    USES             DDCore DDRec
    OPTIONAL         DDG4
    INCLUDE_DIRS     include
    INSTALL_INCLUDES include/DD4hep )
else()
  dd4hep_package(    DDTest
    USES             DDCore DDRec
    INCLUDE_DIRS     include
    INSTALL_INCLUDES include/DD4hep )
endif()

dd4hep_add_test_reg ( test_example             BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_units               BUILD_EXEC REGEX_FAIL "TEST_FAILED" 
  EXEC_ARGS file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml )
dd4hep_add_test_reg ( test_surface             BUILD_EXEC REGEX_FAIL "TEST_FAILED" 
  EXEC_ARGS file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml )

dd4hep_add_test_reg ( test_bitfield64          BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_bitfieldcoder       BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_DetType             BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_PolarGridRPhi2      BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_cellDimensions      BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_cellDimensionsRPhi2 BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_segmentationHandles BUILD_EXEC REGEX_FAIL "TEST_FAILED" )

if (DD4HEP_USE_GEANT4)
  dd4hep_add_test_reg ( test_EventReaders      BUILD_EXEC REGEX_FAIL "TEST_FAILED"
    EXEC_ARGS ${CMAKE_CURRENT_SOURCE_DIR} )

  ADD_TEST( test_ddsim "${CMAKE_INSTALL_PREFIX}/bin/run_test.sh"
    ddsim --compactFile=${CMAKE_INSTALL_PREFIX}/DDDetectors/compact/SiD.xml --runType=batch -G -N=2 --outputFile=testSid.root
    --part.userParticleHandler=)
  SET_TESTS_PROPERTIES( test_ddsim PROPERTIES FAIL_REGULAR_EXPRESSION  "Exception;EXCEPTION;ERROR;Error" )

endif()
