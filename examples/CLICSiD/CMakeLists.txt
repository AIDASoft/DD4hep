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

set(CLICSiDEx_INSTALL         ${CMAKE_INSTALL_PREFIX}/examples/CLICSiD)
dd4hep_install_dir( scripts sim compact DESTINATION ${CLICSiDEx_INSTALL} )
#--------------------------------------------------------------------------
if (DD4HEP_USE_GEANT4)
  add_executable(CLICSiDXML scripts/CLICSiDXML.C)
  target_link_libraries(CLICSiDXML DD4hep::DDCore DD4hep::DDG4)
  
  add_executable(CLICSiDAClick  scripts/CLICSiDAClick.C)
  target_link_libraries(CLICSiDAClick DD4hep::DDCore DD4hep::DDG4)
  install(TARGETS CLICSiDXML CLICSiDAClick DESTINATION bin)
endif()
#
#
dd4hep_configure_scripts ( CLICSiD DEFAULT_SETUP WITH_TESTS )
#
enable_testing ()
include(CTest)


#
#---Testing-------------------------------------------------------------------------
#
#----- Tests for CLICSid: here we simply require that at least 100 volumes have been converted
##dd4hep_add_test_reg ( "CLICSiD_converter_gdml_LONGTEST" 
##  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
##  EXEC_ARGS  geoConverter -compact2gdml 
##                          -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
##                          -output file:CLICSiD.gdml
##  REGEX_PASS " Successfully extracted GDML to" )
foreach ( typ description vis )
  dd4hep_add_test_reg ( "CLICSiD_converter_${typ}_LONGTEST" 
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
    EXEC_ARGS  geoConverter -compact2${typ} 
                            -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
                            -output file:CLICSiD.${typ}
    REGEX_PASS " Handled [1-9][0-9][0-9]+ volumes" )
endforeach()
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( CLICSiD_check_geometry_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/bin/checkGeometry --compact=file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
  # This takes too long                  --full=true --ntracks=10 --option=o --vx=0 --vy=0 --vz=0
  REGEX_PASS " Execution finished..." )
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( CLICSiD_check_overlaps_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/bin/checkOverlaps --compact=file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
                    --tolerance=0.1
  REGEX_PASS " Execution finished..." )
#
#
# Load geometry from multiple input files
dd4hep_add_test_reg( CLICSiD_multiple_inputs
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  geoDisplay -input file:${CLICSiDEx_INSTALL}/compact/SiD_multiple_inputs.xml
             -input file:${CLICSiDEx_INSTALL}/compact/SiD_detectors_1.xml
             -input file:${CLICSiDEx_INSTALL}/compact/SiD_detectors_2.xml
             -input file:${CLICSiDEx_INSTALL}/compact/SiD_close.xml
             -print INFO -destroy -volmgr -load
  REGEX_PASS "VolumeManager    INFO   - populating volume ids - done. 29366 nodes."
  REGEX_FAIL "Exception;EXCEPTION;ERROR" )
#
#
## Always false. Good for now!
if( "${ROOT_FIND_VERSION}" VERSION_GREATER "6.13.0" )
# ROOT Geometry export to GDML
dd4hep_add_test_reg( CLICSiD_GDML_export_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  geoPluginRun -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -print WARNING -destroy -volmgr
             -plugin DD4hep_ROOTGDMLExtract -output EcalBarrel.gdml            -path /world/EcalBarrel
             -plugin DD4hep_ROOTGDMLExtract -output EcalEndcap.gdml            -path /world/EcalEndcap
             -plugin DD4hep_ROOTGDMLExtract -output HcalBarrel.gdml            -path /world/HcalBarrel 
             -plugin DD4hep_ROOTGDMLExtract -output HcalEndcap.gdml            -path /world/HcalEndcap
             -plugin DD4hep_ROOTGDMLExtract -output HcalPlug.gdml              -path /world/HcalPlug
             -plugin DD4hep_ROOTGDMLExtract -output MuonBarrel.gdml            -path /world/MuonBarrel
             -plugin DD4hep_ROOTGDMLExtract -output MuonEndcap.gdml            -path /world/MuonEndcap
             -plugin DD4hep_ROOTGDMLExtract -output LumiCal.gdml               -path /world/LumiCal
             -plugin DD4hep_ROOTGDMLExtract -output BeamCal.gdml               -path /world/BeamCal
             -plugin DD4hep_ROOTGDMLExtract -output LumiReadout_Forward.gdml   -path /world/LumiReadout_Forward
             -plugin DD4hep_ROOTGDMLExtract -output LumiReadout_Backward.gdml  -path /world/LumiReadout_Backward
             -plugin DD4hep_VolumeDump --topstat
  REGEX_PASS "Checked 130738 physical volume placements"
  REGEX_FAIL "Exception;EXCEPTION;ERROR" )
