cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})

file(GLOB sources src/*.cpp)
file(GLOB headers include/*.h)

if(DD4HEP_USE_XERCESC)
  file(GLOB xmlsources src/compact/*.cpp)
  list(APPEND sources ${xmlsources})
  include_directories(${XERCESC_INCLUDE_DIR})
endif()

if(DD4HEP_USE_PYROOT)
  ROOT_GENERATE_DICTIONARY(G__ILDEx ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__ILDEx.cxx)
endif()

add_library(ILDEx SHARED ${sources})
target_link_libraries(ILDEx DD4hepCore)

