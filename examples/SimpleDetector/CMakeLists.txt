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
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#-----------------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package (SimpleDetector MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector] 
               [DD4hep REQUIRED COMPONENTS DDCore DDRec]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )

#-----------------------------------------------------------------------------------
dd4hep_add_plugin(SimpleDetector SOURCES src/*.cpp)
dd4hep_install_dir(compact DESTINATION ${DD4hep_DIR}/examples/ClientTests)
#-----------------------------------------------------------------------------------
dd4hep_configure_scripts(SimpleDetector DEFAULT_SETUP WITH_TESTS)

#***  Testing  *********************************************************************
foreach (test Simple_ILD)
  foreach( type description gdml vis )
    dd4hep_add_test_reg( SimpleDetector_converter_${type}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_SimpleDetector.sh"
      EXEC_ARGS  geoConverter -compact2${type} -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes")
  endforeach(type)
endforeach(test)

if (DD4HEP_USE_GEANT4)
  dd4hep_add_test_reg( SimpleDetector_sim_ILD
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_SimpleDetector.sh"
      EXEC_ARGS  dd_sim ${CMAKE_CURRENT_SOURCE_DIR}/compact/Simple_ILD.xml  ${CMAKE_CURRENT_SOURCE_DIR}/examples/sequences.xml
      REQUIRES   DDG4 Geant4
      REGEX_PASS "Event 9 Begin event action")
endif()
