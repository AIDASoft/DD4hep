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
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

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

add_subdirectory(CLICSiD)
add_subdirectory(LHeD)
add_subdirectory(AlignDet)
add_subdirectory(ClientTests)
add_subdirectory(Conditions)
add_subdirectory(DDG4)
add_subdirectory(DDDigi)
add_subdirectory(Persistency)
add_subdirectory(SimpleDetector)
add_subdirectory(DDG4_MySensDet)
add_subdirectory(DDCodex)

add_subdirectory(DDDB)
add_subdirectory(DDCMS)

add_subdirectory(OpticalSurfaces)
