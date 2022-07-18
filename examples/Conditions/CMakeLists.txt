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
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================

if(TARGET XercesC::XercesC)
  SET(OPT_XERCESC XercesC::XercesC)
endif()
  
#-----------------------------------------------------------------------------------
dd4hep_add_plugin(ConditionsExample SOURCES src/*.cpp
  USES  DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
        ROOT::Core ROOT::Geom ROOT::GenVector
        ${OPT_XERCESC}
  )
install(TARGETS ConditionsExample LIBRARY DESTINATION lib)
dd4hep_install_dir( xml  DESTINATION examples/Conditions )
dd4hep_install_dir( data DESTINATION examples/Conditions )
dd4hep_configure_scripts ( Conditions DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_dynamic_payload
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_Conditions_dynamic
  REGEX_PASS "Test PASSED"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load Telescope geometry and read conditions ------------------
dd4hep_add_test_reg( Conditions_Telescope_cond_dump_by_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml 
  -plugin DD4hep_ConditionsXMLRepositoryParser file:${CMAKE_INSTALL_PREFIX}/examples/Conditions/data/repository.xml 
  -plugin DD4hep_ConditionsDump
  REGEX_PASS "Data\\(Translation-Rotation\\(Phi,Theta,Psi\\)-Pivot\\): \\[\\( 0 , 0 , 1 \\) \\[cm\\], \\( 3.14159265359 , 0 , 0 \\) \\[rad\\], \\( 0 , 0 , 0 \\) \\[cm\\]\\]"
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
  REGEX_PASS "Key\\:FA708B8A6E87AB8E Type\\:dd4hep\\:\\:Delta"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_populate
      -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 5
  REGEX_PASS "Accessed a total of 1000 conditions \\(S:   600,L:     0,C:   400,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 10 -runs 20
  REGEX_PASS "\\+  Accessed a total of 4000 conditions \\(S:  3280,L:     0,C:   720,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_stress2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 10
  REGEX_PASS "\\+  Accessed a total of 2000 conditions \\(S:  1200,L:     0,C:   800,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_Telescope_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 30 -runs 10 -threads 1
  REGEX_PASS "\\+  Accessed a total of 358000 conditions \\(S:334000,L:     0,C: 24000,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_save
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_save
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 30
    -conditions TelescopeConditions.root
  REGEX_PASS "\\+ Successfully saved 16200 condition to file."
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
  REGEX_PASS "\\+  Accessed a total of 6000 conditions \\(S:  5400,L:     0,C:   600,M:0\\)"
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
  REGEX_PASS "\\+  Accessed a total of 6000 conditions \\(S:  5400,L:     0,C:   600,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load conditions from ROOT file
dd4hep_add_test_reg( Conditions_Telescope_root_load_pool
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml
    -conditions TelescopeConditions.root -iovs 30 -restore condpool
  DEPENDS Conditions_Telescope_root_save
  REGEX_PASS "\\+  Accessed a total of 6000 conditions \\(S:  5400,L:     0,C:   600,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Attempt to build unresolved conditions object
dd4hep_add_test_reg( Conditions_Telescope_unresolved
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_populate
      -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 1 -extend 100
  REGEX_PASS "Condition: /world/Telescope/module_6#Unresolved_dependency dependent on missing condition"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress 
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 10 -runs 100
  REGEX_PASS "\\+  Accessed a total of 35107000 conditions \\(S:33702720,L:     0,C:1404280,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Simple stress: Load CLICSiD geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_stress2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_stress2 
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 20
  REGEX_PASS "\\+  Accessed a total of 7021400 conditions \\(S:4212840,L:     0,C:2808560,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Multi-threading test: Load CLICSiD geometry and have multiple parallel runs on IOVs
dd4hep_add_test_reg( Conditions_CLICSiD_MT_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -plugin DD4hep_ConditionExample_MT 
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 3 -runs 2 -threads 1
  REGEX_PASS "\\+  Accessed a total of 11936380 conditions \\(S:11093812,L:     0,C:842568,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_save
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 3
    -conditions CLICSiDConditions.root
  REGEX_PASS "\\+ Successfully saved 2843667 condition to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_iov_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 3 -restore iovpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 1053210 conditions \\(S:947889,L:     0,C:105321,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_usr_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 3 -restore userpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 1053210 conditions \\(S:947889,L:     0,C:105321,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Save conditions to ROOT file
dd4hep_add_test_reg( Conditions_CLICSiD_root_load_cond_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print WARNING -destroy -plugin DD4hep_ConditionExample_load
    -input file:${DD4hep_ROOT}/DDDetectors/compact/SiD.xml -iovs 3 -restore condpool
    -conditions CLICSiDConditions.root
  DEPENDS Conditions_CLICSiD_root_save_LONGTEST
  REGEX_PASS "\\+  Accessed a total of 1053210 conditions \\(S:947889,L:     0,C:105321,M:0\\)"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Basic test using std::any as conditions payload
dd4hep_add_test_reg( Conditions_any_basic
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun -print INFO -destroy -plugin DD4hep_Conditions_any_basic
  REGEX_PASS "Test PASSED"
  REGEX_FAIL "Test FAILED"
  )
#
#---Testing: Simple stress: Load Telescope geometry and have multiple runs on IOVs
dd4hep_add_test_reg( Conditions_any_Telescope_populate
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Conditions.sh"
  EXEC_ARGS  geoPluginRun  -destroy -print WARNING -plugin DD4hep_ConditionAnyExample_populate
      -input file:${CMAKE_INSTALL_PREFIX}/examples/AlignDet/compact/Telescope.xml -iovs 2 -extend 1
  REGEX_PASS "\\+ Analyzed 1360 any object/elements"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
