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

file(GLOB DDParsers_SOURCES src/Parsers/*.cpp src/Evaluator/*.cpp)
if(Boost_FOUND)
  file(GLOB DDParsers_SPIRIT_SOURCES src/Spirit/*.cpp)
  set(BOOST_LIB Boost::boost)
endif()

add_library(DDParsers SHARED ${DDParsers_SOURCES} ${DDParsers_SPIRIT_SOURCES})

target_compile_definitions(DDParsers INTERFACE
  $<$<BOOL:Boost_FOUND>:BOOST_SPIRIT_USE_PHOENIX_V3>
  )

target_include_directories(DDParsers
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDParsers PUBLIC
  ROOT::Core
  ${BOOST_LIB}
  ${XML_LIBRARIES}
  )

set_target_properties(DDParsers PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
install(TARGETS DDParsers EXPORT DD4hep LIBRARY DESTINATION lib)
install(DIRECTORY include/Parsers include/Evaluator DESTINATION include)
