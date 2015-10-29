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
dd4hep_package(    DDG4
  USES            [GEANT4 REQUIRED COMPONENTS gdml ui_all vis_all]
                   BOOST DDCore
  OPTIONAL         CLHEP [XercesC QUIET]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDG4 )
#---Add Library---------------------------------------------------------------------
dd4hep_add_package_library(DDG4 SOURCES src/*.cpp )
#---------------------------  Legacy libraries (for Frank) -------------------------
dd4hep_add_plugin(DDG4Legacy    SOURCES legacy/*.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__DDG4  SOURCES python/DDG4Dict.C )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_plugin(DDG4Plugins
  GENERATED   G__DDG4.cxx
  SOURCES     plugins/*.cpp)
#---------------------------  LCIO Plugins for new simulation framework  -----------
dd4hep_add_plugin(DDG4LCIO 
  OPTIONAL [LCIO REQUIRED SOURCES lcio/*.cpp] )
#---------------------------  DDRec dependent Plugins  -----------------------------
dd4hep_add_plugin(DDG4Reco
  OPTIONAL [DDRec REQUIRED SOURCES reco/*.cpp] )
#-----------------------------------------------------------------------------------
dd4hep_add_executable(g4gdmlDisplay SOURCES g4gdmlDisplay.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_executable(g4FromXML     SOURCES g4FromXML.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_executable(dd_sim        SOURCES ddsim.cpp)
#---Package installation procedure(s) ----------------------------------------------
dd4hep_install_dir(examples DESTINATION examples/DDG4)
dd4hep_install_files(FILES python/*.py python/*.C DESTINATION python)
