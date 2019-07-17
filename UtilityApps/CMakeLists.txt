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
# dd4hep_package(UtilityApps
#   USES DDCore
#       [ROOT REQUIRED COMPONENTS Geom]
#   )
#-----------------------------------------------------------------------------------
add_executable(geoDisplay src/display.cpp)
target_link_libraries(geoDisplay DDCore)
#-----------------------------------------------------------------------------------
add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DDCore)
#-----------------------------------------------------------------------------------
add_executable(geoPluginRun src/plugin_runner.cpp)
target_link_libraries(geoPluginRun DDCore)
#-----------------------------------------------------------------------------------
add_executable(dumpBfield src/dumpBfield.cpp)
target_link_libraries(dumpBfield DDCore)
#-----------------------------------------------------------------------------------
add_executable(print_materials src/print_materials.cpp)
target_link_libraries(print_materials DDCore DDRec)
#-----------------------------------------------------------------------------------
add_executable(materialScan src/materialScan.cpp)
target_link_libraries(materialScan ROOT::Core ROOT::Geom DDRec)
#-----------------------------------------------------------------------------------
add_executable(materialBudget  src/materialBudget.cpp)
target_link_libraries(materialBudget ROOT::Core ROOT::Geom DDRec ROOT::Hist)
#-----------------------------------------------------------------------------------
add_executable(graphicalScan src/graphicalScan.cpp)
target_link_libraries(graphicalScan ROOT::Core ROOT::Geom DDRec ROOT::Hist)
#-----------------------------------------------------------------------------------


if(TARGET Geant4::Interface)
  add_executable(dumpdetector src/dumpdetector.cpp)
  target_link_libraries(dumpdetector Geant4::Interface DDCore DDRec)
endif()

# #-----------------------------------------------------------------------------------
if(BUILD_TESTING)
  if(DD4HEP_USE_LCIO)
    add_executable(test_surfaces src/test_surfaces.cpp)
    target_link_libraries(test_surfaces DDRec DDTest LCIO::LCIO)

    add_executable(test_cellid_position_converter src/test_cellid_position_converter.cpp)
    target_link_libraries(test_cellid_position_converter DDRec DDTest LCIO::LCIO)
  endif()
  #-----------------------------------------------------------------------------------
endif()
new_dd4hep_add_dictionary( G__eve
  SOURCES src/EvNavHandler.h
  LINKDEF src/LinkDef.h
  DEPENDS DDCore ROOT::Geom
  )

# #-----------------------------------------------------------------------------------
if (DD4HEP_USE_LCIO)
  new_dd4hep_add_dictionary( G__eve1
    SOURCES src/EvNavHandler.h
    LINKDEF src/LinkDef.h
    DEPENDS DDCore  ROOT::Geom
    )
  add_executable(teveLCIO G__eve1.cxx src/teve_display.cpp src/next_event_lcio.cpp)
  target_link_libraries(teveLCIO DDRec LCIO::LCIO ROOT::Core ROOT::Eve ROOT::Gui ROOT::Graf3d ROOT::RGL)
endif()

# #-----------------------------------------------------------------------------------
add_executable(teveDisplay src/teve_display.cpp src/next_event_dummy.cpp G__eve.cxx)
target_link_libraries(teveDisplay DDRec ROOT::Core ROOT::Eve ROOT::Gui ROOT::Graf3d ROOT::RGL )

INSTALL(TARGETS geoDisplay
  geoConverter
  geoPluginRun
  dumpBfield
  print_materials
  materialScan
  materialBudget
  graphicalScan
  dumpdetector
  teveDisplay
  RUNTIME DESTINATION bin
  )