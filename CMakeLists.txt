cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)
set(CMAKE_MODULE_PATH      ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake) 
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC" OFF)
option(DD4HEP_USE_PYROOT "Enable 'Detector Builders' based on PyROOT" ON)
option(DD4HEP_WITH_GEANT4 "Enable the simulation part based on Geant4" OFF)

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
add_subdirectory(DDExamples/ILDExDet)
add_subdirectory(DDExamples/ILDExReco)
add_subdirectory(DDExamples/ILDExTest)
if(DD4HEP_WITH_GEANT4)
   add_subdirectory(DDExamples/CLICSiDSimu)
   add_subdirectory(DDExamples/ILDExSimu)
endif()

#---Configuration-------------------------------------------------------------------
configure_file(cmake/thisdd4hep.csh thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  thisdd4hep.sh @ONLY)
