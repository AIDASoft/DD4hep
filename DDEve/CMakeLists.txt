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
dd4hep_package(    DDEve
  USES             DDCore [ROOT REQUIRED COMPONENTS TEve]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDEve)
#
#---Add Plugin Library-----------------------------------------------------
dd4hep_add_dictionary ( G__DDEve SOURCES include/DDEve/*.h )
#
dd4hep_add_plugin ( DDEve SOURCES src/*.cpp 
  GENERATED   G__DDEve.cxx
  OPTIONAL    [DDG4 INTERNAL SOURCES G__DDG4IO.cxx]
              [LCIO EXTERNAL SOURCES lcio/LCIOEventHandler.cpp] )

#---Package installation procedure(s) -------------------------------------
dd4hep_install_dir ( DDEve DESTINATION examples )
