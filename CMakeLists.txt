cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)

PROJECT( DD4hep LANGUAGES NONE)

#################
# Setup version #
#################

SET( DD4hep_VERSION_MAJOR 1 )
SET( DD4hep_VERSION_MINOR 10 )
SET( DD4hep_VERSION_PATCH 0 )

#######################
# Basic project setup #
#######################

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH
       "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."
       FORCE )
 MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

###############################################
# Setup the environment for the documentation #
###############################################

OPTION(BUILD_DOCS_ONLY "Build documentation only" OFF)
OPTION(BUILD_DOCS "Build documentation" ON)

# Add targets for Doxygen code reference and LaTeX User manual

if (BUILD_DOCS)
    ADD_SUBDIRECTORY(doc)
ENDIF()

# If only building docs, stop processing the rest of the CMake file:
IF(BUILD_DOCS_ONLY)
  RETURN()
ENDIF()

#############################################################
# Enable CXX as project language to perform compiler checks #
#############################################################

ENABLE_LANGUAGE(CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard used for compiling")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###############################
# Define DD4hep build options #
###############################

option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC"   OFF)
option(DD4HEP_USE_GEANT4  "Enable the simulation part based on Geant4"    OFF)
option(DD4HEP_IGNORE_GEANT4_TLS "Ignore the tls flag Geant4 was compiled with" OFF)
option(DD4HEP_USE_GEAR    "Build gear wrapper for backward compatibility" OFF)
option(DD4HEP_USE_LCIO    "Build lcio extensions"     OFF)
option(BUILD_TESTING      "Enable and build tests"    ON)
option(CMAKE_MACOSX_RPATH "Build with rpath on macos" ON)

#####################
# Configure version #
#####################

include ( DD4hepBuild )
dd4hep_set_version(DD4hep
        MAJOR ${DD4hep_VERSION_MAJOR}
        MINOR ${DD4hep_VERSION_MINOR}
        PATCH ${DD4hep_VERSION_PATCH} )

configure_file (
        "${PROJECT_SOURCE_DIR}/cmake/Version.h.in"
        "${PROJECT_SOURCE_DIR}/DDCore/include/DD4hep/Version.h"
)

dd4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )

########################
# Resolve dependencies #
########################

# Configure ROOT
find_package ( ROOT 6.08 REQUIRED COMPONENTS Geom GenVector Eve Gui Graf3d RGL EG)
include(${ROOT_USE_FILE})
message ( STATUS "ROOT_VERSION: ${ROOT_VERSION}" )

# Configure BOOST
find_package( Boost 1.49 REQUIRED )
add_definitions( -DBOOST_SPIRIT_USE_PHOENIX_V3 )

# Configure Geant4
if(DD4HEP_USE_GEANT4)
  find_package( Geant4 10.2.2 REQUIRED  gdml ui_all vis_all )
  include( ${Geant4_USE_FILE} )

  IF((NOT ${Geant4_TLS_MODEL} STREQUAL "global-dynamic") AND NOT ${DD4HEP_IGNORE_GEANT4_TLS})
    MESSAGE(FATAL_ERROR "Geant4 was built with ${Geant4_TLS_MODEL}, DD4hep requires 'global-dynamic'! Ignore this ERROR with DD4HEP_IGNORE_GEANT4_TLS=True ")
  ENDIF()

endif()

######################
# Set compiler flags #
######################

dd4hep_set_compiler_flags()

####################
# Include packages #
####################

add_subdirectory ( GaudiPluginService )

include(DD4hep)
include(DD4hepMacros) 

if( DEFINED DD4HEP_WITH_GEANT4 OR DEFINED DD4HEP_WITH_GEAR )
  dd4hep_print_cmake_options( OPTIONAL "Obsolete options: only the following are allowed:" ERROR 1 )
endif()

include(DD4hep_XML_setup)
add_subdirectory(DDParsers)
add_subdirectory(DDCore)
add_subdirectory(DDRec)
add_subdirectory(DDDetectors)
add_subdirectory(DDCond)
add_subdirectory(DDAlign)
add_subdirectory(DDDigi)
if ( DD4HEP_USE_GEANT4 )
  if(Geant4_builtin_clhep_FOUND)
    set(CLHEP "")
  else()
    set(CLHEP CLHEP)
  endif()
  add_subdirectory(DDG4)
endif()
# Note the order: DDEve partially depends on variables from DDG4!
add_subdirectory(DDEve)

dd4hep_enable_tests( DDTest )
add_subdirectory(UtilityApps)

#######################
# Treatment for Apple #
#######################

if( APPLE )
  set( USE_DYLD 1)
  set(CMAKE_MACOSX_RPATH 1)
endif()

#########################
# Configure and install #
#########################
configure_file(cmake/thisdd4hep.csh      bin/thisdd4hep.csh      @ONLY)
configure_file(cmake/thisdd4hep.sh       bin/thisdd4hep.sh       @ONLY)
configure_file(cmake/thisdd4hep_only.csh bin/thisdd4hep_only.csh @ONLY)
configure_file(cmake/thisdd4hep_only.sh  bin/thisdd4hep_only.sh  @ONLY)
configure_file(cmake/run_test.sh         bin/run_test.sh         @ONLY)

install(FILES    ${CMAKE_BINARY_DIR}/bin/thisdd4hep.csh      ${CMAKE_BINARY_DIR}/bin/thisdd4hep.sh DESTINATION bin )
install(FILES    ${CMAKE_BINARY_DIR}/bin/thisdd4hep_only.csh ${CMAKE_BINARY_DIR}/bin/thisdd4hep_only.sh DESTINATION bin )
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/run_test.sh         DESTINATION bin )

install(DIRECTORY cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.cmake" PATTERN "thisdd4hep_package.sh.in" PATTERN "run*.sh"
  PATTERN ".svn" EXCLUDE
  )

###############################
# Dispaly final configuration #
###############################

display_std_variables()

######################################################
# generate and install following configuration files #
######################################################

dd4hep_generate_package_configuration_files( DD4hepConfig.cmake )
if(APPLE)
  SET ( ENV{DD4HEP_LIBRARY_PATH} $ENV{DYLD_LIBRARY_PATH} )
else()
  SET ( ENV{DD4HEP_LIBRARY_PATH} ${LD_LIBRARY_PATH} )
  SET ( DD4HEP_LIBRARY_PATH      ${LD_LIBRARY_PATH} )
endif()

#######################
# Treatment for Apple #
#######################

if(APPLE)
  fill_dd4hep_library_path()
  message(STATUS "DD4HEP_LIBRARY_PATH= $ENV{DD4HEP_LIBRARY_PATH}")
endif()
