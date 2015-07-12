# $Id$
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================

cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
#---------------------------
set( PackageName DDAlign )

#---add additional packages here--------------------------------------------------
find_package( DD4hep ) 
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#---Includedirs ------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )

#---DDAlign library --------------------------------------------------------------
file(GLOB sources src/*.cpp)
add_library(DDAlign SHARED ${sources})
target_link_libraries(DDAlign DDCore ${ROOT_LIBRARIES})
#
#
if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
else()
  add_definitions(-DDD4HEP_USE_TINYXML)
endif()
SET_TARGET_PROPERTIES( DDAlign PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#
#---DDAlign rootmap --------------------------------------------------------------
dd4hep_generate_rootmap(DDAlign)
#
install(DIRECTORY include/DDAlign
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DDAlign
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
