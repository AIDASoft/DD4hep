cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
PROJECT( DD4hep LANGUAGES NONE)
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DD4hep)

#################
# Setup version #
#################

SET( DD4hep_VERSION_MAJOR 1 )
SET( DD4hep_VERSION_MINOR 14 )
SET( DD4hep_VERSION_PATCH 1 )

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
set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard used for compiling")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(DD4hep_DIR ${CMAKE_SOURCE_DIR} CACHE STRING "DD4hep directory")
SET(DD4hep_ROOT ${CMAKE_INSTALL_PREFIX})

IF(${CMAKE_CXX_STANDARD} LESS 14)
  MESSAGE(FATAL_ERROR "DD4hep requires at least CXX Standard 14 to compile")
ENDIF()

# ----for APPLE scripts have to set the DYLD_LIBRARY_PATH
if(APPLE)
  set(USE_DYLD 1)
  set(CMAKE_MACOSX_RPATH 1)
endif()

###############################
# Define DD4hep build options #
###############################
# NINJA ONLY
# Limit number of parallel DDParser builds to number equal to 2 gigs per core
cmake_host_system_information(RESULT HOST_RAM_MB QUERY TOTAL_PHYSICAL_MEMORY)
math(EXPR DD4HEP_HIGH_MEM_POOL_DEPTH "${HOST_RAM_MB} / 2000" OUTPUT_FORMAT DECIMAL)

option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC"   OFF)
option(DD4HEP_USE_GEANT4  "Enable the simulation part based on Geant4"    OFF)
option(DD4HEP_IGNORE_GEANT4_TLS "Ignore the tls flag Geant4 was compiled with" OFF)
option(DD4HEP_USE_GEAR    "Build gear wrapper for backward compatibility" OFF)
option(DD4HEP_USE_LCIO    "Build lcio extensions"                         OFF)
option(DD4HEP_USE_HEPMC3  "Build hepmc3 extensions"                       OFF)
option(DD4HEP_USE_TBB     "Build features that require TBB"               OFF)
option(DD4HEP_LOAD_ASSIMP "Download and build ASSIMP from github"         OFF)
option(BUILD_TESTING      "Enable and build tests"                        ON)
option(BUILD_SHARED_LIBS  "If OFF build STATIC Libraries"                 ON)
option(DD4HEP_SET_RPATH   "Link libraries with built-in RPATH (run-time search path)" ON)

SET(DD4HEP_BUILD_PACKAGES "DDRec DDDetectors DDCond DDAlign DDCAD DDDigi DDG4 DDEve UtilityApps"
  CACHE STRING "List of DD4hep packages to build")
SEPARATE_ARGUMENTS(DD4HEP_BUILD_PACKAGES)
MESSAGE(STATUS "Will be building these packages: ${DD4HEP_BUILD_PACKAGES}")

OPTION(DD4HEP_BUILD_EXAMPLES "Build all the examples" OFF)
OPTION(DD4HEP_DEBUG_CMAKE "Print debugging information for DD4hep CMAKE" OFF)

SET(DD4HEP_USE_EXISTING_DD4HEP "" CACHE STRING "Build some parts of DD4hep against an existing installation")
SET(DD4HEP_HIGH_MEM_POOL_DEPTH "${DD4HEP_HIGH_MEM_POOL_DEPTH}" CACHE STRING "Number of build slots for high memory compile units (DDParsers), Ninja only")

SET(DD4HEP_BUILD_DEBUG "" CACHE STRING "Enable some DEBUG features in DD4hep. Set to 'ON' or 'OFF' to override default handling")

IF(DD4HEP_BUILD_DEBUG AND NOT DD4HEP_BUILD_DEBUG MATCHES "ON|OFF")
  MESSAGE(FATAL_ERROR "Invalid value for DD4HEP_BUILD_DEBUG, ${DD4HEP_BUILD_DEBUG}, use '', 'ON', 'OFF'")
ENDIF()

# create the job pool
set_property(GLOBAL PROPERTY JOB_POOLS HIGH_MEM_POOL=${DD4HEP_HIGH_MEM_POOL_DEPTH})
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
find_package (ROOT 6.08 REQUIRED)
DD4HEP_SETUP_ROOT_TARGETS()

# Configure BOOST
find_package(Boost 1.49 REQUIRED)
DD4HEP_SETUP_BOOST_TARGETS()

# Configure Geant4
if(DD4HEP_USE_GEANT4)
  find_package( Geant4 10.2.2 REQUIRED  gdml ui_all vis_all )
  IF(NOT Geant4_builtin_clhep_FOUND)
    SET(DD4HEP_USE_CLHEP TRUE)
  ENDIF()
  DD4HEP_SETUP_GEANT4_TARGETS()
  # Geant4 sets the CLHEP include directory to include_directories, we undo this here
  # we don't do this inside DD4hep_SETUP_GEANT4_TARGETS, because that is also used in
  # DD4hepConfig by users of DD4hep
  SET_DIRECTORY_PROPERTIES(PROPERTIES INCLUDE_DIRECTORIES "")
