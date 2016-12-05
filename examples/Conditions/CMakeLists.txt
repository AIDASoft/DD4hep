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
dd4hep_configure_output ()
dd4hep_package ( Conditions MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector]
               [DD4hep REQUIRED COMPONENTS DDCore DDCond DDAlign]
  OPTIONAL     XERCESC
)
#-----------------------------------------------------------------------------------
dd4hep_add_plugin(ConditionsExample SOURCES src/*.cpp)
dd4hep_install_dir( xml  DESTINATION ${DD4hep_DIR}/examples/Conditions )
dd4hep_install_dir( data DESTINATION ${DD4hep_DIR}/examples/Conditions )
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( test_Conditions_Telescope_cond_dump_by_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy
  -compact file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${DD4hep_DIR}/examples/Conditions/data/repository.xml
  -plugin DD4hep_ConditionsDump
  REGEX_PASS "Data:\\[\\(0,0,1\\), \\{phi\\(Z angle\\)\\: 3.14159   theta\\(Y angle\\)\\: 0   psi\\(X angle\\)\\: 0\\} , 0  0  0 \\]")
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( test_Conditions_Telescope_cond_dump_by_detelement
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy
  -compact file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${DD4hep_DIR}/examples/Conditions/data/repository.xml
  -plugin DD4hep_DetElementConditionsDump
  REGEX_PASS "Path\\:/world/Telescope/module_9#alignment")
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_Conditions_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_populate
      -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 5
  REGEX_PASS "Total 160 conditions \\(S:100,L:0,C:60,M:0\\) of IOV run\\(0\\):\\[45-45\\]")
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_Conditions_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 10 -runs 20
  REGEX_PASS "Summary: # of IOV:  10  # of Runs:  20")
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_Conditions_Telescope_stress2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress2
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 10
  REGEX_PASS "Summary: # of IOV:  10")
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_Conditions_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 10 -runs 100
  REGEX_PASS "Summary: # of IOV:  10  # of Runs: 100")
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( test_Conditions_CLICSiD_stress2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress2
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 20
  REGEX_PASS "Summary: # of IOV:  20")
