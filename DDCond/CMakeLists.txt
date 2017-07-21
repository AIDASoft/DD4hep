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
dd4hep_package(    DDCond
  USES             BOOST DDCore
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDCond)

#---Generate ROOT dictionary-----------------------------------------------------
dd4hep_add_dictionary( G__DDCond
  SOURCES include/ROOT/Warnings.h
  src/ConditionsDictionary.h
  LINKDEF include/ROOT/LinkDef.h )

#---DDCond library --------------------------------------------------------------
dd4hep_add_package_library(DDCond
  SOURCES        src/*.cpp
  GENERATED      G__DDCond.cxx 
  )

#---DDCond components -----------------------------------------------------------
dd4hep_add_plugin(DDCondPlugins  SOURCES  src/plugins/*.cpp src/Type1/*.cpp )
