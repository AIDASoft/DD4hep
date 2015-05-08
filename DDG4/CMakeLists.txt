#---Find Geant4-------------------------------------------------------------------
find_package(Geant4 REQUIRED gdml ui_all vis_all)
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs
#---DD4hep functions and macros -------------------------------------------------------
include( DD4hep )
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
#
add_library(DDG4 SHARED ${sources})
target_link_libraries(DDG4 DDCore ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
SET_TARGET_PROPERTIES( DDG4 PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#-----------------------------------------------------------------------------------
# No rootmap for link libraries!
#---------------------------  Legacy libraries (for Frank) -------------------------
file(GLOB legacy_sources legacy/*.cpp)
add_library(DDG4Legacy SHARED ${legacy_sources})
target_link_libraries(DDG4Legacy DDCore DDG4 ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DDG4Legacy PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
dd4hep_generate_rootmap(DDG4Legacy)

#---------------------------  Main geant4 interface library (no plugins!)-----------
root_generate_dictionary( G__DDG4 ${CMAKE_CURRENT_SOURCE_DIR}/python/DDG4Dict.C LINKDEF ${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h)
#---------------------------  New Plugin library for new simulation framework  -----
file(GLOB plugins_sources plugins/*.cpp)
list(APPEND plugins_sources G__DDG4.cxx)
add_library(DDG4Plugins SHARED ${plugins_sources})
target_link_libraries(DDG4Plugins DDCore DDG4 ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DDG4Plugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
dd4hep_generate_rootmap(DDG4Plugins)

#---------------------------  LCIO Plugins for new simulation framework  -----------
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories(  ${LCIO_INCLUDE_DIRS} )
  file(GLOB lcio_sources lcio/*.cpp)
  add_library(DDG4LCIO SHARED ${lcio_sources})
  target_link_libraries(DDG4LCIO DDCore DDG4 ${Geant4_LIBRARIES} ${LCIO_LIBRARIES} ${ROOT_LIBRARIES} EG)
  SET_TARGET_PROPERTIES(DDG4LCIO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  dd4hep_generate_rootmap(DDG4LCIO)
endif()

#-----------------------------------------------------------------------------------
add_executable(g4gdmlDisplay g4gdmlDisplay.cpp)
target_link_libraries(g4gdmlDisplay DDG4 DDCore)
#-----------------------------------------------------------------------------------
add_executable(g4FromXML g4FromXML.cpp)
target_link_libraries(g4FromXML DDG4 DDCore)
#-----------------------------------------------------------------------------------
add_executable( dd_sim ddsim.cpp)
target_link_libraries(dd_sim DDCore DDG4)

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

install(FILES
  python/checkOverlaps.py
  python/checkGeometry.py
  DESTINATION python)

install(TARGETS DDG4 DDG4Plugins DDG4Legacy g4gdmlDisplay dd_sim
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

install(TARGETS DDG4 g4FromXML
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

if(DD4HEP_USE_LCIO)
install(TARGETS DDG4LCIO
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
endif()

# to do: add corresponding uninstall...
#-------------------------------------------------------
