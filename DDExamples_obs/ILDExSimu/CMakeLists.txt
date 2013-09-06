cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

find_package(Geant4 REQUIRED ui_all vis_all)
##---Handle the case CLHEP is not included in Geant4------------------------------
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs


find_package(LCIO REQUIRED) 


include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
                     ${CMAKE_SOURCE_DIR}/DDG4/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${ROOT_INCLUDE_DIR}
                     ${Geant4_INCLUDE_DIRS}
                     ${VGM_INCLUDE_DIR}
		     ${LCIO_INCLUDE_DIRS})

file(GLOB sources src/*.cpp)
add_executable(ILDExSimu ILDExSimu.cpp ${sources})
target_link_libraries(ILDExSimu DD4hepCore DD4hepG4 ILDEx ${Geant4_LIBRARIES} ${LCIO_LIBRARIES})
