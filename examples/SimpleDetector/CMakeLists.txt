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
if(TARGET XercesC::XercesC)
  link_libraries(XercesC::XercesC)
endif()

link_libraries(ROOT::Core ROOT::Geom ROOT::GenVector DD4hep::DDCore DD4hep::DDRec)

new_dd4hep_add_plugin(SimpleDetector SOURCES src/*.cpp)
install(TARGETS SimpleDetector LIBRARY DESTINATION lib)
install(DIRECTORY compact DESTINATION examples/ClientTests)
#-----------------------------------------------------------------------------------
dd4hep_configure_scripts(SimpleDetector DEFAULT_SETUP WITH_TESTS)

#***  Testing  *********************************************************************
foreach (test Simple_ILD)
##  dd4hep_add_test_reg( SimpleDetector_converter_gdml
##    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_SimpleDetector.sh"
##    EXEC_ARGS  geoConverter -compact2gdml -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml -output file:${test}.gdml
##    REGEX_PASS " Successfully extracted GDML to")
  foreach( type description vis )
    dd4hep_add_test_reg( SimpleDetector_converter_${type}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_SimpleDetector.sh"
      EXEC_ARGS  geoConverter -compact2${type} -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/${test}.xml -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes")
  endforeach(type)
endforeach(test)
