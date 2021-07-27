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
set(DDCADEx_INSTALL       ${CMAKE_INSTALL_PREFIX}/examples/DDCAD)
set(CLICSiDEx_INSTALL     ${CMAKE_INSTALL_PREFIX}/examples/CLICSiD)
set(ClientTestsEx_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/ClientTests)
#
# Multi-shapes: BLEND_HUMAN COB_dwarf
list(APPEND DDCAD_Tests BHV_Boxing_Toes Collada_duck PLY_Wuson )
foreach (test ${DDCAD_Tests})
  dd4hep_add_test_reg( DDCAD_Check_Shape_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
      EXEC_ARGS  geoDisplay file:${DDCAD_INSTALL}/compact/Check_Shape_${test}.xml -load -destroy
      REGEX_PASS "Shape verification SUCCESSFUL."
      REGEX_FAIL "ERROR;FAILED" )
endforeach()
#
#  Test CAD export from a simplyfied model
dd4hep_add_test_reg( DDCAD_export_sid_vertex
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoPluginRun -input ${CLICSiDEx_INSTALL}/compact/SiD_multiple_inputs.xml
  	                  -input ${CLICSiDEx_INSTALL}/compact/SiD_Vertex.xml
			  -plugin DD4hep_CAD_export -output clicsid_vertex.collada
			  -type collada -recursive -detector /world -recursive -scale 1.0 
  REGEX_PASS "Analysed 383 of 383 meshes"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
)
#
#  Test CAD import from the exported simplyfied model
dd4hep_add_test_reg( DDCAD_import_sid_vertex
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoDisplay -input ${CLICSiDEx_INSTALL}/compact/SiD_multiple_inputs.xml
  	     		-input ${DDCADEx_INSTALL}/compact/Import_SiD_Vertex.xml -load -destroy
  DEPENDS    DDCAD_export_sid_vertex
  REGEX_PASS "Read 383 meshes"
  REGEX_FAIL "Exception"
)
#
#  Test CAD export from a simplyfied model
dd4hep_add_test_reg( DDCAD_export_cal_endcaps
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoPluginRun -input ${ClientTestsEx_INSTALL}/compact/CaloEndcapReflection.xml
  -plugin DD4hep_CAD_export -output endcap_reflection.collada
  -type collada -recursive -detector /world -recursive -scale 1.0
  REGEX_PASS "Analysed 96 of 96 meshes"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
)
#
#  Test CAD import from the exported simplyfied model
dd4hep_add_test_reg( DDCAD_import_cal_endcaps
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoDisplay -input ${DDCADEx_INSTALL}/compact/Import_CaloEndcap.xml -load -destroy
  DEPENDS    DDCAD_export_cal_endcaps
  REGEX_PASS "Read 96 meshes"
  REGEX_FAIL "Exception"
)
#
#
#

