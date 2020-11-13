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

include_directories(./include)
#--------------------------------------------------------------------------
if(TARGET Boost::boost)
  SET(CT_BOOST_SOURCES src_boost/*.cpp)
  set(CT_BOOST_LIB Boost::boost)
endif()
#
dd4hep_add_plugin(ClientTests
  SOURCES src/*.cpp ${CT_BOOST_SOURCES}
  USES DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
       ROOT::Core ROOT::Geom ROOT::GenVector
       ${OPT_XERCESC} ${CT_BOOST_LIB}
  )
install(TARGETS ClientTests LIBRARY DESTINATION lib)
#
#-----------------------------------------------------------------------------------
add_executable( multipleGeo   main/MultipleGeometries.cpp )
target_link_libraries(multipleGeo DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign)
install(TARGETS multipleGeo RUNTIME DESTINATION bin)
#-----------------------------------------------------------------------------------
#
#
set(ClientTestsEx_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/ClientTests)
dd4hep_install_dir( compact scripts ref eve DESTINATION ${ClientTestsEx_INSTALL} )
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
dd4hep_configure_scripts( ClientTests DEFAULT_SETUP WITH_TESTS)
enable_testing ()
include(CTest)
#---  Testing  ------------------------------------------------------------
#
#  Test Volume scanner for CMS
dd4hep_add_test_reg( ClientTests_volume_scanner
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -input ${ClientTestsEx_INSTALL}/compact/MiniTel.xml
  -destroy -plugin DD4hep_PlacedVolumeScannerTest -detector /world
  REGEX_PASS "Visited a total of 31 placed volumes"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test namespaces for constants 
dd4hep_add_test_reg( ClientTests_namespace_constants
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -input ${ClientTestsEx_INSTALL}/compact/NamespaceConstants.xml
  -destroy -plugin DD4hep_VolumeDump -plugin DD4hep_TestConstantsMultiplier
  REGEX_PASS "Constant: world_z          = world::Z         \\[number\\]  -> world_z          =       100"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test JSON based parser
dd4hep_add_test_reg( ClientTests_MiniTel_JSON_Dump
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -destroy -plugin DD4hep_JsonDumper
  ${ClientTestsEx_INSTALL}/compact/MiniTel.json
  REGEX_PASS "Successfully dumped json input"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test JSON based detector construction
dd4hep_add_test_reg( ClientTests_MiniTel_JSON_Detector
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -plugin DD4hep_XMLLoader
  file:${ClientTestsEx_INSTALL}/compact/MiniTel_json.xml
  REGEX_PASS "Successfully processed JSON input"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test JSON based detector construction
dd4hep_add_test_reg( ClientTests_DumpMaterials
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/MiniTel.xml
  -volmgr -destroy -plugin DD4hep_MaterialTable -type xml
  REGEX_PASS "material name=\"PEEK\""
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test readout strings of the form: <id>system:8,barrel:-2</id>
dd4hep_add_test_reg( ClientTests_MultipleGeometries
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS   multipleGeo
  -compact file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
  -compact file:${ClientTestsEx_INSTALL}/compact/MiniTel.xml
  -compact file:${ClientTestsEx_INSTALL}/compact/NestedDetectors.xml
  -no-interp
  REGEX_PASS "DELETE Geometry: .*/NestedDetectors.xml"
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test readout strings of the form: <id>system:8,barrel:-2</id>
dd4hep_add_test_reg( ClientTests_DumpElements
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/MiniTel.xml
  -volmgr -destroy -plugin DD4hep_ElementTable -type xml
  REGEX_PASS "formula=\"Zr\" name=\"Zr\""
  REGEX_FAIL "Exception"
  REGEX_FAIL "FAILED"
  )
#
#  Test long volume IDs Detector setups with placements of
#  identical volumes at different levels of the hierarchy
dd4hep_add_test_reg( ClientTests_MultiPlace
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/SiBarrelMultiSensitiveLongVolID.xml -volmgr -destroy
  -plugin DD4hep_VolumeMgrTest SiTrackerBarrel

  REGEX_PASS "Volume:component1_1                                       IDDesc:OK  \\[S\\]  vid:00200668000000ff system:00ff barrel:0000 layer:0001 module:0033 sensor:0001"
  REGEX_FAIL "FAILED: World transformation DIFFER"
  )
#
#  Test long volume IDs exceeding 32 bit addressing of the form: <id>system:32,barrel:16:-5....</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_LongVoldID
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/SiBarrelMultiSensitiveLongVolID.xml -volmgr -destroy
  -plugin DD4hep_VolumeMgrTest SiTrackerBarrel

  REGEX_PASS "Volume:component1_1                                       IDDesc:OK  \\[S\\]  vid:00200668000000ff system:00ff barrel:0000 layer:0001 module:0033 sensor:0001"
  REGEX_FAIL "FAILED: World transformation DIFFER"
  )
