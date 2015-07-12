# $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================

cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DDCore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${DDSegmentation_INCLUDE_DIRS} 
  ${DDPluginManager_INCLUDE_DIRS})

file(GLOB sources src/*.cpp src/Evaluator/*.cpp src/XML/*.cpp)
file(GLOB plugin_sources src/plugins/*.cpp)

#---Add Library-------------------------------------------------------------------
if(DD4HEP_USE_BOOST)
  file(GLOB parser_sources src/parsers/*.cpp )
  add_definitions(-DDD4HEP_USE_BOOST)
else()
  file(GLOB parser_sources)
endif()

file(GLOB headers include/DD4hep/*.h)
file(GLOB internal_headers include/DD4hep/objects/*.h)
list(APPEND headers ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/BitField64.h)
list(APPEND headers ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/Segmentation.h)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/DetFactoryHelper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/Factories.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/Plugins.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/IoStreams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/SurfaceInstaller.h
  )
root_generate_dictionary( G__DD4hep ${headers} ${internal_headers} LINKDEF include/ROOT/LinkDef.h)
list(APPEND sources G__DD4hep.cxx)

if(DD4HEP_USE_PYROOT)
  add_definitions(-DDD4HEP_USE_PYROOT)
  set(libraries  ${libraries} PyROOT)
endif()

add_library(DDCore SHARED ${sources} ${parser_sources} )

target_link_libraries(DDCore ${ROOT_LIBRARIES} ${XML_LIBRARIES} ${DDPluginManager_LIBRARIES} ${DDSegmentation_LIBRARIES} Geom ${libraries})

## generates rootmap and installs the library
add_dd4hep_plugin(DDCorePlugins SHARED ${plugin_sources})
target_link_libraries(DDCorePlugins ${ROOT_LIBRARIES} ${XML_LIBRARIES} DDCore Geom ${libraries})

SET_TARGET_PROPERTIES( DDCore DDCorePlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#--- install target-------------------------------------
install(DIRECTORY include/DD4hep  include/XML
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DDCore
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

# to do: add corresponding uninstall...
#-------------------------------------------------------
