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
dd4hep_package(    DDDigi
  USES             DDCore
                   [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDDigi )

#---Add Library---------------------------------------------------------------------
dd4hep_add_package_library(DDDigi SOURCES src/*.cpp
  [OPTIONAL TBB REQUIRED]
  [OPTIONAL BOOST REQUIRED]
  )
if( TBB_INCLUDE_DIR )
  dd4hep_print( "|++> TBB_INCLUDE_DIR -->  ${TBB_INCLUDE_DIR}")
  dd4hep_print( "|++> TBB_LIBRARY     -->  ${TBB_LIBRARY}")
  dd4hep_print( "|++> TBB found. DDDigi will run multi threaded.")
  target_compile_definitions(DDDigi PUBLIC DD4HEP_USE_TBB)
  target_link_libraries(DDDigi ${TBB_LIBRARY})
else()
  dd4hep_print( "|++> TBB not found. DDDigi will only work single threaded.")
endif()

#---------------------------  Legacy libraries -------------------------------------
dd4hep_add_dictionary( G__DDDigi
  SOURCES ${DDCore_INCLUDE_DIRS}/include/ROOT/Warnings.h python/DDDigiDict.C
  LINKDEF ${DDCore_INCLUDE_DIRS}/include/ROOT/LinkDef.h
  )
#---------------------------  Plugin library for the digitization framework  -------
dd4hep_add_plugin(DDDigiPlugins
  GENERATED   G__DDDigi.cxx
  SOURCES     plugins/*.cpp
  )
target_compile_definitions(DDDigiPlugins PUBLIC DD4HEP_USE_TBB)
#---Package installation procedure(s) ----------------------------------------------
#d4hep_install_dir(examples DESTINATION examples/DDDigi)
dd4hep_install_files(FILES python/*.py python/*.C DESTINATION python)