#
# ROOT Geometry export to GDML
dd4hep_add_test_reg( CLICSiD_GDML_import_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  geoPluginRun -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml -print WARNING -destroy -volmgr
             -plugin DD4hep_ROOTGDMLParse    -input EcalBarrel.gdml            -path /world/EcalBarrel
             -plugin DD4hep_ROOTGDMLParse    -input EcalEndcap.gdml            -path /world/EcalEndcap
             -plugin DD4hep_ROOTGDMLParse    -input HcalBarrel.gdml            -path /world/HcalBarrel
             -plugin DD4hep_ROOTGDMLParse    -input HcalEndcap.gdml            -path /world/HcalEndcap
             -plugin DD4hep_ROOTGDMLParse    -input HcalPlug.gdml              -path /world/HcalPlug
             -plugin DD4hep_ROOTGDMLParse    -input MuonBarrel.gdml            -path /world/MuonBarrel
             -plugin DD4hep_ROOTGDMLParse    -input MuonEndcap.gdml            -path /world/MuonEndcap
             -plugin DD4hep_ROOTGDMLParse    -input LumiCal.gdml               -path /world/LumiCal
             -plugin DD4hep_ROOTGDMLParse    -input BeamCal.gdml               -path /world/BeamCal
             -plugin DD4hep_ROOTGDMLParse    -input LumiReadout_Forward.gdml   -path /world/LumiReadout_Forward
             -plugin DD4hep_ROOTGDMLParse    -input LumiReadout_Backward.gdml  -path /world/LumiReadout_Backward
             -plugin DD4hep_VolumeDump --topstat
  DEPENDS    CLICSiD_GDML_export_LONGTEST
  REGEX_PASS "Checked 130738 physical volume placements"
  REGEX_FAIL "Exception;EXCEPTION;ERROR" )
#
endif()
#
#---Geant4 Testing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  # Basic DDG4 component/unit tests
  foreach(script testDDPython CLICMagField CLICPhysics CLICRandom CLICSiDScan)
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  python ${CLICSiDEx_INSTALL}/scripts/${script}.py
      REGEX_PASS "TEST_PASSED"
      REGEX_FAIL "Exception;EXCEPTION;ERROR" )
  endforeach(script)
  #
  # Write GDML from Geant4 using UI
  #dd4hep_add_test_reg( CLICSiD_DDG4_GDML_LONGTEST
  #    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  #    EXEC_ARGS  python ${CLICSiDEx_INSTALL}/scripts/CLIC_GDML.py
  #    REGEX_PASS "G4GDML. Writing 'CLICSiD.gdml' done !"
  #    REGEX_FAIL "Exception;EXCEPTION;ERROR" )
  #
  # Material scan
  dd4hep_add_test_reg( CLICSiD_DDG4_g4material_scan_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/bin/g4MaterialScan --compact=$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
                      "--position=0,0,0" "--direction=0,1,0"
    REGEX_PASS " Terminate Geant4 and delete associated actions." )
  #
  # Geometry scan
  dd4hep_add_test_reg( CLICSiD_DDG4_g4geometry_scan_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/bin/g4GeometryScan --compact=$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
                      "--position=0,0,0" "--direction=0,1,0"
    REGEX_PASS "|   856     2374.8789   3000.000     (   0.00,3000.00,   0.00)  Path:\"/world\" Shape:G4Box  Mat:Air" )
  #
  # Geant4 simulations with initialization using AClick and XMl
  foreach(script CLICSiDXML CLICSiDAClick)
    #
    # Build AClick from the source file
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_as_AClick_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  root.exe -b -x -n -q -l "${DD4hep_DIR}/examples/DDG4/examples/run.C(\"${CLICSiDEx_INSTALL}/scripts/${script}\")"
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "EXCEPTION;ERROR;Error" )
    #
    # Execute identical source linked executable 
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_as_exe_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  ${script}
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "EXCEPTION;ERROR;Error" )
  endforeach(script)
  #
  # Invoke G4 multi-threading tests if enabled
  if (Geant4_TLS_MODEL)
    dd4hep_print("|++> Geant4 Multi-threading ENABLED. Configuring multi-threaded DDG4 tests.")
  else()
    dd4hep_print("|++> Geant4 Multi-threading NOT ENABLED. Shall not execute multi-threaded DDG4 tests.")
  endif()
endif()
