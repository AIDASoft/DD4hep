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
cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

#-----------------------------------------------------------------------------------
dd4hep_configure_output()
dd4hep_package ( AlignDet MAJOR 0 MINOR 0 PATCH 1
  USES           [ROOT   REQUIRED COMPONENTS Geom GenVector MathCore] 
                 [DD4hep REQUIRED COMPONENTS DDCore]
  OPTIONAL       XERCESC
  INCLUDE_DIRS   include
  )

dd4hep_add_plugin( AlignDet SOURCES src/*.cpp  )
dd4hep_install_dir( compact DESTINATION examples/AlignDet )
dd4hep_configure_scripts( AlignDet DEFAULT_SETUP WITH_TESTS)
