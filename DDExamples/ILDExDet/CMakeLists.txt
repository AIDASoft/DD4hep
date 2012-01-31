cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(ROOT)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})


file(GLOB sources src/*.cpp)
ROOT_GENERATE_DICTIONARY( G__ILDEx include/ILDExTPC.h 
                                   LINKDEF include/LinkDef.h)

add_library(ILDEx SHARED ${sources} G__ILDEx.cxx)
target_link_libraries(ILDEx DetDesc)

