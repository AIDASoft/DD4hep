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
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDAlign)

file(GLOB DDAlign_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(DDAlign ${DDAlign_SOURCES})
add_library(DD4hep::DDAlign ALIAS DDAlign)

target_include_directories(DDAlign
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDAlign PUBLIC DDCore)

dd4hep_add_plugin(DDAlignPlugins SOURCES src/plugins/*.cpp USES DDAlign)

set_target_properties(DDAlign DDAlignPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDAlign DESTINATION include)
install(TARGETS DDAlign DDAlignPlugins EXPORT DD4hep
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
