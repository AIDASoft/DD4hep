cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(Geant4 REQUIRED)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/VGM/packages/RootGM/include
                     ${CMAKE_SOURCE_DIR}/VGM/packages/Geant4GM/include
                     ${CMAKE_SOURCE_DIR}/VGM/packages/BaseVGM/include
                     ${CMAKE_SOURCE_DIR}/VGM/packages/VGM/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR}
                     ${Geant4_INCLUDE_DIR})

file(GLOB sources src/*.cpp)
add_executable(ILDExSimu ILDExSimu.cpp ${sources})
target_link_libraries(ILDExSimu DD4hepCore ILDEx Geant4GM RootGM ${Geant4_LIBRARIES})

