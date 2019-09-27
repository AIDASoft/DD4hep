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
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

dd4hep_set_compiler_flags()
#==========================================================================
if(TARGET XercesC::XercesC)
  SET(OPT_XERCESC XercesC::XercesC)
endif()

#-----------------------------------------------------------------------------------
dd4hep_add_plugin(PersistencyExample SOURCES src/*.cpp
  USES DD4hep::DDCore ROOT::Core ROOT::Geom ROOT::GenVector ${OPT_XERCESC}
  )
install(TARGETS PersistencyExample LIBRARY DESTINATION lib)
dd4hep_configure_scripts (Persistency DEFAULT_SETUP WITH_TESTS )
#--------------------------------------------------------------------------
#
#  Test saving conditions to ROOT file
dd4hep_add_test_reg( Persist_Conditions_Save
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun
  -plugin    DD4hep_PersistencyExample_write_cond -output Conditions.root
  REGEX_PASS "\\+\\+\\+ PASSED Wrote 14 conditions to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;WriteObjectAny"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager
dd4hep_add_test_reg( Persist_Conditions_Restore
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun
  -plugin    DD4hep_PersistencyExample_read_cond -input Conditions.root
  DEPENDS    Persist_Conditions_Save
  REGEX_PASS "\\+\\+\\+ Read successfully 14 conditions. Result=172"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test saving geometry to ROOT file
dd4hep_add_test_reg( Persist_MiniTel_Save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun
  -volmgr -destroy -input file:${CMAKE_CURRENT_SOURCE_DIR}/../ClientTests/compact/MiniTel.xml
  -plugin    DD4hep_Geometry2ROOT -output MiniTel_geometry.root
  REGEX_PASS "\\+\\+\\+ Successfully saved geometry data to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;WriteObjectAny"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager
dd4hep_add_test_reg( Persist_MiniTel_Restore_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ Successfully loaded detector description from file"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager loading+nominals
dd4hep_add_test_reg( Persist_MiniTel_Restore_VolMgr1_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  -plugin    DD4hep_CheckVolumeManager
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 20 VolumeManager contexts. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: DetElement nominal alignments
dd4hep_add_test_reg( Persist_MiniTel_Restore_Nominal_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  -plugin    DD4hep_CheckNominals
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 20 DetElements. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Sensitive detectors
dd4hep_add_test_reg( Persist_MiniTel_Restore_Sensitives_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  -plugin    DD4hep_CheckSensitives
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 10 SensitiveDetector objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Readout segmentations
dd4hep_add_test_reg( Persist_MiniTel_Restore_Segmentations_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  -plugin    DD4hep_CheckSegmentations
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 10 readout segmentations. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Readout structures
dd4hep_add_test_reg( Persist_MiniTel_Restore_Readouts_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader MiniTel_geometry.root
  -plugin    DD4hep_CheckReadouts
  DEPENDS    Persist_MiniTel_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 10 readout objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test saving geometry to ROOT file
dd4hep_add_test_reg( Persist_CLICSiD_Save_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun
  -volmgr    -destroy -input file:$ENV{DD4hepINSTALL}/DDDetectors/compact/SiD.xml
  -plugin    DD4hep_Geometry2ROOT -output CLICSiD_geometry.root
  REGEX_PASS "\\+\\+\\+ Successfully saved geometry data to file."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;WriteObjectAny"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager
dd4hep_add_test_reg( Persist_CLICSiD_Restore_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ Successfully loaded detector description from file"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Volume Manager loading+nominals
#  Note: BeamCal has a problem. Need to be taken into account
dd4hep_add_test_reg( Persist_CLICSiD_Restore_VolMgr1_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_CheckVolumeManager
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 29366 VolumeManager contexts. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Test Volume Manager results
dd4hep_add_test_reg( Persist_CLICSiD_Restore_VolMgr2_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_VolumeMgrTest SiTrackerBarrel
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED: Checked 81306 objects. Num.Errors:0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: DetElement nominal alignments
#  Note: BeamCal has a problem. Need to be taken into account
dd4hep_add_test_reg( Persist_CLICSiD_Restore_Nominal_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_CheckNominals
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ FAILED Checked 15994 DetElements. Num.Errors: 50"
  REGEX_FAIL "EXCEPTION;Exception;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Sensitive detectors
dd4hep_add_test_reg( Persist_CLICSiD_Restore_Sensitives_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_CheckSensitives
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 14 SensitiveDetector objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Readout segmentations
dd4hep_add_test_reg( Persist_CLICSiD_Restore_Segmentations_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_CheckSegmentations
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 9 readout segmentations. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
#  Test restoring geometry from ROOT file: Readout structures
dd4hep_add_test_reg( Persist_CLICSiD_Restore_Readouts_LONGTEST
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
  EXEC_ARGS  geoPluginRun -print WARNING
  -plugin    DD4hep_RootLoader CLICSiD_geometry.root
  -plugin    DD4hep_CheckReadouts
  DEPENDS    Persist_CLICSiD_Save_LONGTEST
  REGEX_PASS "\\+\\+\\+ PASSED Checked 14 readout objects. Num.Errors: 0"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception;FAILED;TStreamerInfo"
  )
#
if (DD4HEP_USE_GEANT4)
  #
  #
  #  Test restoring geometry from ROOT file and start Geant4
  dd4hep_add_test_reg( Persist_CLICSiD_Geant4_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_Persistency.sh"
    EXEC_ARGS  python ${CMAKE_CURRENT_SOURCE_DIR}/../CLICSiD/scripts/CLICSiD_LoadROOTGeo.py  batch
    REQUIRES   DDG4 Geant4
    REGEX_PASS "\\+\\+\\+ Finished run 0 after 10 events \\(10 events in total\\)"
    REGEX_FAIL "Exception;EXCEPTION;ERROR"
    )
endif()
