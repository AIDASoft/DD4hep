cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
#---------------------------
set( PackageName DDEve )

#---add additional packages here--------------------------------------------------
if(NOT DD4HEP_BUILD_ALL)
find_package( DD4hep ) 
endif()
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#---Includedirs ------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )

#---DD4hepEve library --------------------------------------------------------------
file(GLOB headers include/DDEve/*.h)
file(GLOB sources src/*.cpp)
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories( ${LCIO_INCLUDE_DIRS} )
  list(APPEND sources lcio/LCIOEventHandler.cpp)
endif()

root_generate_dictionary( G__DDEve ${headers} LINKDEF ${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h)
list(APPEND sources G__DDEve.cxx)

add_library(DD4hepEve SHARED ${sources})
target_link_libraries(DD4hepEve DD4hepCore ${ROOT_EVE_LIBRARIES} -lFTGL ${LCIO_LIBRARIES} )
SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")

SET_TARGET_PROPERTIES( DD4hepEve PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#---DD4hepEve rootmap --------------------------------------------------------------
dd4hep_generate_rootmap(DD4hepEve)

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
