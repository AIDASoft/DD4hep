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
dd4hep_package(UtilityApps USES DDCore)
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoDisplay      src/display.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoConverter    src/converter.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoPluginRun    src/plugin_runner.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( print_materials src/print_materials.cpp USES DDRec )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( materialScan    src/materialScan.cpp USES DDRec )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( dumpdetector     
  USES DDRec
  OPTIONAL       [GEANT4 REQUIRED SOURCES src/dumpdetector.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_executable(test_surfaces
  USES     DDRec
  OPTIONAL [LCIO REQUIRED SOURCES src/test_surfaces.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__teve SOURCES src/EvNavHandler.h LINKDEF src/LinkDef.h )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( teveLCIO
  USES           [ROOT REQUIRED COMPONENTS TEve] DDRec
  OPTIONAL       [LCIO REQUIRED SOURCES src/teve_display.cpp src/next_event_lcio.cpp G__teve.cxx] )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( teveDisplay 
  USES           [ROOT REQUIRED COMPONENTS TEve] DDRec
  SOURCES        src/teve_display.cpp src/next_event_dummy.cpp
  GENERATED      G__teve.cxx )
