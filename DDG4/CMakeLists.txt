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
dd4hep_package(    DDG4
  USES            [GEANT4 REQUIRED COMPONENTS gdml ui_all vis_all]
                  [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
                   BOOST DDCore
  OPTIONAL         ${CLHEP} [XercesC QUIET]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDG4 )
MESSAGE(STATUS "CLHEP_LIBRARIES ${CLHEP_LIBRARIES} ${CLHEP_LIBRARY}")
#---Add Library---------------------------------------------------------------------
dd4hep_add_package_library(DDG4 SOURCES src/*.cpp )
#---------------------------  Legacy libraries (for Frank) -------------------------
dd4hep_add_plugin(DDG4Legacy    SOURCES legacy/*.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__DDG4
  SOURCES ${DDCore_INCLUDE_DIRS}/include/ROOT/Warnings.h python/DDG4Dict.C
  LINKDEF ${DDCore_INCLUDE_DIRS}/include/ROOT/LinkDef.h
  )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_plugin(DDG4Plugins
  GENERATED   G__DDG4.cxx
  SOURCES     plugins/*.cpp
  )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_dictionary(G__DDG4Python
  SOURCES ${DDCore_INCLUDE_DIRS}/include/ROOT/Warnings.h src/python/DDG4Python.C
  LINKDEF ${DDCore_INCLUDE_DIRS}/include/ROOT/LinkDef.h
  OPTIONAL    [PYTHON REQUIRED]
  )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_dictionary(G__DDPython
  SOURCES ${DDCore_INCLUDE_DIRS}/include/ROOT/Warnings.h tpython/DDPython.C
  LINKDEF ${DDCore_INCLUDE_DIRS}/include/ROOT/LinkDef.h
  OPTIONAL    [PYTHON REQUIRED]
  )
#---------------------------  Specialized python plugins  --------------------------
dd4hep_add_regular_library(DDPython
  GENERATED   G__DDPython.cxx
  USES        [ROOT   REQUIRED COMPONENTS PyROOT]
  OPTIONAL    [PYTHON REQUIRED SOURCES tpython/DDPython.cpp])
#---------------------------  Specialized python plugins  --------------------------
dd4hep_add_plugin(DDG4Python
  GENERATED   G__DDG4Python.cxx
  LINK_LIBRARIES DDPython
  OPTIONAL    [PYTHON REQUIRED SOURCES src/python/*.cpp])
#---------------------------  LCIO Plugins for new simulation framework  -----------
dd4hep_add_plugin(DDG4LCIO 
  OPTIONAL [LCIO REQUIRED SOURCES lcio/*.cpp] )
#---------------------------  DDRec dependent Plugins  -----------------------------
#dd4hep_add_plugin(DDG4Reco
#  OPTIONAL [DDRec REQUIRED SOURCES reco/*.cpp] )
#-----------------------------------------------------------------------------------
dd4hep_add_executable(g4gdmlDisplay SOURCES g4gdmlDisplay.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_executable(g4FromXML     SOURCES g4FromXML.cpp)
#-----------------------------------------------------------------------------------
dd4hep_add_executable(dd_sim        SOURCES ddsim.cpp)
#---Helper to overcome deficiency of the python executable concerning multi-threading
dd4hep_add_executable(pyddg4
  LINK_LIBRARIES DDPython
  USES        [ROOT   REQUIRED COMPONENTS PyROOT]
  OPTIONAL    [PYTHON REQUIRED SOURCES pyddg4.cpp])
#---Package installation procedure(s) ----------------------------------------------
dd4hep_install_dir(examples DESTINATION examples/DDG4)
dd4hep_install_files(FILES python/*.py python/*.C DESTINATION python)
