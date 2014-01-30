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
                    ${Geant4_INCLUDE_DIRS} ) #${LCIO_DIR}/include )

#---Add Library-------------------------------------------------------------------
if(DD4HEP_USE_BOOST)
 #fg:  canonical way of finding Boost w/ cmake uses 
 #     -D Boost_DIR=_path_to_boost
 #     and then FindBoost.cmake sets the variable Boost_INCLUDE_DIR
  FIND_PACKAGE( Boost REQUIRED) 
  include_directories( ${Boost_INCLUDE_DIR})
  #list(APPEND include_directories ${BOOST_INCLUDE_DIR})
  file(GLOB sources src/*.cpp parsers/*.cpp)  
  add_definitions(-DDD4HEP_USE_BOOST)
#  MESSAGE( STATUS  "Boost_INCLUDE_DIR = ${Boost_INCLUDE_DIR} ")
#  MESSAGE( STATUS  "include_directories= ${include_directories} ")
else()
  file(GLOB sources src/*.cpp)
endif()

if(NOT DD4HEP_USE_XERCESC)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/Geant4XML.cpp)
endif()
SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")

#---------------------------  Main geant4 interface library (no plugins!)-----------
add_library(DD4hepG4 SHARED ${sources})
target_link_libraries(DD4hepG4 DD4hepCore ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})
SET_TARGET_PROPERTIES( DD4hepG4 PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
# No rootmap for link libraries!

#---------------------------  Legacy libraries (for Frank) -------------------------
file(GLOB legacy_sources legacy/*.cpp)
add_library(DD4hepG4Legacy SHARED ${legacy_sources})
target_link_libraries(DD4hepG4Legacy DD4hepCore DD4hepG4 ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DD4hepG4Legacy PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
if(APPLE)
  dd4hep_generate_rootmap_apple(DD4hepG4Legacy)
else()
  dd4hep_generate_rootmap(DD4hepG4Legacy)
endif()

#---------------------------  New Plugin library for new simulation framework  -----
file(GLOB plugins_sources plugins/*.cpp)
add_library(DD4hepG4Plugins SHARED ${plugins_sources})
target_link_libraries(DD4hepG4Plugins DD4hepCore DD4hepG4 ${ROOT_LIBRARIES} Reflex ${Geant4_LIBRARIES})

SET_TARGET_PROPERTIES(DD4hepG4Plugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
if(APPLE)
  dd4hep_generate_rootmap_apple(DD4hepG4Plugins)
else()
  dd4hep_generate_rootmap(DD4hepG4Plugins)
endif()

#---------------------------  LCIO Plugins for new simulation framework  -----------
#if(LCIO_DIR)
#  list(APPEND include_directories ${LCIO_DIR}/include)

if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories(  ${LCIO_INCLUDE_DIRS} )

  file(GLOB lcio_sources lcio/*.cpp)
  add_library(DD4hepG4LCIO SHARED ${lcio_sources})

  target_link_libraries(DD4hepG4LCIO DD4hepCore DD4hepG4 ${Geant4_LIBRARIES} ${LCIO_LIBRARIES} ${ROOT_LIBRARIES} Reflex)

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
