#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
#
# Simple CMakeList.txt file that allows to
# build all dd4hep examples in one go, e.g.
#
# . ./bin/thisdd4hep.sh
# cd examples ;
# mkdir build ; cd build
# cmake ..
#
# F.Gaede, DESY, 2013
# 
# M.Frank, CERN, 2015:  Adapt to new cmake scripts
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif ()

project( DD4hep_Examples )

option(BUILD_TESTING "Enable and build tests" ON)
option(CMAKE_MACOSX_RPATH "Build with rpath on macos" ON)
#
find_package ( DD4hep REQUIRED )
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
#
find_package ( ROOT REQUIRED COMPONENTS Geom GenVector )
include(${ROOT_USE_FILE})
#
dd4hep_set_compiler_flags()
SET( ENV{DD4hepExamplesINSTALL} ${CMAKE_INSTALL_PREFIX} )
#
dd4hep_configure_output()
dd4hep_package ( dd4hepExample MAJOR 0 MINOR 15 PATCH 0 
  USES           [ROOT REQUIRED COMPONENTS Geom GenVector] 
                 [dd4hep REQUIRED COMPONENTS DDCore]
)
#
dd4hep_enable_tests (
  CLICSiD
  LHeD
  AlignDet
  ClientTests
  Conditions
  DDG4
  DDDigi
  Persistency
  SimpleDetector
  DDG4_MySensDet
  DDCodex
  )
if ( "${ROOT_VERSION}" VERSION_GREATER "6.17.00" )
  dd4hep_enable_tests (
    OpticalSurfaces
    )
endif()
#
# XercesC dependent stuff
if ( DD4HEP_USE_XERCESC )
  dd4hep_print("|++> XercesC PRESENT. Building DDDB examples.")
  dd4hep_enable_tests (DDDB)
else()
  dd4hep_print("|++> XercesC is not present. NOT building DDDB examples.")
endif()
#
# CLHEP dependent stuff:
find_package (CLHEP QUIET)
string(FIND "${CLHEP_DIR}" "NOTFOUND" HaveDDCMS)
if ( NOT ${HaveDDCMS} GREATER 0 )
  dd4hep_print("|++> XercesC PRESENT. Building DDCMS examples.")
  dd4hep_enable_tests (DDCMS)
else()
  dd4hep_print("|++> CLHEP is not present. NOT building DDCMS examples.")
endif()
