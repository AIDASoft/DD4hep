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
dd4hep_configure_output()
dd4hep_package ( OpticalSurfaces MAJOR 0 MINOR 0 PATCH 1
  USES           [ROOT   REQUIRED COMPONENTS Geom GenVector MathCore] 
                 [DD4hep REQUIRED COMPONENTS DDCore DDCond]
  OPTIONAL       XERCESC
  INCLUDE_DIRS   include
  )
set(OpticalSurfaces_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/OpticalSurfaces)
dd4hep_add_plugin(   OpticalSurfacesExample SOURCES src/*.cpp  )
dd4hep_install_dir(  compact scripts DESTINATION ${OpticalSurfaces_INSTALL} )
dd4hep_configure_scripts( OpticalSurfaces DEFAULT_SETUP WITH_TESTS)
#
message (STATUS "OpticalSurfaces: ROOT version: ${ROOT_VERSION}")
#
#---Testing: Load ROOT GDMLMatrix objects from compact
#            and register them to the TGeoManager
dd4hep_add_test_reg( Surfaces_read_GDMLMatrices
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalSurfaces.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${OpticalSurfaces_INSTALL}/compact/ReadGDMLMatrices.xml
  -plugin  DD4hep_Dump_GDMLTables
  REGEX_PASS "Successfully dumped 2 GDML tables with 42 elements."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load ROOT TGeoOpticalSurface objects from compact
#            and register them to the TGeoManager
dd4hep_add_test_reg( Surfaces_read_OpticalSurfaces
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalSurfaces.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${OpticalSurfaces_INSTALL}/compact/ReadOpticalSurfaces.xml
  -plugin  DD4hep_Dump_GDMLTables
  -plugin  DD4hep_Dump_OpticalSurfaces
  REGEX_PASS "Successfully dumped 2 Optical surfaces."
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load ROOT material properties and dump material
dd4hep_add_test_reg( Surfaces_read_MaterialProperties
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalSurfaces.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy -print INFO
  -compact file:${OpticalSurfaces_INSTALL}/compact/ReadMaterialProperties.xml
  -plugin DD4hep_MaterialTable -name Water
  REGEX_PASS "Property: SLOWCOMPONENT        \\[32 x 2\\] --> SLOWCOMPONENT__0x123aff00"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#---Testing: Load OpNovice and dump it to GDML
dd4hep_add_test_reg( Surfaces_OpNovice_write_gdml
  COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalSurfaces.sh"
  EXEC_ARGS  geoPluginRun -volmgr -destroy 
  -compact file:${OpticalSurfaces_INSTALL}/compact/OpNovice.xml
  -plugin DD4hep_ROOTGDMLExtract -output OpNovice.gdml -path /world/BubbleDevice
  REGEX_PASS "File OpNovice.gdml saved"
  REGEX_FAIL " ERROR ;EXCEPTION;Exception"
  )
#
#
if ( "${ROOT_VERSION}" VERSION_GREATER "6.19.00" )
  #
  #---Testing: Load GDML and dump volumes
  dd4hep_add_test_reg( Surfaces_OpNovice_read_gdml
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_OpticalSurfaces.sh"
    EXEC_ARGS  geoPluginRun -volmgr -destroy 
    -compact file:${OpticalSurfaces_INSTALL}/compact/OpNovice_gdml.xml
    -print INFO -destroy -volmgr
    -plugin DD4hep_ROOTGDMLParse -input OpNovice.gdml -path /world/BubbleDevice
    -plugin DD4hep_VolumeDump
    REGEX_PASS "\\+\\+\\+ Checked 5 physical volume placements."
    REGEX_FAIL " ERROR ;EXCEPTION;Exception"
    )
endif()
#
