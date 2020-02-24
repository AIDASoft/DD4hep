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
# Only newer versions of ROOT support tessellated shapes!
if(${ROOT_VERSION} VERSION_LESS 6.21.01)
  return()
endif()
#
# We need the DDCAD module to run these tests!
if(NOT TARGET DD4hep::DDCAD)
  MESSAGE(STATUS "+++> DDCAD examples shall not be built! [DDCAD not enabled]")
  return()
endif()
#
dd4hep_set_compiler_flags()
#==========================================================================
#
set(DDCAD_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/DDCAD)
dd4hep_install_dir( compact models ref DESTINATION ${DDCAD_INSTALL} )
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
dd4hep_configure_scripts( DDCAD DEFAULT_SETUP WITH_TESTS)
enable_testing ()
include(CTest)
#---  Testing  ------------------------------------------------------------
#
#
#  Test basic shapes by comparing mesh vertices with reference file
set(DDCAD_Tests)
## Multi-shapes: BLEND_HUMAN COB_dwarf
list(APPEND DDCAD_Tests BHV_Boxing_Toes Collada_duck PLY_Wuson )
foreach (test ${DDCAD_Tests})
  dd4hep_add_test_reg( DDCAD_Check_Shape_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
      EXEC_ARGS  geoDisplay file:${DDCAD_INSTALL}/compact/Check_Shape_${test}.xml -load -destroy
      REGEX_PASS "Shape verification SUCCESSFUL."
      REGEX_FAIL "ERROR;FAILED" )
endforeach()
