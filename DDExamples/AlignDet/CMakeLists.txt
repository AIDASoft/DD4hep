cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
                     ${XERCESC_INCLUDE_DIR})

if(DD4HEP_USE_XERCESC)
  file(GLOB sources src/*.cpp)
endif()

add_executable(Aligntest test/main.cpp ${sources})
target_link_libraries(Aligntest DD4hepCore ${ROOT_LIBRARIES} Rint Reflex)