#
#  Test readout strings of the form: <id>system:8,barrel:-2</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_BarrelSides
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/Bitfield_SidesTest.xml -volmgr -destroy
  -plugin DD4hep_DetectorVolumeDump
  -plugin DD4hep_VolumeDump volids
  -plugin DD4hep_VolumeMgrTest all
  REGEX_PASS "Volume:Shell_2                                            IDDesc:OK  \\[S\\]  vid:0000000000000102 system:0002 barrel:0001")
#
#  Test readout strings of the form: <id>system:16,barrel:16:-5</id>
dd4hep_add_test_reg( ClientTests_Bitfield64_BarrelSides2
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -input file:${ClientTestsEx_INSTALL}/compact/Bitfield_SidesTest2.xml -volmgr -destroy
  -plugin DD4hep_DetectorVolumeDump
  -plugin DD4hep_VolumeDump volids
  -plugin DD4hep_VolumeMgrTest all
  REGEX_PASS "Volume:Shell_2                                            IDDesc:OK  \\[S\\]  vid:0000000000010002 system:0002 barrel:0001")
#
#  Test handle casting procedures.
dd4hep_add_test_reg( ClientTests_Check_Handle_Casts
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -volmgr -destroy -input file:${ClientTestsEx_INSTALL}/compact/Check_Handles.xml
  -plugin DD4hep_DetectorVolumeDump
  REGEX_PASS "|   164  casts PASSED     90 casts FAILED                         |")
#
#  Test saving geometry to file
dd4hep_add_test_reg( ClientTests_Save_ROOT_MiniTel_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -volmgr -destroy -input file:${ClientTestsEx_INSTALL}/compact/MiniTel.xml
  -plugin DD4hep_Geometry2ROOT -output MiniTel_geometry.root
  REGEX_PASS "\\+\\+\\+ Successfully saved geometry data to file.")
#
#  Test basic shapes by comparing mesh vertices with reference file
set(ClientTests_ShapeTests)
list(APPEND ClientTests_ShapeTests Box Cone ConeSegment Tube ElTube CutTube Hyperboloid Paraboloid)
list(APPEND ClientTests_ShapeTests EightPointSolid Eightpoint_Reflect_Volume Eightpoint_Reflect_DetElement)
list(APPEND ClientTests_ShapeTests Polycone Polyhedra PseudoTrap PseudoTrap2 Sphere Torus Trap Trd1 Trd2)
list(APPEND ClientTests_ShapeTests TruncatedTube ExtrudedPolygon)
if(${ROOT_VERSION} VERSION_GREATER 6.21.00)
list(APPEND ClientTests_ShapeTests Tessellated)
endif()
foreach (test ${ClientTests_ShapeTests})
  dd4hep_add_test_reg( ClientTests_Check_Shape_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoDisplay file:${ClientTestsEx_INSTALL}/compact/Check_Shape_${test}.xml -load -destroy
      REGEX_PASS "Shape verification SUCCESSFUL"
      REGEX_FAIL "ERROR;FAILED" )
endforeach()
#
#  Test Volume division procedure.
dd4hep_add_test_reg( ClientTests_Check_VolumeDivisionTest
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -destroy -input file:${ClientTestsEx_INSTALL}/compact/VolumeDivisionTest.xml
  -plugin DD4hep_VolumeDump
  REGEX_PASS "Checked 8 physical volume placements")
