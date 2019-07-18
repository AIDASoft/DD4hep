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

if(TARGET XercesC::XercesC)
  link_libraries(XercesC::XercesC)
endif()

link_libraries(ROOT::Core ROOT::Geom ROOT::GenVector
  DD4hep::DDCore
  DD4hep::DDCond
  DD4hep::DDAlign
  )
  
# dd4hep_package ( Conditions MAJOR 0 MINOR 0 PATCH 1
#   USES         [ROOT   REQUIRED COMPONENTS Geom GenVector]
#                [DD4hep REQUIRED COMPONENTS DDCore DDCond DDAlign]
#   OPTIONAL     XERCESC
# )
#-----------------------------------------------------------------------------------
dd4hep_add_plugin(ConditionsExample SOURCES src/*.cpp)
install(TARGETS ConditionsExample LIBRARY DESTINATION lib)
dd4hep_install_dir( xml  DESTINATION examples/Conditions )
dd4hep_install_dir( data DESTINATION examples/Conditions )
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_Telescope_cond_dump_by_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml 
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_ConditionsDump
  REGEX_PASS "Data\\(Translation-Rotation-Pivot\\): \\[\\( 0 , 0 , 1 \\), \\{phi\\(Z angle\\): 3.14159   theta\\(Y angle\\): 0   psi\\(X angle\\): 0\\}, \\( 0 , 0 , 0 \\)\\]"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_Telescope_cond_dump_by_detelement
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml 
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_DetElementConditionsDump
  REGEX_PASS "Path\\:/world/Telescope/module_9#alignment"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_populate
      -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 5
  REGEX_PASS "Accessed a total of 800 conditions \\(S:   500,L:     0,C:   300,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 10 -runs 20
  REGEX_PASS "\\+  Accessed a total of 3200 conditions \\(S:  2660,L:     0,C:   540,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 10
  REGEX_PASS "\\+  Accessed a total of 1600 conditions \\(S:  1000,L:     0,C:   600,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 30 -runs 10 -threads 1
  REGEX_PASS "\\+  Accessed a total of 286400 conditions \\(S:268400,L:     0,C: 18000,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_save
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_save
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 30
    -conditions TelescopeConditions.root
  REGEX_PASS "\\+ Successfully saved 14400 condition to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_load_iov
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml
    -conditions TelescopeConditions.root -iovs 30 -restore iovpool
  DEPENDS Conditions_Telescope_root_save
  REGEX_PASS "\\+  Accessed a total of 4800 conditions \\(S:  4800,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_load_usr
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml
    -conditions TelescopeConditions.root -iovs 30 -restore userpool
  DEPENDS Conditions_Telescope_root_save
  REGEX_PASS "\\+  Accessed a total of 4800 conditions \\(S:  4800,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_load_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml
    -conditions TelescopeConditions.root -iovs 30 -restore condpool
  DEPENDS Conditions_Telescope_root_save
  REGEX_PASS "\\+  Accessed a total of 4800 conditions \\(S:  4800,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 10 -runs 100
  REGEX_PASS "\\+  Accessed a total of 28085600 conditions \\(S:27032390,L:     0,C:1053210,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 20
  REGEX_PASS "\\+  Accessed a total of 5617120 conditions \\(S:3510700,L:     0,C:2106420,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 3 -runs 2 -threads 1
  REGEX_PASS "\\+  Accessed a total of 9549104 conditions \\(S:8917178,L:     0,C:631926,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_save
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 3
    -conditions CLICSiDConditions.root
  REGEX_PASS "\\+ Successfully saved 2527704 condition to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_iov_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 3 -restore iovpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 842568 conditions \\(S:842568,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_usr_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 3 -restore userpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 842568 conditions \\(S:842568,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_cond_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -iovs 3 -restore condpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 842568 conditions \\(S:842568,L:     0,C:     0,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
