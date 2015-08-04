# $Id: $
#==========================================================================
#  AIDA Detector description implementation for LCD
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
# build all DD4hep examples in one go, e.g.
#
# . ./bin/thisDD4hep.sh
# cd examples ;
# mkdir build ; cd build
# cmake ..
#
# F.Gaede, DESY, 2013
# 
# M.Frank, CERN, 2015:  Adapt to new cmake scripts
#==========================================================================
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
option(BUILD_TESTING "Enable and build tests" ON)
find_package ( ROOT   REQUIRED COMPONENTS Geom GenVector MathCore )
find_package ( DD4hep REQUIRED )
#
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
#
dd4hep_package ( DD4hepExample MAJOR 0 MINOR 0 PATCH 1 
  USES           [ROOT REQUIRED COMPONENTS Geom GenVector MathCore] 
                 [DD4hep REQUIRED COMPONENTS DDCore]
)
#
dd4hep_enable_tests ( AlignDet CLICSiD ClientTests )
###SimpleDetector )