#
#  Test Setting temperature and pressure to material
dd4hep_add_test_reg( ClientTests_Check_Temp_Pressure_Air
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -destroy -input file:${ClientTestsEx_INSTALL}/compact/Check_Air.xml
  -plugin DD4hep_MaterialTable -name Vacuum
  REGEX_PASS "Temp=111 \\[Kelvin\\] Pressure=1e-12 \\[hPa\\] state=Undefined"
  REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
#
#  Test Setting temperature and pressure to material
dd4hep_add_test_reg( ClientTests_Check_Temp_Pressure_Air_NTP
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -destroy -input file:${ClientTestsEx_INSTALL}/compact/Check_Air_NTP.xml
  -plugin DD4hep_MaterialTable -name Vacuum
  REGEX_PASS "Temp=293 \\[Kelvin\\] Pressure=1013.2 \\[hPa\\] state=Undefined"
  REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
#
#  Test Setting temperature and pressure to material
dd4hep_add_test_reg( ClientTests_Check_Temp_Pressure_Air_STP
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoPluginRun
  -destroy -input file:${ClientTestsEx_INSTALL}/compact/Check_Air_STP.xml
  -plugin DD4hep_MaterialTable -name Vacuum
  REGEX_PASS "Temp=273 \\[Kelvin\\] Pressure=1000 \\[hPa\\] state=Undefined"
  REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
#
#  Test reflection matrices
dd4hep_add_test_reg( ClientTests_Check_reflection_matrices
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  EXEC_ARGS  geoDisplay
  -input file:${ClientTestsEx_INSTALL}/compact/ReflectionMatrices.xml
  -load -destroy -print WARNING
  REGEX_PASS "Analysed 7 right handed and 10 left handed matrices"
  REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
#
# only if root version > 6.19: MaterialTester
#
foreach (test Assemblies BoxTrafos CaloEndcapReflection IronCylinder LheD_tracker MagnetFields  
    MiniTel SectorBarrelCalorimeter SiliconBlock NestedSimple NestedDetectors 
    MultiCollections MultiSegmentations )

  # Test materials in volumes. Test here the proper access to the materials from the volumes,
  # which was broken at some point.
  dd4hep_add_test_reg( ClientTests_volume_materials_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoPluginRun -print WARNING -volmgr -destroy
      -input  file:${ClientTestsEx_INSTALL}/compact/${test}.xml
      -plugin DD4hep_VolumeDump -materials
      REGEX_PASS "\\+\\+\\+ Checked [1-9][0-9]* materials in volume placements.   0 are BAD." )
            
  # Test format conversions
  ##dd4hep_add_test_reg( ClientTests_converter_gdml_${test}
  ##  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
  ##  EXEC_ARGS  geoConverter -compact2gdml
  ##                          -input file:${ClientTestsEx_INSTALL}/compact/${test}.xml
  ##                          -output file:${test}.gdml
  ##  REGEX_PASS " Successfully extracted GDML to" )
  foreach( type description vis )
    dd4hep_add_test_reg( ClientTests_converter_${type}_${test}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  geoConverter -compact2${type}
                              -input file:${ClientTestsEx_INSTALL}/compact/${test}.xml
                              -output file:${test}.${type}
      REGEX_PASS " Handled [1-9][0-9]* volumes" )
  endforeach(type)
endforeach()
#
#
# Note:
# IronCylinder has no segmentation!
# MaterialTester no geometry
# SectorBarrelCalorimeter is bad
foreach (test Assemblies BoxTrafos CaloEndcapReflection LheD_tracker MagnetFields MiniTel SiliconBlock 
         NestedSimple NestedDetectors MultiCollections )
  #
  # Test material scans in [origine to 10 meters in y]
  dd4hep_add_test_reg( ClientTests_material_scan_${test}
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  materialScan file:${ClientTestsEx_INSTALL}/compact/${test}.xml 0 0 0 0 10000 0
    REGEX_PASS " Average Material " )
  #
  # Geant4 material scan. From position=0,0,0 to end-of-world 
  if (DD4HEP_USE_GEANT4)
    dd4hep_add_test_reg( ClientTests_g4material_scan_${test}_LONGTEST
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${DD4hep_ROOT}/bin/g4MaterialScan --compact=file:${ClientTestsEx_INSTALL}/compact/${test}.xml
                        "--position=0,0,0" "--direction=0,1,0"
      REGEX_PASS " Terminate Geant4 and delete associated actions." )
  endif(DD4HEP_USE_GEANT4)
endforeach()
#
#
#
foreach (test BoxTrafos CaloEndcapReflection IronCylinder MiniTel SiliconBlock NestedSimple MultiCollections )
  #
  #  Read data from XML file. Then parse the pure XML string.
  dd4hep_add_test_reg( ClientTests_parse_xml_string_${test}
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  geoPluginRun -destroy -print WARNING
          -plugin DD4hep_XML_InMemory -input ${ClientTestsEx_INSTALL}/compact/${test}.xml
          -plugin DD4hep_VolumeDump   -materials
      REGEX_PASS "\\+\\+\\+ Checked [1-9][0-9]* materials in volume placements.   0 are BAD." )
  #
  # ROOT Geometry checks
  dd4hep_add_test_reg( ClientTests_check_geometry_${test}_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/bin/checkGeometry --compact=file:${ClientTestsEx_INSTALL}/compact/${test}.xml
                      --full=true --ntracks=10
    REGEX_PASS " Execution finished..." )
  #
  # ROOT Geometry overlap checks
  dd4hep_add_test_reg( ClientTests_check_overlaps_${test}_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/bin/checkOverlaps --compact=file:${ClientTestsEx_INSTALL}/compact/${test}.xml
                      --tolerance=0.1
    REGEX_PASS " Execution finished..." )
endforeach()
#
#      EXEC_ARGS  test_with_root.sh ${script}
#
if (DD4HEP_USE_GEANT4)
  #
  #  Test Setting temperature and pressure to material
  dd4hep_add_test_reg( ClientTests_sim_Check_Temp_Pressure_Air
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/Check_Air.py
    -geometry  ${ClientTestsEx_INSTALL}/compact/Check_Air.xml batch
    REQUIRES   DDG4 Geant4
    REGEX_PASS "Imean:  85.538 eV   temperature: 333.33 K  pressure:   2.22 atm"
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
  #
  # Geant4 test with gdml input file (LHCb:FT)
  dd4hep_add_test_reg( ClientTests_g4_gdml_detector
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/bin/g4GeometryScan --compact=${ClientTestsEx_INSTALL}/compact/GdmlDetector.xml
                      --position=200,200,-2000 --direction=0,0,1
    REGEX_PASS "   122   2777.0000  5200.000  "
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
  #
  # Geant4 test with gdml input file (LHCb:MT)
  dd4hep_add_test_reg( ClientTests_g4_gdml_MT
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/bin/g4GeometryScan --compact=${ClientTestsEx_INSTALL}/compact/MT.xml
                      --position=200,200,7900 --direction=0,0,1
    REGEX_PASS "    15   2777.0000  4210.000   "
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error;FATAL" )
  #
  # Geant4 test if production cuts are processed
  dd4hep_add_test_reg( ClientTests_sim_MiniTel_prod_cuts
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/MiniTelRegions.py batch
    REGEX_PASS "minitel_region_1: Set cut  \\[gamma/0\\] = 5.000"
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  #
  # Geant4 test if production cuts are processed
  dd4hep_add_test_reg( ClientTests_sim_MiniTel_limitset
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/MiniTelRegions.py batch
    REGEX_PASS "LimitSet:    Particle type: mu-                PDG: 13     : 3.000000"
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  #
  # Test of an example user analysis creating an N-tuple instead of an output file with events
  dd4hep_add_test_reg( ClientTests_sim_UserAnalysis
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/MiniTelEnergyDeposits.py batch
    REGEX_PASS "Entries :      200 "
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  #
  # Test of an example user analysis creating an N-tuple instead of an output file with events
  dd4hep_add_test_reg( ClientTests_sim_TrackingRegion
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
    EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/TrackingRegion.py batch
    REGEX_PASS "Placement tracking_volume_1 not converted \\[Veto'ed for simulation\\]"
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  #
  # Geant4 full simulation checks of simple detectors
  foreach(script Assemblies LheD_tracker MiniTel MiniTel_hepmc NestedDetectors )
    dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/${script}.py batch
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)
  #
  # Geant4 full simulation checks of multi-collection/segmentation detectors
  foreach(script MultiCollections MultiSegmentations MultiSegmentCollections )
    dd4hep_add_test_reg( ClientTests_sim_${script}
      COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_ClientTests.sh"
      EXEC_ARGS  python ${ClientTestsEx_INSTALL}/scripts/MultiCollections.py 
                        -compact ${ClientTestsEx_INSTALL}/compact/${script}.xml -batch
      REGEX_PASS NONE
      REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )
  endforeach(script)

endif(DD4HEP_USE_GEANT4)
