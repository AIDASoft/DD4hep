#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#--------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package (ClientTests MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector] 
               [DD4hep REQUIRED COMPONENTS DDCore]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )
#--------------------------------------------------------------------------
dd4hep_add_plugin( ClientTests SOURCES src/*.cpp
  OPTIONAL       [BOOST SOURCES src_boost/*.cpp]
  )
#
dd4hep_install_dir( compact scripts DESTINATION ${DD4hep_DIR}/examples/ClientTests )
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
dd4hep_configure_scripts( ClientTests DEFAULT_SETUP WITH_TESTS)
#---  Testing  ------------------------------------------------------------
#
#  Test JSON based parser
dd4hep_add_test_reg( ClientTests_MiniTel_JSON_Dump
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -destroy -plugin DD4hep_JsonDumper
  ${CMAKE_CURRENT_SOURCE_DIR}/compact/MiniTel.json
  REGEX_PASS "Successfully dumped json input"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test JSON based detector construction
dd4hep_add_test_reg( ClientTests_MiniTel_JSON_Detector
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hepCompactLoader
  file:${CMAKE_CURRENT_SOURCE_DIR}/compact/MiniTel_json.xml
  REGEX_PASS "Successfully processed JSON input"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test readout strings of the form: <id>system:8,barrel:-2</id>
dd4hep_add_test_reg( ClientTests_DumpElements
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -plugin DD4hepElementTable -type xml
  REGEX_PASS "formula=\"UUB\" name=\"UUB\"" )
#
#  Test long volume IDs Detector setups with placements of
#  identical volumes at different levels of the hierarchy
dd4hep_add_test_reg( ClientTests_MultiPlace
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/SiBarrelMultiSensitiveLongVolID.xml -volmgr -destroy
  -plugin DD4hepVolumeMgrTest SiTrackerBarrel

  REGEX_PASS "Volume:component1_1                                       IDDesc:OK  \\[S\\]  vid:00200668000000ff system:00ff barrel:0000 layer:0001 module:0033 sensor:0001"
  REGEX_FAIL "FAILED: World transformation DIFFER"
  )
#
#  Test long volume IDs exceeding 32 bit addressing of the form: <id>system:32,barrel:16:-5....</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_LongVoldID
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/SiBarrelMultiSensitiveLongVolID.xml -volmgr -destroy
  -plugin DD4hepVolumeMgrTest SiTrackerBarrel

  REGEX_PASS "Volume:component1_1                                       IDDesc:OK  \\[S\\]  vid:00200668000000ff system:00ff barrel:0000 layer:0001 module:0033 sensor:0001"
  REGEX_FAIL "FAILED: World transformation DIFFER"
  )
#
#  Test readout strings of the form: <id>system:8,barrel:-2</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_BarrelSides
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/Bitfield_SidesTest.xml -volmgr -destroy
  -plugin DD4hepDetectorVolumeDump
  -plugin DD4hepVolumeDump volids
  -plugin DD4hepVolumeMgrTest all
  REGEX_PASS "Volume:Shell_2                                            IDDesc:OK  \\[S\\]  vid:0000000000000102 system:0002 barrel:0001")
#
#  Test readout strings of the form: <id>system:16,barrel:16:-5</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_BarrelSides2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/Bitfield_SidesTest2.xml -volmgr -destroy
  -plugin DD4hepDetectorVolumeDump
  -plugin DD4hepVolumeDump volids
  -plugin DD4hepVolumeMgrTest all
  REGEX_PASS "Volume:Shell_2                                            IDDesc:OK  \\[S\\]  vid:0000000000010002 system:0002 barrel:0001")
#
#
foreach (test Assemblies BoxTrafos IronCylinder LheD_tracker MagnetFields MaterialTester 
              MiniTel SectorBarrelCalorimeter SiliconBlock NestedSimple NestedDetectors 
              MultiCollections MultiSegmentations )
  # Test format conversions
  foreach( type lcdd gdml vis )
    dd4hep_add_test_reg( ClientTests_${test}_converter_${type}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoConverter -compact2${type}
                              -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml
                              -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes" )
  endforeach(type)
endforeach()
#
#
# Note:
# IronCylinder has no segmentation!
# MaterialTester no geometry
# SectorBarrelCalorimeter is bad
foreach (test Assemblies BoxTrafos LheD_tracker MagnetFields MiniTel SiliconBlock 
         NestedSimple NestedDetectors MultiCollections )
  #
  # Test material scans in [origine to 10 meters in y]
  dd4hep_add_test_reg( ClientTests_${test}_material_scan
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  materialScan file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml 0 0 0 0 10000 0
    REGEX_PASS " Average Material " )
  #
  # Geant4 material scan. From position=0,0,0 to end-of-world 
  if (DD4HEP_USE_GEANT4)
    dd4hep_add_test_reg( ClientTests_${test}_g4material_scan_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${DD4hep_DIR}/python/g4MaterialScan.py
                        --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml
                        "--position=0,0,0" "--direction=0,1,0"
      REGEX_PASS " Terminate Geant4 and delete associated actions." )
  endif(DD4HEP_USE_GEANT4)
endforeach()
#
#
#
foreach (test BoxTrafos IronCylinder MiniTel SiliconBlock NestedSimple MultiCollections )
  #
  # ROOT Geometry checks
  dd4hep_add_test_reg( ClientTests_${test}_check_geometry_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/python/checkGeometry.py
                      --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml
                      --full=true --ntracks=10
    REGEX_PASS " Execution finished..." )
  #
  # ROOT Geometry overlap checks
  dd4hep_add_test_reg( ClientTests_${test}_check_overlaps_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/python/checkOverlaps.py 
                      --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml
                      --tolerance=0.1
    REGEX_PASS " Execution finished..." )
endforeach()
#
#      EXEC_ARGS  test_with_root.sh ${script}
#
if (DD4HEP_USE_GEANT4)
  #
  # Geant4 full simulation checks of simple detectors
  foreach(script Assemblies LheD_tracker MiniTel NestedDetectors )
    dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${script}.py batch
      REQUIRES   DDG4 Geant4
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
  # Geant4 full simulation checks of multi-collection/segmentation detectors
  foreach(script MultiCollections MultiSegmentations MultiSegmentCollections )
    dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/MultiCollections.py 
                        -compact ${CMAKE_CURRENT_SOURCE_DIR}/compact/${script}.xml -batch
      REQUIRES   DDG4 Geant4
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
endif(DD4HEP_USE_GEANT4)
