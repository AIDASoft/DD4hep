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
dd4hep_install_dir( compact models scripts ref DESTINATION ${DDCAD_INSTALL} )
#--------------------------------------------------------------------------
find_package(assimp QUIET)
MESSAGE(STATUS "ASSIMP VERSION IS ${assimp_VERSION}")
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
# Single shape tests:

#Reference for test depends on assimp version
if(assimp_VERSION VERSION_GREATER_EQUAL 5.2.0)
  set(assimp_duck_VERSION 5.2.0)
else()
  set(assimp_duck_VERSION 5.0.0)
endif()
configure_file(${CMAKE_CURRENT_LIST_DIR}/compact/Check_Shape_Collada_duck.xml.in
  ${CMAKE_CURRENT_LIST_DIR}/compact/Check_Shape_Collada_duck.xml @ONLY
)

list(APPEND DDCAD_Tests BHV_Boxing_Toes Collada_duck PLY_Wuson)
foreach (test ${DDCAD_Tests})
  dd4hep_add_test_reg( DDCAD_Check_Shape_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
      EXEC_ARGS  geoDisplay file:${DDCADEx_INSTALL}/compact/Check_Shape_${test}.xml -load -destroy
      REGEX_PASS "Shape verification SUCCESSFUL."
      REGEX_FAIL "ERROR;FAILED" )
endforeach()
#
# Multi-shape tests
# Not working: OBJ_spider
list(APPEND DDCAD_Tests_MV COB_dwarf MS3D_jeep RelativePath)
foreach (test ${DDCAD_Tests_MV})
  dd4hep_add_test_reg( DDCAD_Check_Shape_${test}
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
    EXEC_ARGS  geoDisplay file:${DDCAD_INSTALL}/compact/Check_Shape_${test}.xml -load -destroy
    REGEX_PASS "Read [0-9]([0-9]*) meshes"
    REGEX_FAIL "Exception"
  )
endforeach()
#
#  Test CAD export from a simplyfied model
dd4hep_add_test_reg( DDCAD_export_sid_vertex
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoPluginRun -input ${CLICSiDEx_INSTALL}/compact/SiD_multiple_inputs.xml
  	                  -input ${CLICSiDEx_INSTALL}/compact/SiD_Vertex.xml
			  -plugin DD4hep_CAD_export -output clicsid_vertex.collada
			  -type collada -recursive -detector /world -recursive -scale 1.0 
  REGEX_PASS "Analysed 383 out of 383 meshes"
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
  REGEX_PASS "Analysed 96 out of 96 meshes"
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
#  Test CAD export of FCC machine part
dd4hep_add_test_reg( DDCAD_export_FCC_machine
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoPluginRun -input ${ClientTestsEx_INSTALL}/compact/FCCmachine/FCCee_DectMaster.xml
  -plugin DD4hep_CAD_export -output Machine.collada
  -type collada -recursive -detector /world -recursive -scale 1.0
  REGEX_PASS "Analysed 49 out of 49 meshes"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
)
#
#  Test CAD import of FCC machine part
dd4hep_add_test_reg( DDCAD_import_FCC_machine
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoDisplay -input ${DDCADEx_INSTALL}/compact/Import_FCC_Machine.xml -load -destroy
  DEPENDS    DDCAD_export_FCC_machine
  REGEX_PASS "Read 49 meshes"
  REGEX_FAIL "Exception"
)
#
#  Geometry loading the CAD file from Armin Ilg's example
dd4hep_add_test_reg( DDCAD_Issue1134_geometry_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  geoDisplay ${DDCADEx_INSTALL}/compact/DD4hep_Issue_1134.xml -load -destroy
  REGEX_PASS "Created successfull shape of type: CAD_MultiVolume \\[CAD_MultiVolume\\]"
  REGEX_FAIL "Exception;ERROR;FAILED"
)
#
#  ROOT based Overlap check of Armin Ilg's example
dd4hep_add_test_reg( DDCAD_Issue1134_overlap_check
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
  EXEC_ARGS  checkOverlaps --compact=file:${DDCADEx_INSTALL}/compact/DD4hep_Issue_1134.xml
              --print=1 --tolerance 0.000001
  REGEX_PASS "Number of illegal overlaps/extrusions : 0"
  REGEX_FAIL "Exception;ERROR;FAILED"
)
if(TARGET DD4hep::DDG4)
  #
  #  Overlap check of Armin Ilg's example
  dd4hep_add_test_reg( DDCAD_Issue1134_g4overlap_check
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
    EXEC_ARGS  ddsim --compactFile ${DDCADEx_INSTALL}/compact/DD4hep_Issue_1134.xml
    --enableG4Gun --runType run
    --macroFile ${DDCADEx_INSTALL}/scripts/overlap.mac
    REGEX_PASS "Checking overlaps for volume AV_2.Shape_STL_0.0.tessellated_.* .G4TessellatedSolid. ... OK"
    REGEX_FAIL "EXCEPTION;ERROR;Error;FAILED"
  )
  #
  #  Geometry scan of Armin Ilg's example
  dd4hep_add_test_reg( DDCAD_Issue1134_g4geometry_scan
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCAD.sh"
    EXEC_ARGS  g4GeometryScan
    --compact=file:${DDCADEx_INSTALL}/compact/DD4hep_Issue_1134.xml
    --position=-35,-35,-300 --direction=7,7,60
    REGEX_PASS ".  -1.29,  -1.29, -11.06.  Path:./world/AV_2.Shape_STL_0.0.tessellated_.* Shape:G4TessellatedSolid  Mat:Gold"
    REGEX_FAIL "EXCEPTION;ERROR;Error;FAILED"
  )
  #
endif()
