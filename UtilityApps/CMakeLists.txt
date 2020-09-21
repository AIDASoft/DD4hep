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
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME UtilityApps)

add_executable(geoDisplay src/display.cpp)
target_link_libraries(geoDisplay DD4hep::DDCore)
#-----------------------------------------------------------------------------------
add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DD4hep::DDCore)
#-----------------------------------------------------------------------------------
add_executable(geoPluginRun src/plugin_runner.cpp)
target_link_libraries(geoPluginRun DD4hep::DDCore)
#-----------------------------------------------------------------------------------
add_executable(dumpBfield src/dumpBfield.cpp)
target_link_libraries(dumpBfield DD4hep::DDCore)
#-----------------------------------------------------------------------------------
add_executable(print_materials src/print_materials.cpp)
target_link_libraries(print_materials DD4hep::DDCore DD4hep::DDRec)
#-----------------------------------------------------------------------------------
add_executable(materialScan src/materialScan.cpp)
target_link_libraries(materialScan DD4hep::DDRec ROOT::Core ROOT::Geom)
#-----------------------------------------------------------------------------------
add_executable(materialBudget  src/materialBudget.cpp)
target_link_libraries(materialBudget DD4hep::DDRec ROOT::Core ROOT::Geom ROOT::Hist)
#-----------------------------------------------------------------------------------
add_executable(graphicalScan src/graphicalScan.cpp)
target_link_libraries(graphicalScan  DD4hep::DDRec ROOT::Core ROOT::Geom ROOT::Hist)
#-----------------------------------------------------------------------------------

if(TARGET Geant4::Interface)
  add_executable(dumpdetector src/dumpdetector.cpp)
  target_link_libraries(dumpdetector DD4hep::DDCore DD4hep::DDRec Geant4::Interface)
  LIST(APPEND OPTIONAL_EXECUTABLES dumpdetector)
endif()

# the web display tool depends on the ROOT7 version of DDEve
if(TARGET ROOT::ROOTEve)
  add_executable(geoWebDisplay src/webdisplay.cpp)
  target_link_libraries(geoWebDisplay DD4hep::DDCore)
  LIST(APPEND OPTIONAL_EXECUTABLES geoWebDisplay)
endif()

# #-----------------------------------------------------------------------------------
if(BUILD_TESTING)
  if(DD4HEP_USE_LCIO)
    add_executable(test_surfaces src/test_surfaces.cpp)
    target_link_libraries(test_surfaces DD4hep::DDRec DD4hep::DDTest LCIO::LCIO)

    add_executable(test_cellid_position_converter src/test_cellid_position_converter.cpp)
    target_link_libraries(test_cellid_position_converter DD4hep::DDRec DD4hep::DDTest LCIO::LCIO)

    LIST(APPEND OPTIONAL_EXECUTABLES test_surfaces)
    LIST(APPEND OPTIONAL_EXECUTABLES test_cellid_position_converter)
  endif()
  #-----------------------------------------------------------------------------------
endif()
dd4hep_add_dictionary( G__eve
  SOURCES src/EvNavHandler.h
  LINKDEF src/LinkDef.h
  USES    DD4hep::DDCore ROOT::Geom
  )

# #-----------------------------------------------------------------------------------
if (DD4HEP_USE_LCIO)
  dd4hep_add_dictionary( G__eve1
    SOURCES src/EvNavHandler.h
    LINKDEF src/LinkDef.h
    USES    DD4hep::DDCore ROOT::Geom
    )
  add_executable(teveLCIO G__eve1.cxx src/teve_display.cpp src/next_event_lcio.cpp)
  target_link_libraries(teveLCIO DD4hep::DDRec LCIO::LCIO ROOT::Core ROOT::Eve ROOT::Gui ROOT::Graf3d ROOT::RGL)
  LIST(APPEND OPTIONAL_EXECUTABLES teveLCIO)
endif()

# #-----------------------------------------------------------------------------------
add_executable(teveDisplay src/teve_display.cpp src/next_event_dummy.cpp G__eve.cxx)
target_link_libraries(teveDisplay DD4hep::DDRec ROOT::Core ROOT::Eve ROOT::Gui ROOT::Graf3d ROOT::RGL )

INSTALL(TARGETS geoDisplay
  geoConverter
  geoPluginRun
  dumpBfield
  print_materials
  materialScan
  materialBudget
  graphicalScan
  teveDisplay
  ${OPTIONAL_EXECUTABLES}
  EXPORT DD4hep
  RUNTIME DESTINATION bin
  )
