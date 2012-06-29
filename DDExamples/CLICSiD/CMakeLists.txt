cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
		     ${XERCESC_INCLUDE_DIR})

file(GLOB sources src/*.cpp)
file(GLOB headers include/*.h)

if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
endif()

add_library(CLICSiD SHARED ${sources})
target_link_libraries(CLICSiD DD4hepCore ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${libraries})
