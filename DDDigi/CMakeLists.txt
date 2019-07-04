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
		   TBB
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDDigi )

#---Add Library---------------------------------------------------------------------
dd4hep_add_package_library(DDDigi SOURCES src/*.cpp)
target_compile_definitions(DDDigi PUBLIC DD4HEP_USE_TBB)
message( STATUS "TBB_INCLUDE_DIR : ${TBB_INCLUDE_DIR}")
message( STATUS "TBB_LIBRARY : ${TBB_LIBRARY}")

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
