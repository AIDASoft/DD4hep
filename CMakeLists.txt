cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)

# project version
SET( DD4hep_VERSION_MAJOR 0 )
SET( DD4hep_VERSION_MINOR 5 )
SET( DD4hep_VERSION_PATCH 0 )

SET( DD4hep_VERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )
SET( DD4hep_SOVERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake ) 
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#------------- set the default installation directory to be the source directory

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
    "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..." 
    FORCE )
  MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC" OFF)
option(DD4HEP_USE_PYROOT "Enable 'Detector Builders' based on PyROOT" OFF)  # does not work (compile error)
option(DD4HEP_WITH_GEANT4 "Enable the simulation part based on Geant4" OFF)


#---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "One of: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

#---DD4hep functions and macros -------------------------------------------------------
include(DD4hep)
include( DD4hepMacros ) 

#-----------------------------

find_package(ROOT REQUIRED)
if(DD4HEP_USE_XERCESC)
  find_package(XercesC)
endif()

#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)

  include(CTest)
  enable_testing()
  add_subdirectory(DDTest)

endif(BUILD_TESTING)

#---Documentation-------------------------------------------------------------------

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
IF( INSTALL_DOC )
  INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )
  include(GlobalSVNRevision)
  include(DD4hepDoxygen)
ENDIF()


#---Packages------------------------------------------------------------------------
add_subdirectory(DDSegmentation)
add_subdirectory(DDCore)

if(DD4HEP_WITH_GEANT4)
  add_subdirectory(DDG4)
  add_subdirectory(DDSense)

  #--- create a geant4 variables for the thisdd4hep.sh script
  find_package( Geant4 ) 
  get_filename_component(Geant4_ROOT "${Geant4_INCLUDE_DIR}/../../" ABSOLUTE)

endif()


add_subdirectory(UtilityApps)

#---Configuration-------------------------------------------------------------------
configure_file(cmake/thisdd4hep.csh thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  thisdd4hep.sh @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/thisdd4hep.csh ${CMAKE_BINARY_DIR}/thisdd4hep.sh
  DESTINATION bin
  )

#-----------------------------------------------------------------------------------
display_std_variables()

##############################################################################
# generate and install following configuration files

generate_package_configuration_files( DD4hepConfig.cmake DD4hepConfigVersion.cmake DD4hepLibDeps.cmake )
