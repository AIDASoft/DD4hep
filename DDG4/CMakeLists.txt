#---Find Geant4-------------------------------------------------------------------
find_package(Geant4 REQUIRED)
if(NOT Geant4_clhep_FOUND)
  find_package(CLHEP REQUIRED)
  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
endif()

#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIRS})

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)
add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
