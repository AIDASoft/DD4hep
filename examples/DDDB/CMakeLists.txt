# $Id$
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
#
# DDDB is a detector description convention developed by the LHCb experiment.
# For further information concerning the DTD, please see:
# http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
#
# The usage of Xerces-C is mandatory due to the entity resolution mechanism
#

#==========================================================================
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

if (DD4HEP_USE_XERCESC)
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

  #-----------------------------------------------------------------------------------
  dd4hep_configure_output ()
  dd4hep_package (DDDB MAJOR 0 MINOR 0 PATCH 1
    USES  [XERCESC REQUIRED]
    [ROOT    REQUIRED COMPONENTS Geom] 
    [DD4hep  REQUIRED COMPONENTS DDCore DDDB]
    )
  #---Package installation procedure(s) -----------------------------------------
  install ( PROGRAMS scripts/run_dddb.sh DESTINATION bin)
  install ( PROGRAMS scripts/extract_dddb.sh DESTINATION bin)
  install ( FILES    data/DDDB.tar.gz DESTINATION examples/DDDB)
  #---Testing--------------------------------------------------------------------
  dd4hep_configure_scripts ( DDDB DEFAULT_SETUP WITH_TESTS )
  #
  #---Testing: Extract DDDB data from zip archive -------------------------------
  dd4hep_add_test_reg( test_DDDB_extract
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/extract_dddb.sh
    REGEX_PASS "DDDB Database successfully installed." )
  #
  #---Testing: Load the geometry from archive -----------------------------------
  dd4hep_add_test_reg( test_DDDB_load
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    REGEX_PASS "Converted    12768 placements" )
  #
  #---Testing: Load the geometry + conditions from archive ----------------------
  dd4hep_add_test_reg( test_DDDB_conditions
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    REGEX_PASS "Converted     9353 conditions" )
  #
  #---Testing: Load the geometry + conditions dump as view from DetElement ------
  dd4hep_add_test_reg( test_DDDB_conditions_dump_simple
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_ConditionsDump
    REGEX_PASS "Path:/dd/Conditions/ReadoutConf/Velo/Tell1Module03 Class:5 .DD4hep::Conditions::AbstractMap." )
  #
  #---Testing: Load the geometry + dump detector elemets ------------------------
  dd4hep_add_test_reg( test_DDDB_det_elements
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetectorDump
    REGEX_PASS "Detector: /world/LHCb/UpstreamRegion #Dau:5" )
  #
  #---Testing: Load the geometry + dump volumes ---------------------------------
  dd4hep_add_test_reg( test_DDDB_det_volumes
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetectorVolumeDump
    REGEX_PASS "Detector: /world/LHCb/MagnetRegion/BcmDown #Dau:10" )
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_keys
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetectorConditionKeysDump
    REGEX_PASS "Key: D88E83E7 -> 20537B67 -> /dd/Conditions/ReadoutConf/Prs/Readout")
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_data
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetectorConditionDump
    REGEX_PASS "Path:/dd/Conditions/Alignment/Spd/SpdCSystem Class:6 .DD4hep::Conditions::AbstractMap." )
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_align
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetectorAlignmentDump
    REGEX_PASS "Number of attached alignments:             2496")
  #
  #---Testing: Load the geometry + conditions dump as view from DetElement ------
  dd4hep_add_test_reg( test_DDDB_detelement_conditions_dump
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DetElementConditionDump
    REGEX_PASS "Path:/dd/Conditions/Alignment/Spd/SpdCSystem Class:6 .DD4hep::Conditions::AbstractMap." )

  #
  #---Testing: Load the geometry + conditions + conditions derives
  dd4hep_add_test_reg( test_DDDB_derived_conditions
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec   DDDB_DerivedCondTest
    REGEX_PASS "Building dependent condition: /dd/Conditions/Alignment/TT/TTbVLayerR1Module3B/derived_3" )
  #
  #---Testing: Extract DDDB data from zip archive -------------------------------
  dd4hep_add_test_reg( test_DDDB_clean
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/extract_dddb.sh -clean
    REGEX_PASS "DDDB Database successfully removed" )


endif()
