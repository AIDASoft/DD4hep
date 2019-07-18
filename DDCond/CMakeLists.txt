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
dd4hep_add_dictionary(G__DDCond
  SOURCES src/ConditionsDictionary.h
  USES    DDCore DDParsers
  )

file(GLOB DDCond_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(DDCond SHARED ${DDCond_SOURCES} G__DDCond.cxx)

target_include_directories(DDCond
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDCond PUBLIC DDCore)

dd4hep_add_plugin(DDCondPlugins
  SOURCES src/plugins/*.cpp src/Type1/*.cpp
  USES    DDCond
  )

set_target_properties(DDCond DDCondPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDCond DESTINATION include)
install(TARGETS DDCond DDCondPlugins EXPORT DD4hep DESTINATION lib)
