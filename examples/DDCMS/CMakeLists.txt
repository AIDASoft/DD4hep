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
# DDCMS is a detector description convention developed by the CMS experiment.
#
#==========================================================================
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
#------------------------------------------------------------------------------
dd4hep_configure_output ()
dd4hep_package (DDCMS MAJOR 0 MINOR 0 PATCH 1
  USES         [ROOT   REQUIRED COMPONENTS Geom GenVector] 
  [DD4hep REQUIRED COMPONENTS DDCore DDCMS]
  OPTIONAL     XERCESC
  INCLUDE_DIRS include )

#------------------------------------------------------------------------------
dd4hep_add_plugin(DDCMSexample SOURCES src/*.cpp)
#---Package installation procedure(s) -----------------------------------------
dd4hep_install_dir( data DESTINATION ${DD4hep_DIR}/examples/DDCMS )
#---Testing--------------------------------------------------------------------
dd4hep_configure_scripts ( DDCMS DEFAULT_SETUP WITH_TESTS )

