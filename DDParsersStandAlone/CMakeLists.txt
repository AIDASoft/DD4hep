cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set ( CMAKE_MODULE_PATH      ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake ) 
#
include ( DD4hepBuild )
#

#------------- set the default installation directory to be the source directory

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH
       "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."
       FORCE )
 MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#
#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_CXX14   "Build DD4hep using c++14" OFF)
option(CMAKE_MACOSX_RPATH "Build with rpath on macos" ON)
#
#-----------------------------------------------------------------------------------------------------
#
#  project version (Major,minor,patch)
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  !! Use the same version numbers as in DDSegmentation/CMakeLists.txt    !!
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# project version
SET( DD4hep_VERSION_MAJOR 0  )
SET( DD4hep_VERSION_MINOR 24 )
SET( DD4hep_VERSION_PATCH 0  )

dd4hep_set_version(DD4hep
  MAJOR ${DD4hep_VERSION_MAJOR}
  MINOR ${DD4hep_VERSION_MINOR}
  PATCH ${DD4hep_VERSION_PATCH} )

dd4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )
#
# Include ROOT
#
find_package ( ROOT REQUIRED COMPONENTS Geom GenVector)
find_package ( ROOT REQUIRED)
include(${ROOT_USE_FILE})
message ( STATUS "ROOT_VERSION: ${ROOT_VERSION}" )

#---------------------------------------------------------------------------------------------------
dd4hep_set_compiler_flags()
#
include(DD4hep)
include(DD4hepMacros) 
#---Packages------------------------------------------------------------------------
add_subdirectory(DDParsers)
#
#---Configuration-------------------------------------------------------------------
# ----for APPLE scripts have to set the DYLD_LIBRARY_PATH 
if( APPLE ) 
  set( USE_DYLD 1)
  set(CMAKE_MACOSX_RPATH 1)
endif()
#-----------------------------------------------------------------------------------
display_std_variables()

##############################################################################
# generate and install following configuration files
#dd4hep_generate_package_configuration_files( DD4hepConfig.cmake )
if(APPLE)
  SET ( ENV{DD4HEP_LIBRARY_PATH} $ENV{DYLD_LIBRARY_PATH} )
else()
  SET ( ENV{DD4HEP_LIBRARY_PATH} ${LD_LIBRARY_PATH} )
  SET ( DD4HEP_LIBRARY_PATH ${LD_LIBRARY_PATH} )
endif()

if(APPLE)
  fill_dd4hep_library_path()
  message(STATUS "DD4HEP_LIBRARY_PATH= $ENV{DD4HEP_LIBRARY_PATH}")
endif()
