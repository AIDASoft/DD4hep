
cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(ILDExample)
 

SET( Geant4_CONFIG_DEBUG TRUE )
find_package(Geant4 REQUIRED COMPONENTS gdml qt )
include(${Geant4_USE_FILE})


GEANT4_COLLATE_APPLICATION_SOURCES(ILDEXAMPLE_SOURCES)

add_executable(ILDExample ${ILDEXAMPLE_SOURCES})
target_link_libraries(ILDExample ${Geant4_LIBRARIES} )

 
