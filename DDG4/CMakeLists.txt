#---Find Geant4-------------------------------------------------------------------

find_package(Geant4 REQUIRED gdml ui_all vis_all)
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs

include(DD4hep_XML_setup)

#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${CLHEP_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIRS})

#---Add Library-------------------------------------------------------------------
if(DD4HEP_USE_BOOST)
  list(APPEND include_directories ${BOOST_INCLUDE_DIR})
  file(GLOB sources src/*.cpp parsers/*.cpp)  
  add_definitions(-DDD4HEP_USE_BOOST)
else()
  file(GLOB sources src/*.cpp)
endif()

if(NOT DD4HEP_USE_XERCESC)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
endif()

add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES( DD4hepG4 PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
if(APPLE)
  dd4hep_generate_rootmap_apple(DD4hepG4)
else()
  dd4hep_generate_rootmap(DD4hepG4)
endif()

#-----------------------------------------------------------------------------------
add_executable(g4gdmlDisplay g4gdmlDisplay.cpp)
target_link_libraries(g4gdmlDisplay DD4hepG4 DD4hepCore)
#-----------------------------------------------------------------------------------
add_executable(g4FromXML g4FromXML.cpp)
target_link_libraries(g4FromXML DD4hepG4 DD4hepCore)

#--- install target-------------------------------------
install(DIRECTORY include/DDG4
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepG4 g4gdmlDisplay
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

install(TARGETS DD4hepG4 g4FromXML
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
