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

IF(NOT TARGET DD4hep::DDCore)
  find_package ( DD4hep REQUIRED )
  include ( ${DD4hep_DIR}/cmake/DD4hep.cmake )
  include ( ${DD4hep_DIR}/cmake/DD4hepBuild.cmake )
  dd4hep_configure_output()
ENDIF()

dd4hep_set_compiler_flags()
#==========================================================================
if(NOT TARGET Geant4::Interface)
  dd4hep_print("Not creating DDG4_MySensDet tests")
  return()
endif()

#-----------------------------------------------------------------------------------
set(DDG4_MySensDet_INSTALL ${CMAKE_INSTALL_PREFIX}/examples/DDG4_MySensDet)
install(DIRECTORY scripts DESTINATION ${DDG4_MySensDet_INSTALL})
dd4hep_configure_scripts(DDG4_MySensDet DEFAULT_SETUP WITH_TESTS)
enable_testing ()
include(CTest)
#
#---Geant4 Testing-----------------------------------------------------------------
#
if (DD4HEP_USE_GEANT4)
  #---------------------------  Plugin library for the simulation framework  ---------
  dd4hep_add_dictionary(G__DDG4_MySensDet
    SOURCES ${DD4hep_DIR}/include/ROOT/Warnings.h src/MyTrackerHit.h
    LINKDEF ${DD4hep_DIR}/include/ROOT/LinkDef.h
    OUTPUT  ${LIBRARY_OUTPUT_PATH}
    USES    DD4hep::DDCore DD4hep::DDG4 Geant4::Interface
    )

  #----  Example of a client library with user defined plugins  --------------------
  dd4hep_add_plugin( DDG4_MySensDet
    GENERATED G__DDG4_MySensDet.cxx
    SOURCES   src/*.cpp
    USES      DD4hep::DDCore DD4hep::DDG4 Geant4::Interface ROOT::Core ROOT::Geom ROOT::GenVector ROOT::RIO
    )
  install(TARGETS DDG4_MySensDet DESTINATION lib)
  
  # Geant4 material scan. From position=0,0,0 to end-of-world 
  dd4hep_add_test_reg( DDG4_MySensDet_g4material_scan_SiliconBlock_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4_MySensDet.sh"
    EXEC_ARGS  python ${DD4hep_ROOT}/bin/g4MaterialScan --compact=file:${CMAKE_INSTALL_PREFIX}/examples/ClientTests/compact/SiliconBlock.xml
               "--position=0,0,0" "--direction=0,1,0"
    REGEX_PASS " Terminate Geant4 and delete associated actions."
  )
             
  dd4hep_add_test_reg( DDG4_MySensDet_sim_SiliconBlock_LONGTEST
    COMMAND    "${CMAKE_INSTALL_PREFIX}/bin/run_test_DDG4_MySensDet.sh"
    EXEC_ARGS  python ${DDG4_MySensDet_INSTALL}/scripts/MyTrackerSD_sim.py 
               --compact=file:${CMAKE_INSTALL_PREFIX}/examples/ClientTests/compact/SiliconBlock.xml
    REGEX_PASS NONE
    REGEX_FAIL "Exception;EXCEPTION;ERROR;Error" )

endif()
