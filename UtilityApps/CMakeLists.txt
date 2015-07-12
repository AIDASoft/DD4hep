# $Id$
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================

cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
                     ${CMAKE_SOURCE_DIR}/DDSegmentation/include
                     ${CMAKE_SOURCE_DIR}/DDSurfaces/include
                     ${CMAKE_SOURCE_DIR}/DDRec/include  
		     ${CMAKE_SOURCE_DIR}/DDTest/include )

#-----------------------------------------------------------------------------------
add_executable(geoDisplay src/display.cpp)
target_link_libraries(geoDisplay DDCore ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------
add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DDCore ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------
add_executable(geoPluginRun src/plugin_runner.cpp)
target_link_libraries(geoPluginRun DDCore ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------
add_executable( print_materials src/print_materials.cpp)
target_link_libraries(print_materials DDCore DDRec ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------
add_executable( materialScan src/materialScan.cpp)
target_link_libraries(materialScan DDCore DDRec ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------
add_executable( dumpdetector  src/dumpdetector.cpp)
target_link_libraries(dumpdetector DDCore DDRec ${ROOT_LIBRARIES})
#-----------------------------------------------------------------------------------


root_generate_dictionary( G__teve src/EvNavHandler.h LINKDEF src/LinkDef.h)
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories( ${LCIO_INCLUDE_DIRS} )
  add_executable(teveDisplay src/teve_display.cpp G__teve.cxx src/next_event_lcio.cpp)

  add_executable(test_surfaces src/test_surfaces.cpp )
  target_link_libraries(test_surfaces DDCore DDRec ${LCIO_LIBRARIES}  ${ROOT_LIBRARIES})
else()
  add_executable(teveDisplay src/teve_display.cpp G__teve.cxx src/next_event_dummy.cpp)
endif()
target_link_libraries( teveDisplay DDCore ${ROOT_EVE_LIBRARIES} DDRec ${LCIO_LIBRARIES} )

#--- install target-------------------------------------

if(DD4HEP_USE_LCIO)
  install(TARGETS geoDisplay geoConverter geoPluginRun teveDisplay print_materials materialScan dumpdetector test_surfaces
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
else()
  install(TARGETS geoDisplay geoConverter geoPluginRun teveDisplay print_materials materialScan dumpdetector
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
endif()

# to do: add corresponding uninstall...
#-------------------------------------------------------
