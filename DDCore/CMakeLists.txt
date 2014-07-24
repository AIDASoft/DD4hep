cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DDCore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR} ${DDSegmentation_INCLUDE_DIRS} )


file(GLOB sources src/*.cpp src/Evaluator/*.cpp src/XML/*.cpp)
file(GLOB plugin_sources src/plugins/*.cpp)

#---Add Library-------------------------------------------------------------------
if(DD4HEP_USE_BOOST)
  file(GLOB parser_sources src/parsers/*.cpp )
else()
  file(GLOB parser_sources )
endif()

file(GLOB headers include/DD4hep/*.h)
file(GLOB internal_headers include/DD4hep/objects/*.h)
list(APPEND headers ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/BitField64.h)
list(APPEND headers ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/Segmentation.h)
list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/DetFactoryHelper.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/Factories.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/Plugins.h
  )
root_generate_dictionary( G__DD4hep ${headers} ${internal_headers} LINKDEF include/ROOT/LinkDef.h)
list(APPEND sources G__DD4hep.cxx)

if(DD4HEP_USE_PYROOT)
  add_definitions(-DDD4HEP_USE_PYROOT)
  set(libraries  ${libraries} PyROOT)
endif()

add_library(DD4hepCore SHARED ${sources} ${parser_sources} )

target_link_libraries(DD4hepCore ${ROOT_LIBRARIES} ${XML_LIBRARIES} ${DDSegmentation_LIBRARIES} Geom Reflex ${libraries})

## generates rootmap and installs the library
add_dd4hep_plugin(DD4hepPlugins SHARED ${plugin_sources})
target_link_libraries(DD4hepPlugins ${ROOT_LIBRARIES} ${XML_LIBRARIES} DD4hepCore Geom Reflex ${libraries})

SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
SET_TARGET_PROPERTIES( DD4hepCore DD4hepPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})


#--- install target-------------------------------------

install(DIRECTORY include/DD4hep  include/XML
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepCore
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

# to do: add corresponding uninstall...
#-------------------------------------------------------