endif()

if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  DD4HEP_SETUP_LCIO_TARGETS()
endif()

IF(DD4HEP_USE_HEPMC3)
  find_package(HepMC3 REQUIRED)
ENDIF()

if(DD4HEP_USE_TBB)
  find_package(TBB REQUIRED)
endif()


######################
# Set compiler flags #
######################

dd4hep_set_compiler_flags()

####################
# Include packages #
####################
include(DD4hep)
include(DD4hepMacros) 

if( DEFINED DD4HEP_WITH_GEANT4 OR DEFINED DD4HEP_WITH_GEAR )
  dd4hep_print_cmake_options( OPTIONAL "Obsolete options: only the following are allowed:" ERROR 1 )
endif()

IF(DD4HEP_USE_EXISTING_DD4HEP)

  MESSAGE(STATUS "|!!> Using pre-existing DD4hep: ${DD4HEP_USE_EXISTING_DD4HEP}")
  SET(CMAKE_PROJECT_NAME DD4hepSelected)
  dd4hep_set_version(${CMAKE_PROJECT_NAME}
    MAJOR ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}
    MINOR ${${CMAKE_PROJECT_NAME}_VERSION_MINOR}
    PATCH ${${CMAKE_PROJECT_NAME}_VERSION_PATCH} )

  FIND_PACKAGE(DD4hep REQUIRED  NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_SYSTEM_ENVIRONMENT_PATH
    PATHS ${DD4HEP_USE_EXISTING_DD4HEP})

  dd4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )

  FOREACH(DDPackage IN LISTS DD4HEP_BUILD_PACKAGES)
    dd4hep_print("|> Building ${DDPackage}")
    add_subdirectory(${DDPackage})
  ENDFOREACH()
  LIST(GET DD4HEP_BUILD_PACKAGES 0 DD4HEP_SELECTED_PACKAGE)
ELSE()

  include(DD4hep_XML_setup)

  #These packages are mandatory
  SET(DD4HEP_SELECTED_PACKAGE DDCore)
  FOREACH(DDPackage GaudiPluginService DDParsers DDCore)
    dd4hep_print("|> Building ${DDPackage}")
    add_subdirectory(${DDPackage})
  ENDFOREACH()
  IF(DD4HEP_BUILD_DEBUG MATCHES "ON" OR (CMAKE_BUILD_TYPE MATCHES "DEBUG|Debug" AND NOT DD4HEP_BUILD_DEBUG MATCHES "OFF"))
    target_compile_definitions(DDCore PUBLIC DD4HEP_DEBUG)
  ENDIF()

  message(STATUS "BUILD Packages: ${DD4HEP_BUILD_PACKAGES}")
  FOREACH(DDPackage IN LISTS DD4HEP_BUILD_PACKAGES)
    dd4hep_print("|> Building ${DDPackage}")
    add_subdirectory(${DDPackage})
  ENDFOREACH()

  if(BUILD_TESTING)
    dd4hep_enable_tests()
    add_subdirectory(DDTest)
  endif()

  if(DD4HEP_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()


ENDIF(DD4HEP_USE_EXISTING_DD4HEP)

#########################
# Configure and install #
#########################
configure_file(cmake/thisdd4hep.sh       bin/thisdd4hep.sh       @ONLY)
configure_file(cmake/thisdd4hep_only.sh  bin/thisdd4hep_only.sh  @ONLY)
configure_file(cmake/run_test.sh         bin/run_test.sh         @ONLY)

install(FILES    ${CMAKE_BINARY_DIR}/bin/thisdd4hep.sh DESTINATION bin )
install(FILES    ${CMAKE_BINARY_DIR}/bin/thisdd4hep_only.sh DESTINATION bin )
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/run_test.sh         DESTINATION bin )

install(DIRECTORY cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.cmake" PATTERN "thisdd4hep_package.sh.in" PATTERN "run*.sh" PATTERN "*.cpp"
  PATTERN ".svn" EXCLUDE
  )

#--- enable CPack --------------------------------------------

option(ENABLE_CPACK "Whether or not to use cpack config" OFF)
if(ENABLE_CPACK)
  include(cmake/DD4hepCPack.cmake)
endif()

###############################
# Dispaly final configuration #
###############################

display_std_variables()

######################################################
# generate and install following configuration files #
######################################################

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/DD4hepConfig.cmake.in
  ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake @ONLY
  )
INSTALL(FILES ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake DESTINATION ./cmake )

dd4hep_generate_package_configuration_files( ${CMAKE_PROJECT_NAME}Config.cmake )

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

INSTALL(EXPORT DD4hep 
  NAMESPACE DD4hep::
  FILE ${CMAKE_PROJECT_NAME}Config-targets.cmake
  DESTINATION cmake
  )
