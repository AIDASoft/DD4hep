cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)

# project version
SET( DD4hep_VERSION_MAJOR 0 )
SET( DD4hep_VERSION_MINOR 2 )
SET( DD4hep_VERSION_PATCH 0 )

SET( DD4hep_VERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )
SET( DD4hep_SOVERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )


set(CMAKE_MODULE_PATH      ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake ) 
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
option(DD4HEP_USE_PYROOT "Enable 'Detector Builders' based on PyROOT" ON)
option(DD4HEP_WITH_GEANT4 "Enable the simulation part based on Geant4" OFF)
option(DD4HEP_WITH_LCIO  "Build ILD examples with LCIO - builds LCIO as external project or uses pre-installed" ON)

find_package(ROOT REQUIRED)
if(DD4HEP_USE_XERCESC)
  find_package(XercesC)
endif()

#---Testing-------------------------------------------------------------------------
include(CTest)
enable_testing()

#---Documentation-------------------------------------------------------------------
include(DD4hepDoxygen)

#---DD4hep internal functions-------------------------------------------------------
include(DD4hep)

#---Packages------------------------------------------------------------------------
add_subdirectory(DDCore)
if(DD4HEP_WITH_GEANT4)
  add_subdirectory(DDG4)
endif()

#---Examples------------------------------------------------------------------------
add_subdirectory(DDExamples/UtilityApps)
add_subdirectory(DDExamples/AlignDet)
add_subdirectory(DDExamples/CLICSiD)


## --- ILD examples ----- 
if( DD4HEP_WITH_LCIO )
add_subdirectory(DDExamples/ILDExDet)
add_subdirectory(DDExamples/ILDExReco)
add_subdirectory(DDExamples/ILDExTest)
endif()


if(DD4HEP_WITH_GEANT4)
   add_subdirectory(DDExamples/CLICSiDSimu)
endif()

if(DD4HEP_WITH_GEANT4 AND DD4HEP_WITH_LCIO)
   add_subdirectory(DDExamples/ILDExSimu)
endif()

#---Configuration-------------------------------------------------------------------
configure_file(cmake/thisdd4hep.csh thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  thisdd4hep.sh @ONLY)


