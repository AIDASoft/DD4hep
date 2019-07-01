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
#---Generate ROOT dictionary-----------------------------------------------------

#find_package (ROOT 6.08 REQUIRED COMPONENTS Geom GenVector Eve Gui Graf3d RGL EG) # COMPONENTS Geom GenVector Eve Gui Graf3d RGL EG
new_dd4hep_add_dictionary( G__DDCond
  SOURCES src/ConditionsDictionary.h
  DEPENDS DDCore DDParsers
  )

file(GLOB DDCond_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#---DDCond library --------------------------------------------------------------
add_library(DDCond SHARED ${DDCond_SOURCES}
  G__DDCond.cxx 
  )

target_link_libraries(DDCond
  PUBLIC
  DDCore
  )

target_include_directories(DDCond
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )


#---DDCond components -----------------------------------------------------------
new_dd4hep_add_plugin(DDCondPlugins
  SOURCES  src/plugins/*.cpp src/Type1/*.cpp
  LINK_LIBRARIES DDCond)

INSTALL(TARGETS DDCond EXPORT DD4hep DESTINATION lib)