#---Find Geant4-------------------------------------------------------------------
find_package(Geant4 REQUIRED gdml ui_all vis_all)
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs
#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIR}
                    ${CLHEP_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIRS} )

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)

if(NOT DD4HEP_USE_XERCESC)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
endif()
SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
#
add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})
SET_TARGET_PROPERTIES( DD4hepG4 PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#-----------------------------------------------------------------------------------
# No rootmap for link libraries!
#---------------------------  Legacy libraries (for Frank) -------------------------
file(GLOB legacy_sources legacy/*.cpp)
add_library(DD4hepG4Legacy SHARED ${legacy_sources})
target_link_libraries(DD4hepG4Legacy DD4hepCore DD4hepG4 ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DD4hepG4Legacy PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
dd4hep_generate_rootmap(DD4hepG4Legacy)

#---------------------------  Main geant4 interface library (no plugins!)-----------
root_generate_dictionary( G__DDG4 ${CMAKE_CURRENT_SOURCE_DIR}/python/DDG4Dict.C LINKDEF ${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h)
#---------------------------  New Plugin library for new simulation framework  -----
file(GLOB plugins_sources plugins/*.cpp)
list(APPEND plugins_sources G__DDG4.cxx)
add_library(DD4hepG4Plugins SHARED ${plugins_sources})
target_link_libraries(DD4hepG4Plugins DD4hepCore DD4hepG4 ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DD4hepG4Plugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
dd4hep_generate_rootmap(DD4hepG4Plugins)

#---------------------------  LCIO Plugins for new simulation framework  -----------
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories(  ${LCIO_INCLUDE_DIRS} )
  file(GLOB lcio_sources lcio/*.cpp)
  add_library(DD4hepG4LCIO SHARED ${lcio_sources})
  target_link_libraries(DD4hepG4LCIO DD4hepCore DD4hepG4 ${Geant4_LIBRARIES} ${LCIO_LIBRARIES} ${ROOT_LIBRARIES} Reflex EG)
  SET_TARGET_PROPERTIES(DD4hepG4LCIO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  dd4hep_generate_rootmap(DD4hepG4LCIO)
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

install(DIRECTORY examples
  DESTINATION examples/DDG4
  PATTERN ".svn" EXCLUDE )

install(FILES 
  python/DD4hep.py 
  python/DDG4.py 
  python/DDG4Dict.C 
  python/SystemOfUnits.py
  DESTINATION python)

install(TARGETS DD4hepG4 DD4hepG4Plugins DD4hepG4Legacy g4gdmlDisplay
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

install(TARGETS DD4hepG4 g4FromXML
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

if(DD4HEP_USE_LCIO)
install(TARGETS DD4hepG4LCIO 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
endif()

# to do: add corresponding uninstall...
#-------------------------------------------------------
