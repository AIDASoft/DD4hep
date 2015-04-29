cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
#---------------------------
set( PackageName DDEve )

#---add additional packages here--------------------------------------------------
if(DD4HEP_BUILD_ALL)
  find_package( DD4hep ) 
endif()
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#---Includedirs ------------------------------------------------------------------
if(DD4HEP_USE_GEANT4)
  include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
    ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
    ${CMAKE_SOURCE_DIR}/DDG4/include	
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )
else()
  include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
    ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )
endif()
#---DD4hepEve library --------------------------------------------------------------
file(GLOB headers include/DDEve/*.h)
file(GLOB sources src/*.cpp)
set( eve_link_libraries DD4hepCore ${ROOT_EVE_LIBRARIES} -lFTGL )
#---------------------------  Support for the LCIO data I/O ------------------------
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories( ${LCIO_INCLUDE_DIRS} )
  list(APPEND sources lcio/LCIOEventHandler.cpp)
  list(APPEND eve_link_libraries ${LCIO_LIBRARIES} )
endif()
#---------------------------  Support for the DDG4 data I/O ------------------------
if(DD4HEP_USE_GEANT4)
  root_generate_dictionary( G__DDG4IO ${CMAKE_CURRENT_SOURCE_DIR}/DDEve/DDG4IO.C LINKDEF ${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h )
  list(APPEND sources G__DDG4IO.cxx)
  list(APPEND eve_link_libraries DD4hepG4 )
endif()
#---------------------------  Add DDEve dictionary for interactive ROOT sessions ---
root_generate_dictionary( G__DDEve ${headers} LINKDEF ${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h)
list(APPEND sources G__DDEve.cxx)

add_library(DD4hepEve SHARED ${sources})
target_link_libraries(DD4hepEve ${eve_link_libraries} )

SET_TARGET_PROPERTIES( DD4hepEve PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#---DD4hepEve rootmap --------------------------------------------------------------
dd4hep_generate_rootmap(DD4hepEve)
#---Package installation procedure(s) ----------------------------------------------
install(DIRECTORY include/DDEve
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(DIRECTORY ${PackageName}
  DESTINATION examples
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepEve
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
