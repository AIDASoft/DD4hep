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
#
# DDCMS is a detector description convention developed by the CMS experiment.
#
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
#------------------------------------------------------------------------------
dd4hep_configure_output ()
dd4hep_package (DDCMS MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector] 
  [DD4hep REQUIRED COMPONENTS DDCore DDCMS]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )

#------------------------------------------------------------------------------
dd4hep_add_plugin(DDCMSexample SOURCES src/*.cpp)
#---Package installation procedure(s) -----------------------------------------
dd4hep_install_dir( data DESTINATION ${DD4hep_DIR}/examples/DDCMS )
dd4hep_install_dir( eve  DESTINATION ${DD4hep_DIR}/examples/DDCMS )
#---Testing--------------------------------------------------------------------
dd4hep_configure_scripts ( DDCMS DEFAULT_SETUP WITH_TESTS )
#
#  Test CMS tracker detector construction
dd4hep_add_test_reg( DDCMS_LoadGeometry
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/cms_tracker.xml -destroy -print WARNING
  -plugin DD4hep_PlacedVolumeProcessor -recursive -processor DDCMS_DetElementCreator
  REGEX_PASS "Instrumented 5 subdetectors with 36091 DetElements 25776 sensitives out of 224404 volumes and 1161 sensitive placements."
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS material table
dd4hep_add_test_reg( DDCMS_DumpMaterials
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -plugin DD4hep_MaterialTable -type xml
  REGEX_PASS "material name=\"tobmaterial_TOB_ax_services_C18\""
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS volume tree
dd4hep_add_test_reg( DDCMS_DumpVolumes
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -plugin DD4hep_VolumeDump -sensitive -volids
  REGEX_PASS "Checked 224414 physical volume placements.     25776 are sensitive."
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS detector element tree
dd4hep_add_test_reg( DDCMS_DumpDetElements
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -plugin DD4hep_DetectorDump -sensitive
  REGEX_PASS "Scanned a total of 36101 elements."
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS detector element tree of SD PixelBarrel
dd4hep_add_test_reg( DDCMS_VolumeMgrTest_PixelBarrel
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -print WARNING
  -plugin DD4hep_VolumeMgrTest PixelBarrel_1
  REGEX_PASS "PASSED: Checked 10981 objects. Num.Errors:0"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS detector element tree of SD TIB
dd4hep_add_test_reg( DDCMS_VolumeMgrTest_TIB
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -print WARNING
  -plugin DD4hep_VolumeMgrTest TIB_1
  REGEX_PASS "PASSED: Checked 47964 objects. Num.Errors:0"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS detector element tree of SD TOB
#  Sucks. To be investigated. VolumeManager does not work!
#dd4hep_add_test_reg( DDCMS_VolumeMgrTest_TOB
#  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
#  EXEC_ARGS  geoPluginRun
#  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
#  -destroy -print WARNING
#  -plugin DD4hep_VolumeMgrTest TOB_1
#  REGEX_PASS "PASSED: Checked 150699 objects. Num.Errors:0"
#  REGEX_FAIL "Exception"
#  REGEX_FAIL "FAILED"
#  )
