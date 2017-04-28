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
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

if (DD4HEP_USE_XERCESC)
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

  #------------------------------------------------------------------------------
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
  dd4hep_add_test_reg( test_DDDB_extract_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/extract_dddb.sh
    REGEX_PASS "DDDB Database successfully installed." )
  #
  #---Testing: Load the geometry from archive -----------------------------------
  dd4hep_add_test_reg( test_DDDB_load_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    REGEX_PASS "Converted    12852 placements" )
  #
  #---Testing: Load the geometry + conditions from archive ----------------------
  dd4hep_add_test_reg( test_DDDB_conditions_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    REGEX_PASS "Converted     9353 conditions" )
  #
  #---Testing: Load the geometry + conditions dump as view from DetElement ------
  dd4hep_add_test_reg( test_DDDB_conditions_dump_simple_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -exec DDDB_ConditionsSummary
    REGEX_PASS "DDDB Total Number of parameters:    19057  " )
  #
  #---Testing: Load the geometry + dump detector elemets ------------------------
  dd4hep_add_test_reg( test_DDDB_det_elements_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller  -end-plugin
    -plugin   DDDB_DetectorDump -print DEBUG   -end-plugin
    REGEX_PASS "DDDB: Number of DetElements in the geometry:     3060" )
  #
  #---Testing: Load the geometry + dump volumes ---------------------------------
  dd4hep_add_test_reg( test_DDDB_det_volumes_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DetectorVolumeDump -print DEBUG
    REGEX_PASS "DDDB: Number of DetElement placements:           3056" )
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_keys_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DetectorConditionKeysDump -print DEBUG
    REGEX_PASS "DDDB: Number of DetElement condition keys:      14042")
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_data_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DetectorConditionDump -print DEBUG
    REGEX_PASS "DDDB: Number of attached conditions:             2261" )
  #
  #---Testing: Load the geometry + dump condition keys --------------------------
  dd4hep_add_test_reg( test_DDDB_det_conditions_align_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DetectorAlignmentDump -print DEBUG
    REGEX_PASS "Number of attached alignments:             2496")
  #
  #---Testing: Load the geometry + conditions dump as view from DetElement ------
  dd4hep_add_test_reg( test_DDDB_detelement_conditions_dump_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DetElementConditionDump -print DEBUG
    REGEX_PASS "Total Number of parameters:    18383" )
  #
  #---Testing: Load the geometry + conditions + conditions derives
  dd4hep_add_test_reg( test_DDDB_derived_conditions_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -plugin DDDB_DerivedCondTest -print DEBUG
    REGEX_PASS "Total Number of callbacks:    7479" )
  #
  #---Testing: Load the geometry + conditions + run basic derived alignments test
  dd4hep_add_test_reg( test_DDDB_derived_alignments_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -config DD4hep_AlignmentsManagerInstaller
    -plugin DDDB_DerivedAlignmentsTest -print DEBUG
    REGEX_PASS "AlignmentManager:   11848 conditions \\(S:0,L:9353,C:2495,M:0\\) \\(A:2493,M:0\\)" )
  #
  #---Testing: Load the geometry + conditions + access derived alignments from DetElement structures
  dd4hep_add_test_reg( test_DDDB_alignment_access_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/run_dddb.sh
    -config DD4hep_ConditionsManagerInstaller
    -config DD4hep_AlignmentsManagerInstaller
    -plugin DDDB_AlignmentsAccessTest -print DEBUG
    REGEX_PASS "Accessed 4990 conditions from the DetElement objects" )
  #
  #---Testing: Extract DDDB data from zip archive -------------------------------
  dd4hep_add_test_reg( test_DDDB_clean_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDDB.sh"
    EXEC_ARGS  ${CMAKE_INSTALL_PREFIX}/bin/extract_dddb.sh -clean
    REGEX_PASS "DDDB Database successfully removed" )

endif()
