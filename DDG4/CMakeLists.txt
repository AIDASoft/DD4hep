#---Find Geant4-------------------------------------------------------------------

find_package(Geant4 REQUIRED)
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs


include(DD4hep_XML_setup)

#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${CLHEP_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIRS})

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)
if(NOT DD4HEP_USE_XERCESC)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
endif()
add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})
dd4hep_generate_rootmap(DD4hepG4)

#--- install target-------------------------------------

install(DIRECTORY include/DDG4
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepG4
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------