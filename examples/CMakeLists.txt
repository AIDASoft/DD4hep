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
include(DD4hepMacros) 

IF( "$ENV{DD4hepINSTALL}" STREQUAL "" )
  IF( DD4hep_FOUND )
    MESSAGE( FATAL_ERROR "Must source <DD4hep-install-dir>/bin/thisdd4hep.sh first" )
    EXIT(1)
  ENDIF()
  # When building the examples together with DD4hep (-DDD4HEP_BUILD_EXAMPLES=ON),
  # thisdd4hep.sh hasn't run. Manually set up the required variables for the tests.
  SET( ENV{DD4hepINSTALL} ${CMAKE_INSTALL_PREFIX} )
ENDIF()

dd4hep_set_compiler_flags()
dd4hep_configure_output()


#==========================================================================

SET(DD4HEP_EXAMPLES "AlignDet CLICSiD ClientTests Conditions DDCMS DDCodex DDDigi DDG4 DDG4_MySensDet LHeD OpticalSurfaces Persistency DDCAD SimpleDetector"
  CACHE STRING "List of DD4hep Examples to build")
SEPARATE_ARGUMENTS(DD4HEP_EXAMPLES)

IF(DD4HEP_BUILD_DEBUG)
  SET(DD4HEP_EXAMPLES "${DD4HEP_EXAMPLES} DDDB"
  CACHE STRING "List of DD4hep Examples to build")
ENDIF()

FOREACH(DDExample IN LISTS DD4HEP_EXAMPLES)
  dd4hep_print("|> Building ${DDExample}")
  add_subdirectory(${DDExample})
ENDFOREACH()

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
