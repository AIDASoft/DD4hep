#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
# dd4hep_package(    DDParsers
#   USES             [ROOT REQUIRED]
#   OPTIONAL         XERCESC
#   INCLUDE_DIRS     include
#   INSTALL_INCLUDES include/Evaluator include/Parsers )

# dd4hep_add_package_library ( DDParsers
#   INCLUDE_DIRS
#   LINK_LIBRARIES
#   SOURCES        src/Parsers/*.cpp src/Evaluator/*.cpp
#   OPTIONAL       [BOOST SOURCES src/Spirit/*.cpp]
#   )


file(GLOB DDParsers_SOURCES src/Parsers/*.cpp src/Evaluator/*.cpp)
if(Boost_FOUND)
  file(GLOB DDParsers_SPIRIT_SOURCES src/Spirit/*.cpp)
endif()

add_library(DDParsers SHARED ${DDParsers_SOURCES} ${DDParsers_SPIRIT_SOURCES})
#target_compile_definitions(DDParsers PUBLIC BOOST_SPIRIT_USE_PHOENIX_V3)

target_include_directories(DDParsers
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

target_include_directories(DDParsers
  SYSTEM
  PUBLIC
  $<BUILD_INTERFACE:${ROOT_INCLUDE_DIRS}>
  )

target_link_libraries(DDParsers PUBLIC
  $<BUILD_INTERFACE:Boost::boost>
  ${XML_LIBRARIES}
  # $<BUILD_INTERFACE:${ROOT_LIBRARIES}>
  )

INSTALL(TARGETS DDParsers EXPORT DD4hep DESTINATION lib)