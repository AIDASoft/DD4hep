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

file(GLOB DDAlign_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(DDAlign SHARED ${DDAlign_SOURCES} )

target_include_directories(DDAlign
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDAlign
  PUBLIC
  DDCore
  )

new_dd4hep_add_plugin(DDAlignPlugins
  SOURCES src/plugins/*.cpp
  LINK_LIBRARIES DDAlign
  )

INSTALL(TARGETS DDAlign EXPORT DD4hep DESTINATION lib)
