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
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )

dd4hep_set_compiler_flags()
#==========================================================================
#---DDCodex plugin library -------------------------------------------------------
dd4hep_add_plugin(DDCodexPlugins
  SOURCES src/*.cpp
  USES    DD4hep::DDCore ROOT::Core ROOT::Geom ROOT::GenVector
  )
install(TARGETS DDCodexPlugins DESTINATION lib)
#
#
install(DIRECTORY compact python eve DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDCodex )
install(FILES Upgrade.root DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/DDCodex )
#--------------------------------------------------------------------------
#
dd4hep_configure_scripts ( DDCodex DEFAULT_SETUP WITH_TESTS )
