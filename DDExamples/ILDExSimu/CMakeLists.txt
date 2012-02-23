cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(Geant4 REQUIRED)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDGeant4/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR}
                     ${Geant4_INCLUDE_DIR}
                     ${VGM_INCLUDE_DIR})

file(GLOB sources src/*.cpp)
add_executable(ILDExSimu ILDExSimu.cpp ${sources})
target_link_libraries(ILDExSimu DD4hepCore DD4hepGeant4 ILDEx ${Geant4_LIBRARIES})

