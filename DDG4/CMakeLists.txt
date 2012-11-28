#---Find Geant4-------------------------------------------------------------------
find_package(Geant4 REQUIRED)
if(NOT Geant4_clhep_FOUND)
  find_package(CLHEP REQUIRED)
  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
endif()

if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
endif()

#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIRS}
                    ${XERCESC_INCLUDE_DIR})

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)
if(NOT DD4HEP_USE_XERCESC)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
endif()
add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})
#target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${Geant4_LIBRARIES})
