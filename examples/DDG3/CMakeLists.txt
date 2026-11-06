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
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
dd4hep_use_python_executable()
#==========================================================================

set(DDG3_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/DDG3)
dd4hep_add_plugin(DDG3AlephExample SOURCES src/*.cpp
  USES DD4hep::DDCore DD4hep::DDCond DD4hep::DDAlign
       ROOT::Core ROOT::Geom ROOT::GenVector ROOT::MathCore
  INCLUDES include
  )
dd4hep_install_dir(compact DESTINATION ${DDG3_INSTALL})
dd4hep_install_dir(scripts DESTINATION ${DDG3_INSTALL})
dd4hep_configure_scripts(DDG3 DEFAULT_SETUP WITH_TESTS)
