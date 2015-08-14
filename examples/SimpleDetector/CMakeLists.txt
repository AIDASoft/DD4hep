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
dd4hep_configure_output ()
dd4hep_package ( SimpleDetector MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
)
dd4hep_install_dir( compact DESTINATION examples/SimpleDetector )
#---Testing-------------------------------------------------------------------------
dd4hep_configure_scripts ( SimpleDetector DEFAULT_SETUP WITH_TESTS )

#----- Tests for CLICSid: here we simply require that at least 100 volumes have been converted
foreach ( typ lcdd gdml vis )
  dd4hep_add_test_reg ( "test_converter_SimpleDetector_${typ}" 
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_SimpleDetector.sh"
    EXEC_ARGS  geoConverter -compact2${typ} -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml -output file:SimpleDetector.${typ}
    REGEX_PASS " Handled [1-9][0-9][0-9]+ volumes" )
endforeach()
