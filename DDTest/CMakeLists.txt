#=================================================================================
#  $Id: $
#
#  AIDA Detector description implementation for LCD
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
dd4hep_package(    DDTest
  USES             DDCore DDRec
  OPTIONAL         DDG4
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DD4hep )

dd4hep_add_test_reg ( test_example             BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_units               BUILD_EXEC REGEX_FAIL "TEST_FAILED" 
  EXEC_ARGS file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml )
dd4hep_add_test_reg ( test_surface             BUILD_EXEC REGEX_FAIL "TEST_FAILED" 
  EXEC_ARGS file:${CMAKE_CURRENT_SOURCE_DIR}/units.xml )

dd4hep_add_test_reg ( test_bitfield64          BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_DetType             BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_PolarGridRPhi2      BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_cellDimensions      BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_cellDimensionsRPhi2 BUILD_EXEC REGEX_FAIL "TEST_FAILED" )
dd4hep_add_test_reg ( test_segmentationHandles BUILD_EXEC REGEX_FAIL "TEST_FAILED" )

if (DD4HEP_USE_GEANT4)
  dd4hep_add_test_reg ( test_EventReaders BUILD_EXEC REGEX_FAIL "TEST_FAILED"
    EXEC_ARGS ${CMAKE_CURRENT_SOURCE_DIR} )
endif()
