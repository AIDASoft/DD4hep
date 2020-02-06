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

SET( ENV{DD4hepExamplesINSTALL} ${CMAKE_INSTALL_PREFIX} )

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
ENDIF()

dd4hep_configure_output()

#==========================================================================

SET(DD4HEP_EXAMPLES "AlignDet CLICSiD ClientTests Conditions DDCMS DDCodex DDDB DDDigi DDG4 DDG4_MySensDet LHeD OpticalSurfaces Persistency DDCAD SimpleDetector"
  CACHE STRING "List of DD4hep Examples to build")
SEPARATE_ARGUMENTS(DD4HEP_EXAMPLES)
MESSAGE(STATUS "Will be building these examples: ${DD4HEP_EXAMPLES}")

FOREACH(DDExample IN LISTS DD4HEP_EXAMPLES)
  dd4hep_print("|> Building ${DDExample}")
  add_subdirectory(${DDExample})
ENDFOREACH()
