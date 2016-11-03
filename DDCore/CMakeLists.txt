#=================================================================================
#  $Id: $
#
#  AIDA Detector description implementation for LCD
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
dd4hep_package(    DDCore
  USES             [ROOT REQUIRED COMPONENTS Geom GenVector]
                   DDSegmentation
  OPTIONAL         XERCESC
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DD4hep include/XML)

#---Generate ROOT dictionary------------------------------------------------------
dd4hep_add_dictionary( G__DD4hep
  SOURCES include/DD4hep/*.h 
  include/DD4hep/objects/*.h 
  include/XML/objects/*.h 
  ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/BitField64.h
  ${DDSegmentation_INCLUDE_DIRS}/DDSegmentation/Segmentation.h
  EXCLUDE include/DD4hep/DetFactoryHelper.h
  include/DD4hep/Factories.h
  include/DD4hep/Plugins.h
  include/DD4hep/IoStreams.h
  include/DD4hep/SurfaceInstaller.h
  include/XML/tinyxml.h
  include/XML/tinystring.h
  LINKDEF include/ROOT/LinkDef.h )

dd4hep_add_package_library ( DDParsers
  INCLUDE_DIRS  
  LINK_LIBRARIES
  SOURCES        src/Evaluator/*.cpp 
  OPTIONAL       [BOOST SOURCES src/parsers/*.cpp]
  )

#---Generate DDCore Library-------------------------------------------------------
dd4hep_add_package_library ( DDCore
  SOURCES        src/*.cpp src/XML/*.cpp
  GENERATED      G__DD4hep.cxx 
  INCLUDE_DIRS   ${GaudiPluginService_INCLUDE_DIRS}
  LINK_LIBRARIES DDParsers ${GaudiPluginService_LIBRARIES}
  )

# Generate DDCore plugins---------------------------------------------------------
dd4hep_add_plugin ( DDCorePlugins  SOURCES src/plugins/*.cpp )
