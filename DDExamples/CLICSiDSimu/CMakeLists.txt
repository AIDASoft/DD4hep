cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(Geant4 REQUIRED)
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
find_package(Geant4 REQUIRED)



if(Geant4_gdml_FOUND)
  add_definitions(-DGEANT4_HAS_GDML)
endif()

include(DD4hep_XML_setup)
set(libraries CLICSiD ${XML_LIBRARIES})

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDG4/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR}
                     ${Geant4_INCLUDE_DIRS})

file(GLOB sources src/*.cpp)
add_executable(CLICSiDSimu CLICSiDSimu.cpp ${sources})
target_link_libraries(CLICSiDSimu DD4hepCore DD4hepG4 ${libraries} ${Geant4_LIBRARIES})

