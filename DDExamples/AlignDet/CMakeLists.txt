cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})

include(DD4hep_XML_setup)
file(GLOB sources src/*.cpp)

add_executable(Aligntest test/main.cpp ${sources})
target_link_libraries(Aligntest DD4hepCore ${ROOT_LIBRARIES} Rint Reflex)

