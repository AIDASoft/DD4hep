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
dd4hep_configure_output ()
dd4hep_package ( CLICSiD MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
)
dd4hep_install_dir( xml DESTINATION ${DD4hep_DIR}/examples/Conditions )
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( test_Conditions_Telescope_cond_dump_by_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy
  -compact file:${CMAKE_CURRENT_SOURCE_DIR}/../AlignDet/compact/Telescope.xml
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${CMAKE_CURRENT_SOURCE_DIR}/data/repository.xml
  -plugin DD4hep_ConditionsDump
  REGEX_PASS "Data:\\[\\(0,0,29\\), \\{phi\\(Z angle\\)\\: 0   theta\\(Y angle\\)\\: 1.5708   psi\\(X angle\\)\\: 0\\} , 0  0  0 \\]")
#
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( test_Conditions_Telescope_cond_dump_by_detelement
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy
  -compact file:${CMAKE_CURRENT_SOURCE_DIR}/../AlignDet/compact/Telescope.xml
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${CMAKE_CURRENT_SOURCE_DIR}/data/repository.xml
  -plugin DD4hep_DetElementConditionsDump
  REGEX_PASS "Path\\:/world/Telescope/module_9#alignment")
