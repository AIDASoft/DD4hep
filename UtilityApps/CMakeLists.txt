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
dd4hep_package(UtilityApps
  USES DDCore
      [ROOT REQUIRED COMPONENTS Geom]
  )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoDisplay      src/display.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoConverter    src/converter.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( geoPluginRun    src/plugin_runner.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( dumpBfield      src/dumpBfield.cpp )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( print_materials src/print_materials.cpp USES DDRec )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( materialScan    src/materialScan.cpp USES DDRec )
#-----------------------------------------------------------------------------------
dd4hep_add_executable( graphicalMaterialScan src/graphicalMaterialScan.cpp USES DDRec ROOT )
#-----------------------------------------------------------------------------------
#dd4hep_add_executable( pydd4hep     
#  USES        [ROOT   REQUIRED COMPONENTS PyROOT]
#  OPTIONAL    [PYTHON REQUIRED SOURCES src/DD4hep_python.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_executable( dumpdetector     
  USES DDRec
  OPTIONAL       [GEANT4 REQUIRED SOURCES src/dumpdetector.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_executable(test_surfaces
  USES     DDRec DDTest
  OPTIONAL [LCIO REQUIRED SOURCES src/test_surfaces.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_executable(test_cellid_position_converter
  USES     DDRec DDTest
  OPTIONAL [LCIO REQUIRED SOURCES src/test_cellid_position_converter.cpp])
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__eve
  SOURCES src/EvNavHandler.h
  LINKDEF src/LinkDef.h )

#-----------------------------------------------------------------------------------
if ( ${DD4HEP_USE_LCIO} )
  dd4hep_add_dictionary( G__eve1
    SOURCES src/EvNavHandler.h
    LINKDEF src/LinkDef.h )
  dd4hep_add_executable( teveLCIO
    USES           [ROOT REQUIRED COMPONENTS Eve Gui Graf3d RGL]
                   [LCIO REQUIRED]
                   DDRec
    GENERATED      G__eve1.cxx
    SOURCES        src/teve_display.cpp src/next_event_lcio.cpp
 )
endif()

#-----------------------------------------------------------------------------------
dd4hep_add_executable( teveDisplay
  USES           [ROOT REQUIRED COMPONENTS Eve Gui Graf3d RGL] DDRec
  GENERATED      G__eve.cxx
  SOURCES        src/teve_display.cpp src/next_event_dummy.cpp
)
