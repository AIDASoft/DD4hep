cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DDCore)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${XERCESC_INCLUDE_DIR})

file(GLOB sources src/*.cpp src/Evaluator/*.cpp)

if(DD4HEP_USE_PYROOT)
  file(GLOB headers include/DD4hep/*.h)
  list(REMOVE_ITEM headers ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/DetFactoryHelper.h
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/DD4hep/Factories.h)
  ROOT_GENERATE_DICTIONARY( G__DD4hep ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__DD4hep.cxx)
  add_definitions(-DDD4HEP_USE_PYROOT)
  set(libraries  ${libraries} PyROOT)
endif()

if(DD4HEP_USE_XERCESC)
  file(GLOB xmlsources src/XML/*.cpp src/compact/*.cpp)
  list(APPEND sources ${xmlsources})
  add_definitions(-DDD4HEP_USE_XERCESC)
  set(libraries ${libraries} Reflex)
endif()

add_library(DD4hepCore SHARED ${sources})
target_link_libraries(DD4hepCore ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} Geom ${libraries})


