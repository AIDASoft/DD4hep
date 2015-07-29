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

#-----------------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package (ClientTests MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom] 
               [DD4hep REQUIRED COMPONENTS DDCore]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )
#-----------------------------------------------------------------------------------
dd4hep_add_plugin( ClientTests SOURCES src/*.cpp  )
dd4hep_install_dir( compact scripts DESTINATION ${DD4hep_DIR}/examples/ClientTests )
#-----------------------------------------------------------------------------------
dd4hep_configure_scripts( ClientTests DEFAULT_SETUP )

#***  Testing  *********************************************************************
dd4hep_enable_tests()
#-----------------------------------------------------------------------------------
foreach (test Assemblies BoxTrafos IronCylinder LheD_tracker MagnetFields MaterialTester MiniTel SectorBarrelCalorimeter SiliconBlock NestedDetectors )
  foreach( type lcdd gdml vis )
    dd4hep_add_test_reg( ClientTests_converter_simple_${type}_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoConverter -compact2${type} -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes" )
  endforeach(type)
endforeach()
#
foreach(script MiniTel LheD_tracker )
  dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${script}.py batch
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
endforeach(script)
