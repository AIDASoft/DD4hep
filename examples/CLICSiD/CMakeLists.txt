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
dd4hep_configure_output ()
dd4hep_package ( CLICSiD MAJOR 0 MINOR 0 PATCH 1
  USES  [ROOT   REQUIRED COMPONENTS Geom] 
        [DD4hep REQUIRED COMPONENTS DDCore]
)
dd4hep_install_dir( compact scripts sim DESTINATION ${DD4hep_DIR}/examples/CLICSiD )
#--------------------------------------------------------------------------
if (DD4HEP_USE_GEANT4)
  find_package(Geant4 REQUIRED)
  dd4hep_add_executable(CLICSiDXML SOURCES scripts/CLICSiDXML.C
    USES [DD4hep REQUIRED COMPONENTS DDCore DDG4] Geant4)
  #--------------------------------------------------------------------------
  dd4hep_add_executable(CLICSiDAClick SOURCES scripts/CLICSiDAClick.C
    USES [DD4hep REQUIRED COMPONENTS DDCore DDG4] Geant4)
endif()
#
dd4hep_configure_scripts ( CLICSiD DEFAULT_SETUP WITH_TESTS )
#
#---Testing-------------------------------------------------------------------------
#
#----- Tests for CLICSid: here we simply require that at least 100 volumes have been converted
foreach ( typ description gdml vis )
  dd4hep_add_test_reg ( "CLICSiD_converter_${typ}_LONGTEST" 
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
    EXEC_ARGS  geoConverter -compact2${typ} 
                            -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml
                            -output file:CLICSiD.${typ}
    REGEX_PASS " Handled [1-9][0-9][0-9]+ volumes" )
endforeach()
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( CLICSiD_check_geometry_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/python/checkGeometry.py
                    --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml
  # This takes too long                  --full=true --ntracks=10 --option=o --vx=0 --vy=0 --vz=0
  REGEX_PASS " Execution finished..." )
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( CLICSiD_check_overlaps_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/python/checkOverlaps.py 
                    --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml
                    --tolerance=0.1
  REGEX_PASS " Execution finished..." )
#
###if (DD4HEP_HAVE_HIGHER_ROOT_VERSION)
# ROOT Geometry export to GDML
dd4hep_add_test_reg( CLICSiD_GDML_export_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
  EXEC_ARGS  geoPluginRun -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml -print WARNING -destroy -volmgr
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
  EXEC_ARGS  geoPluginRun -input file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact_nocalo.xml -print WARNING -destroy -volmgr
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
####endif(DD4HEP_HAVE_HIGHER_ROOT_VERSION)
#
#---Geant4 Testsing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  # Basic DDG4 component/unit tests
  foreach(script testDDPython CLICMagField CLICPhysics CLICRandom CLICSiDScan)
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${script}.py
      REQUIRES   DDG4 Geant4
      REGEX_PASS "TEST_PASSED"
      REGEX_FAIL "Exception;EXCEPTION;ERROR" )
  endforeach(script)
  #
  # Material scan
  dd4hep_add_test_reg( CLICSiD_DDG4_g4material_scan_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/python/g4MaterialScan.py
                      --compact=file:${CMAKE_CURRENT_SOURCE_DIR}/compact/compact.xml 
                      "--position=0,0,0" "--direction=0,1,0"
    REQUIRES   DDG4 Geant4
    REGEX_PASS " Terminate Geant4 and delete associated actions." )
  #
  # Geant4 simulations with initialization using AClick and XMl
  foreach(script CLICSiDXML CLICSiDAClick)
    #
    # Build AClick from the source file
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_as_AClick_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  root.exe -b -x -n -q -l "${DD4hep_DIR}/examples/DDG4/examples/run.C(\"${DD4hep_DIR}/examples/CLICSiD/scripts/${script}\")"
      REQUIRES   DDG4 Geant4
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
    #
    # Execute identical source linked executable 
    dd4hep_add_test_reg( CLICSiD_DDG4_${script}_as_exe_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_CLICSiD.sh"
      EXEC_ARGS  ${script}
      REQUIRES   DDG4 Geant4
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
endif()
