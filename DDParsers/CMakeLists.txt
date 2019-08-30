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

dd4hep_print("|++> Will build DDParsers")

file(GLOB DDParsers_SOURCES src/Parsers/*.cpp src/Evaluator/*.cpp)
file(GLOB DDParsers_SPIRIT_SOURCES src/Spirit/*.cpp)

add_library(DDParsers SHARED ${DDParsers_SOURCES} ${DDParsers_SPIRIT_SOURCES})
add_library(DD4hep::DDParsers ALIAS DDParsers)

target_compile_definitions(DDParsers INTERFACE BOOST_SPIRIT_USE_PHOENIX_V3)

target_include_directories(DDParsers
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDParsers PUBLIC ROOT::Core Boost::boost ${XML_LIBRARIES})

set_target_properties(DDParsers PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
install(TARGETS DDParsers EXPORT DD4hep LIBRARY DESTINATION lib)
install(DIRECTORY include/Parsers include/Evaluator DESTINATION include)
