cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)


include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR})

add_executable(ILDExDisplay src/display.cpp)
target_link_libraries(ILDExDisplay DD4hepCore ILDEx ${ROOT_LIBRARIES} Rint)

