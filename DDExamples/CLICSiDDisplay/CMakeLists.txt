cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
                     ${XERCESC_INCLUDE_DIR})

add_executable(CLICSiDtest main.cpp)
target_link_libraries(CLICSiDtest CLICSiD DD4hepCore ${ROOT_LIBRARIES} Rint Reflex)
