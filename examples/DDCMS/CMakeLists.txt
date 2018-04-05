##==========================================================================
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
#=================================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
#------------------------------------------------------------------------------
dd4hep_configure_output ()

dd4hep_package(    DDCMS
  USES             [DD4hep REQUIRED COMPONENTS DDCore]
                   [CLHEP REQUIRED]
		   [ROOT REQUIRED COMPONENTS Geom GenVector]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDCMS)

#
# We only create only library for DDCMS. The whole package is a single component
# library. A priory there is no need to seperate the implementation from the
# plugins....
#
# If need arises, this can be changed easily.
#
#---DDCMS plugin library -------------------------------------------------------
dd4hep_add_plugin ( DDCMS SOURCES src/*.cpp src/plugins/*.cpp
  USES  [DD4hep REQUIRED COMPONENTS DDCore DDAlign DDCond]
        [ROOT REQUIRED COMPONENTS Geom GenVector])

#---Package installation procedure(s) -----------------------------------------
dd4hep_install_dir( data    DESTINATION ${DD4hep_DIR}/examples/DDCMS )
dd4hep_install_dir( eve     DESTINATION ${DD4hep_DIR}/examples/DDCMS )
dd4hep_install_dir( scripts DESTINATION ${DD4hep_DIR}/examples/DDCMS )
#
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
  REGEX_PASS "Scanned a total of 36096 elements."
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
dd4hep_add_test_reg( DDCMS_VolumeMgrTest_TOB
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -print WARNING
  -plugin DD4hep_VolumeMgrTest TOB_1
  REGEX_PASS "PASSED: Checked 150699 objects. Num.Errors:0"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Dump CMS detector element tree of SD TOB
dd4hep_add_test_reg( DDCMS_Geant4_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/CMSTrackerSim.py batch test numevents 2
  REGEX_PASS "\\+\\+\\+ Finished run 0 after 2 events \\(2 events in total\\)"
  REGEX_FAIL "Exception;EXCEPTION;ERROR;FAILED"
  )
#
#  Test saving geometry to ROOT file
dd4hep_add_test_reg( DDCMS_Persist_Save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/data/dd4hep-config.xml
  -destroy -print WARNING
  -plugin    DD4hep_Geometry2ROOT -output DDCMS_geometry.root
  REGEX_PASS "\\+\\+\\+ Successfully saved geometry data to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager
dd4hep_add_test_reg( DDCMS_Persist_Restore_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ Successfully loaded detector description from file"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager loading+nominals
dd4hep_add_test_reg( DDCMS_Persist_Restore_VolMgr1_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  -plugin    DD4hep_CheckVolumeManager
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 36096 VolumeManager contexts. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
#
#  Test restoring geometry from ROOT file: Test Volume Manager results
dd4hep_add_test_reg( DDCMS_Persist_Restore_VolMgr2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  -plugin    DD4hep_VolumeMgrTest PixelBarrel_1
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED: Checked 10981 objects. Num.Errors:0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
#
#  Test restoring geometry from ROOT file: DetElement nominal alignments
#  Note: BeamCal has a problem. Need to be taken into account
dd4hep_add_test_reg( DDCMS_Persist_Restore_Nominal_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  -plugin    DD4hep_CheckNominals
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 25776 DetElements. Num.Errors: 0"
  )
#
#  Test restoring geometry from ROOT file: Sensitive detectors
dd4hep_add_test_reg( DDCMS_Persist_Restore_Sensitives_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  -plugin    DD4hep_CheckSensitives
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 5 SensitiveDetector objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
#
#  Test restoring geometry from ROOT file: Readout structures
dd4hep_add_test_reg( DDCMS_Persist_Restore_Readouts_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDCMS.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader DDCMS_geometry.root
  -plugin    DD4hep_CheckReadouts
  DEPENDS    DDCMS_Persist_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 5 readout objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED"
  )
