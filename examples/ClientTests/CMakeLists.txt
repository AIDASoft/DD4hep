# $Id: $
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
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#--------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package (ClientTests MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector] 
               [DD4hep REQUIRED COMPONENTS DDCore]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )
#--------------------------------------------------------------------------
dd4hep_add_plugin( ClientTests SOURCES src/*.cpp  )
dd4hep_install_dir( compact scripts DESTINATION ${DD4hep_DIR}/examples/ClientTests )
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
dd4hep_configure_scripts( ClientTests DEFAULT_SETUP WITH_TESTS)
#---  Testing  ------------------------------------------------------------
#
#
#
foreach (test Assemblies BoxTrafos IronCylinder LheD_tracker MagnetFields MaterialTester MiniTel SectorBarrelCalorimeter SiliconBlock NestedSimple NestedDetectors )
  # Test format conversions
  foreach( type lcdd gdml vis )
    dd4hep_add_test_reg( ClientTests_${test}_converter_${type}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoConverter -compact2${type} -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes" )
  endforeach(type)
endforeach()
#
#
#
foreach (test Assemblies BoxTrafos IronCylinder LheD_tracker MagnetFields MiniTel SiliconBlock NestedSimple NestedDetectors )
  #
  # Test material scans in [origine to 10 meters in y]
  dd4hep_add_test_reg( ClientTests_${test}_material_scan
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  materialScan file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml 0 0 0 0 10000 0
    REGEX_PASS " Average Material " )
  #
  # Geant4 material scan. From position=0,0,0 to end-of-world 
  if (DD4HEP_USE_GEANT4)
    dd4hep_add_test_reg( ClientTests_${test}_g4material_scan
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${CMAKE_INSTALL_PREFIX}/python/g4MaterialScan.py --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml "--position=0,0,0" "--direction=0,1,0"
      REGEX_PASS " Terminate Geant4 and delete associated actions." )
  endif(DD4HEP_USE_GEANT4)
endforeach()
#
#
#
foreach (test BoxTrafos IronCylinder MiniTel SiliconBlock NestedSimple )
  #
  # ROOT Geometry checks
  dd4hep_add_test_reg( ClientTests_${test}_check_geometry
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${CMAKE_INSTALL_PREFIX}/python/checkGeometry.py --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml --full=true --ntracks=10
    REGEX_PASS " Execution finished..." )
  #
  # ROOT Geometry overlap checks
  dd4hep_add_test_reg( ClientTests_${test}_check_overlaps
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${CMAKE_INSTALL_PREFIX}/python/checkOverlaps.py --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml --tolerance=0.1
    REGEX_PASS " Execution finished..." )
endforeach()
#
#      EXEC_ARGS  test_with_root.sh ${script}
#
if (DD4HEP_USE_GEANT4)
  #
  # Geant4 full simulation checks of simple detectors
  foreach(script MiniTel LheD_tracker)
    dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${script}.py batch
      REQUIRES   DDG4 Geant4
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
endif(DD4HEP_USE_GEANT4)
