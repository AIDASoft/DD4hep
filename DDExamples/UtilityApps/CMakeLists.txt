cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})

add_executable(displayGeo src/display.cpp)
target_link_libraries(displayGeo DD4hepCore)

add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DD4hepCore)
