#---Find Geant4-------------------------------------------------------------------

find_package(Geant4 REQUIRED ) #gdml ui_all vis_all)
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs


# --- needed ?
# include(DD4hep_XML_setup)

#---Includedirs-------------------------------------------------------------------
include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include
  ${CMAKE_SOURCE_DIR}/DDSegmentation/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  #${CLHEP_INCLUDE_DIR}
  ${Geant4_INCLUDE_DIRS}
  )

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)
#if(NOT DD4HEP_USE_XERCESC)
#  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
#endif()

add_library(DD4hepSense SHARED ${sources})

#target_link_libraries(DDSense ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES( DD4hepSense PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#--------------------------------------------------
# trivial example library
file(GLOB exsrc example/*.cpp)
#if(NOT DD4HEP_USE_XERCESC)
#  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
#endif()
add_library(DD4hepSenseExample SHARED ${exsrc} )
target_link_libraries(DD4hepSenseExample  DD4hepSense ${Geant4_LIBRARIES} ${CMAKE_DL_LIBS} )

#-----------------------------------------------------------------------------------
add_executable(ddsense_example example/ddsense_example.cpp )
target_link_libraries(ddsense_example DD4hepSense ${CMAKE_DL_LIBS} )


#--- install target-------------------------------------
install(DIRECTORY include/DDSense
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

#install(TARGETS DD4hepG4 g4gdmlDisplay
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#  )

# to do: add corresponding uninstall...
#-------------------------------------------------------
