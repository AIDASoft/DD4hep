cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DDCore)

find_package(ROOT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})

file(GLOB detdesc_sources src/*.cpp src/Evaluator/*.cpp)
file(GLOB detdesc_headers include/*.h)
list(REMOVE_ITEM detdesc_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h)

ROOT_GENERATE_DICTIONARY( G__DetDesc ${detdesc_headers} LINKDEF include/LinkDef.h)

add_library(DetDesc SHARED ${detdesc_sources} G__DetDesc.cxx)
target_link_libraries(DetDesc ${ROOT_LIBRARIES} Geom)

