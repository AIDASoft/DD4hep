cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

#---------------------------
set( PackageName DDSurfaces )
#---------------------------

project(${PackageName})

# project version
SET( DDSurfaces_VERSION_MAJOR 0 )
SET( DDSurfaces_VERSION_MINOR 1 )
SET( DDSurfaces_VERSION_PATCH 0 )

SET( DDSurfaces_VERSION "${DDSurfaces_VERSION_MAJOR}.${DDSurfaces_VERSION_MINOR}" )
SET( DDSurfaces_SOVERSION "${DDSurfaces_VERSION_MAJOR}.${DDSurfaces_VERSION_MINOR}" )

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake ) 
#set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
    "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..." 
    FORCE )
  MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#file(GLOB sources src/*.cpp)

#add_library(${PackageName} SHARED ${sources} )

#SET_TARGET_PROPERTIES( ${PackageName} PROPERTIES VERSION ${DDSurfaces_VERSION} SOVERSION ${DDSurfaces_SOVERSION})

#--- install target-------------------------------------
install(DIRECTORY include/${PackageName}
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

#install(TARGETS ${PackageName}
#  RUNTIME DESTINATION bin
#  LIBRARY DESTINATION lib
#)

# to do: add corresponding uninstall...
#-------------------------------------------------------
