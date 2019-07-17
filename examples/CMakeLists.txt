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

include(CTest)
option(BUILD_TESTING "Enable and build tests" ON)

option(CMAKE_MACOSX_RPATH "Build with rpath on macos" ON)
#
find_package ( DD4hep REQUIRED )
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
#
find_package ( ROOT REQUIRED COMPONENTS Geom GenVector )
#include(${ROOT_USE_FILE})
#
dd4hep_set_compiler_flags()
SET( ENV{DD4hepExamplesINSTALL} ${CMAKE_INSTALL_PREFIX} )
#
dd4hep_configure_output()

find_package(ROOT REQUIRED COMPONENTS Geom GenVector)

add_subdirectory(CLICSiD)
add_subdirectory(LHeD)
add_subdirectory(AlignDet)
add_subdirectory(ClientTests)
add_subdirectory(Conditions)
# add_subdirectory(DDG4)
# add_subdirectory(DDDigi)
# #add_subdirectory(OpticalSurfaces) ## needs root version >= 6.18
# add_subdirectory(Persistency)
# add_subdirectory(SimpleDetector)
# add_subdirectory(DDG4_MySensDet)
# add_subdirectory(DDCodex)


# # XercesC dependent stuff
# if(DD4HEP_USE_XERCESC)
#   dd4hep_print("|++> XercesC PRESENT. Building DDDB examples.")
#   add_subdirectory(DDDB)
# else()
#   dd4hep_print("|++> XercesC is not present. NOT building DDDB examples.")
# endif()

# # CLHEP dependent stuff:
# find_package (CLHEP QUIET)
# string(FIND "${CLHEP_DIR}" "NOTFOUND" HaveDDCMS)
# if (TARGET CLHEP::CLHEP)
#   dd4hep_print("|++> CLHEP PRESENT. Building DDCMS examples.")
#   add_subdirectory(DDCMS)
# else()
#   dd4hep_print("|++> CLHEP is not present. NOT building DDCMS examples.")
# endif()
>>>>>>> Modifications in examples
