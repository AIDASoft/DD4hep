cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake) 

#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC" OFF)
option(DD4HEP_USE_PYROOT "Enable 'Detector Builders' based on PyROOT" ON)
option(DD4HEP_WITH_GEANT4 "Enable the simulation part based on Geant4" ON)

find_package(ROOT REQUIRED)
if(DD4HEP_USE_XERCESC)
  find_package(XercesC)
endif()

#---Packages------------------------------------------------------------------------
add_subdirectory(DDCore)
if(DD4HEP_WITH_GEANT4)
  add_subdirectory(DDGeant4)
endif()

#---Examples------------------------------------------------------------------------
add_subdirectory(DDExamples/ILDExDet)
add_subdirectory(DDExamples/CLICSiD)
add_subdirectory(DDExamples/ILDExDisplay)
add_subdirectory(DDExamples/ILDExReco)
if(DD4HEP_WITH_GEANT4)
  add_subdirectory(DDExamples/ILDExSimu)
endif()

#---Configuration-------------------------------------------------------------------
configure_file(cmake/thisdd4hep.csh thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh thisdd4hep.sh @ONLY)




