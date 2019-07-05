#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================
dd4hep_package(    DDCore
  USES             DDParsers
                   [ROOT REQUIRED COMPONENTS Geom GenVector]
  OPTIONAL         XERCESC
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDSegmentation
  include/DD4hep
  include/XML
  include/JSON
  include/Parsers
  include/ROOT)

#---Generate ROOT dictionary------------------------------------------------------
dd4hep_add_dictionary( G__DD4hep
  SOURCES include/ROOT/Warnings.h
  include/DD4hep/*.h
  include/DD4hep/detail/*.h
  include/XML/*.h
  src/DetectorImp.h
  src/RootDictionary.h
  EXCLUDE include/DD4hep/DetFactoryHelper.h
  include/DD4hep/Any.h
  include/DD4hep/Factories.h
  include/DD4hep/Plugins.h
  include/DD4hep/IoStreams.h
  include/DD4hep/SurfaceInstaller.h
  include/DD4hep/detail/BasicGrammar_inl.h
  include/XML/UnicodeValues.h
  include/XML/tinyxml.h
  include/XML/tinystring.h
  LINKDEF include/ROOT/LinkDef.h )

#---Generate ROOT dictionary------------------------------------------------------
dd4hep_add_dictionary( G__DD4hepGeo
  SOURCES include/ROOT/Warnings.h
  src/GeoDictionary.h
  LINKDEF include/ROOT/LinkDef.h )

#---Generate ROOT dictionary------------------------------------------------------
dd4hep_add_dictionary( G__DD4hepProperties
  SOURCES include/ROOT/Warnings.h
  src/PropertyDictionary.h
  LINKDEF include/ROOT/LinkDef.h )

#---Generate ROOT dictionary------------------------------------------------------
dd4hep_add_dictionary( G__DD4hepSegmentations
  SOURCES include/ROOT/Warnings.h
  src/SegmentationDictionary.h
  LINKDEF include/ROOT/LinkDef.h )

#---Generate DDCore Library-------------------------------------------------------
dd4hep_add_package_library ( DDCore
  SOURCES        src/*.cpp src/segmentations/*.cpp src/XML/*.cpp
  OPTIONAL       [BOOST SOURCES src/JSON/*.cpp]
  GENERATED      G__DD4hep.cxx
  G__DD4hepProperties.cxx
  G__DD4hepSegmentations.cxx
  G__DD4hepGeo.cxx
  INCLUDE_DIRS   ${GaudiPluginService_INCLUDE_DIRS}
  LINK_LIBRARIES DDParsers ${GaudiPluginService_LIBRARIES}
  )

# Generate DDCore plugins---------------------------------------------------------
dd4hep_add_plugin ( DDCorePlugins SOURCES src/plugins/*.cpp )

#---------------------------  Specialized  GDML  plugin ----------------------------
# This plugins depends on the ROOT GDML readers. Hence, extra library
dd4hep_add_plugin(DDGDMLPlugins
  OPTIONAL    [ROOT REQUIRED COMPONENTS Gdml SOURCES src/gdml/*.cpp]
)
#---------------------------  Specialized PYTHON plugin ----------------------------
dd4hep_add_plugin(DDPythonPlugins
  OPTIONAL    [ROOT REQUIRED COMPONENTS PyROOT SOURCES src/python/*.cpp]
)
#
#
#---Package installation procedure(s) ----------------------------------------------
dd4hep_install_files(FILES python/*.py DESTINATION python)
