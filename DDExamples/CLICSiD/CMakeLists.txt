cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})

file(GLOB sources src/*.cpp)
include(DD4hep_XML_setup)

add_library(CLICSiD SHARED ${sources})
target_link_libraries(CLICSiD DD4hepCore ${ROOT_LIBRARIES})
dd4hep_generate_rootmap(CLICSiD)
