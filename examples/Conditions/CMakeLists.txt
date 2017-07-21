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

#--------------------------------------------------------------------------
dd4hep_configure_output ()
dd4hep_package ( Conditions MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector]
               [DD4hep REQUIRED COMPONENTS DDCore DDCond DDAlign]
  OPTIONAL     XERCESC
)
#-----------------------------------------------------------------------------------
dd4hep_add_plugin(ConditionsExample SOURCES src/*.cpp)
dd4hep_install_dir( xml  DESTINATION examples/Conditions )
dd4hep_install_dir( data DESTINATION examples/Conditions )
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_Telescope_cond_dump_by_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml 
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${DD4hep_DIR}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_ConditionsDump
  REGEX_PASS "Data\\(Translation-Rotation-Pivot\\): \\[\\( 0 , 0 , 1 \\), \\{phi\\(Z angle\\): 3.14159   theta\\(Y angle\\): 0   psi\\(X angle\\): 0\\}, \\( 0 , 0 , 0 \\)\\]"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_Telescope_cond_dump_by_detelement
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml 
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${DD4hep_DIR}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_DetElementConditionsDump
  REGEX_PASS "Path\\:/world/Telescope/module_9#alignment"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_populate
      -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 5
  REGEX_PASS "Accessed a total of 800 conditions \\(S:   500,L:     0,C:   300,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 10 -runs 20
  REGEX_PASS "\\+  Accessed a total of 3200 conditions \\(S:  2660,L:     0,C:   540,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 10
  REGEX_PASS "\\+  Accessed a total of 1600 conditions \\(S:  1000,L:     0,C:   600,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 30 -runs 10 -threads 1
  REGEX_PASS "\\+  Accessed a total of 286400 conditions \\(S:268400,L:     0,C: 18000,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_save
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -volmgr -destroy -plugin DD4hep_ConditionExample_save
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml -iovs 30
    -conditions TelescopeConditions.root
  REGEX_PASS "\\+ Successfully saved 14400 condition to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_load
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -volmgr -destroy -plugin DD4hep_ConditionExample_load
    -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml
    -conditions TelescopeConditions.root
  REGEX_PASS "\\+  Accessed a total of 1600 conditions \\(S:  1600,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 10 -runs 100
  REGEX_PASS "\\+  Accessed a total of 28008800 conditions \\(S:26958470,L:     0,C:1050330,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 20
  REGEX_PASS "\\+  Accessed a total of 5601760 conditions \\(S:3501100,L:     0,C:2100660,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -volmgr -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 3 -runs 2 -threads 1
  REGEX_PASS "\\+  Accessed a total of 9522992 conditions \\(S:8892794,L:     0,C:630198,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -volmgr -destroy -plugin DD4hep_ConditionExample_save
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 3
    -conditions CLICSiDConditions.root
  REGEX_PASS "\\+ Successfully saved 2520792 condition to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -volmgr -destroy -plugin DD4hep_ConditionExample_load
    -input file:${DD4hep_DIR}/examples/CLICSiD/compact/compact.xml -iovs 3
    -conditions CLICSiDConditions.root
  REGEX_PASS "\\+  Accessed a total of 840264 conditions \\(S:840264,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
