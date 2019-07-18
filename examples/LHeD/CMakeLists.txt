#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
# CLICSiD used as template!
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#-----------------------------------------------------------------------------------
dd4hep_configure_output ()
# dd4hep_package ( LHeD MAJOR 0 MINOR 0 PATCH 1
#   USES           [ROOT   REQUIRED COMPONENTS Geom GenVector MathCore] 
#                  [DD4hep REQUIRED COMPONENTS DDRec DDCore DDCond DDAlign]
#   )
set(LHeDEx_INSTALL         ${CMAKE_INSTALL_PREFIX}/examples/LHeD)
dd4hep_install_dir( src compact scripts sim DESTINATION ${LHeDEx_INSTALL} )
#-----------------------------------------------------------------------------------
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

dd4hep_add_plugin(LHeD
  SOURCES src/*.cpp
  USES  ROOT::Core ROOT::Geom ROOT::GenVector DD4hep::DDRec DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
  )

if (DD4HEP_USE_GEANT4)
  find_package(Geant4 REQUIRED)
  add_executable(LHeDXML scripts/LHeDXML.C)
  target_link_libraries(LHeDXML DD4hep::DDCore DD4hep::DDG4 Geant4::Interface)
  #--------------------------------------------------------------------------
  add_executable(LHeDACLick scripts/LHeDACLick.C)
  target_link_libraries(LHeDACLick DD4hep::DDCore DD4hep::DDG4 Geant4::Interface)
  INSTALL(TARGETS LHeDXML LHeDACLick  LHeD
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
endif()

#
dd4hep_configure_scripts(LHeD DEFAULT_SETUP WITH_TESTS)
#
#---Testing-------------------------------------------------------------------------
#
#----- Tests for LHeD: here we simply require that at least 100 volumes have been converted
##dd4hep_add_test_reg ( "LHeD_converter_gdml_LONGTEST" 
##  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
##  EXEC_ARGS  geoConverter -compact2gdml 
##                          -input file:${LHeDEx_INSTALL}/compact/compact.xml
##                          -output file:LHeD.gdml
##  REGEX_PASS " Successfully extracted GDML to" )
foreach ( typ description vis )
  dd4hep_add_test_reg ( "LHeD_converter_${typ}_LONGTEST" 
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
    EXEC_ARGS  geoConverter -compact2${typ} 
                            -input file:${LHeDEx_INSTALL}/compact/compact.xml
                            -output file:LHeD.${typ}
    REGEX_PASS " Handled [1-9][0-9][0-9]+ volumes" )
endforeach()
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( LHeD_check_geometry_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/python/checkGeometry.py
                    --compact=file:${LHeDEx_INSTALL}/compact/compact.xml
  # This takes too long                  --full=true --ntracks=10 --option=o --vx=0 --vy=0 --vz=0
  REGEX_PASS " Execution finished..." )
#
# ROOT Geometry overlap checks
dd4hep_add_test_reg( LHeD_check_overlaps_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
  EXEC_ARGS  python ${DD4hep_DIR}/python/checkOverlaps.py 
                    --compact=file:${LHeDEx_INSTALL}/compact/compact.xml
                    --tolerance=0.1
  REGEX_PASS " Execution finished..." )
#
#---Geant4 Testsing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #
  # Basic DDG4 component/unit tests
  foreach(script LHeDMagField LHeDPhysics LHeDRandom LHeDScan)
    dd4hep_add_test_reg( LHeD_DDG4_${script}_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
      EXEC_ARGS  python ${LHeDEx_INSTALL}/scripts/${script}.py
      REGEX_PASS "TEST_PASSED"
      REGEX_FAIL "Exception;EXCEPTION;ERROR" )
  endforeach(script)
  #
  # Material scan
  dd4hep_add_test_reg( LHeD_DDG4_g4material_scan_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
    EXEC_ARGS  python ${DD4hep_DIR}/python/g4MaterialScan.py
                      --compact=file:${LHeDEx_INSTALL}/compact/compact.xml 
                      "--position=0,0,0" "--direction=0,1,0"
    REGEX_PASS " Terminate Geant4 and delete associated actions." )
  #
  # Geant4 simulations with initialization using ACLick and XMl
  foreach(script LHeDXML LHeDACLick)
    #
    # Build ACLick from the source file
    dd4hep_add_test_reg( LHeD_DDG4_${script}_as_ACLick_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
      EXEC_ARGS  root.exe -b -x -n -q -l "${LHeDEx_INSTALL}/scripts/run.C(\"${LHeDEx_INSTALL}/scripts/${script}\")"
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
    #
    # Execute identical source linked executable 
    dd4hep_add_test_reg( LHeD_DDG4_${script}_as_exe_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_LHeD.sh"
      EXEC_ARGS  ${script}
      REGEX_PASS "UserEvent_1      INFO  Geant4TestEventAction> calling end.event_id=9"
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
endif()
