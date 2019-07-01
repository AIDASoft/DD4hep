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
SET(PACKAGE DDCore)

#---Generate ROOT dictionary------------------------------------------------------
new_dd4hep_add_dictionary( G__DD4hep
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
  LINKDEF include/ROOT/LinkDef.h
  DEPENDS DDParsers ${XML_LIBRARIES}
  )

new_dd4hep_add_dictionary( G__DD4hepGeo
  SOURCES include/ROOT/Warnings.h src/GeoDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  DEPENDS DDParsers ${XML_LIBRARIES}
  )

new_dd4hep_add_dictionary( G__DD4hepProperties
  SOURCES include/ROOT/Warnings.h src/PropertyDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  DEPENDS DDParsers ${XML_LIBRARIES}
  )

new_dd4hep_add_dictionary( G__DD4hepSegmentations
  SOURCES include/ROOT/Warnings.h src/SegmentationDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  DEPENDS DDParsers ${XML_LIBRARIES}
  )

file(GLOB DDCore_SOURCES src/*.cpp src/segmentations/*.cpp src/XML/*.cpp)
if(Boost_FOUND)
  file(GLOB DDCore_BOOST_SOURCES src/JSON/*.cpp)
endif()

add_library(DDCore SHARED ${DDCore_SOURCES} ${DDCore_BOOST_SOURCES}
  G__DD4hep.cxx
  G__DD4hepSegmentations.cxx
  G__DD4hepProperties.cxx
  G__DD4hepGeo.cxx
  )

target_include_directories(DDCore
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDCore
  PUBLIC
  DD4hepGaudiPluginMgr
  DDParsers
  ROOT::Interface
  ROOT::Geom ROOT::Geom GenVector
  ${XML_LIBRARIES} # XercesC or not
  )

TARGET_COMPILE_FEATURES(DDCore
  PUBLIC cxx_alignof
  #cxx_std_${CMAKE_CXX_STANDARD} # Needs cmake 3.8
  )

#FIXME: install header file folders
# dd4hep_package(    DDCore
#   USES             [ROOT REQUIRED COMPONENTS Geom GenVector]
#                    DDParsers
#   OPTIONAL         XERCESC
#   INCLUDE_DIRS     include
#   INSTALL_INCLUDES include/DDSegmentation
#   include/DD4hep 
#   include/XML    
#   include/JSON   
#   include/Parsers
#   include/ROOT   )

# # Generate DDCore plugins---------------------------------------------------------
new_dd4hep_add_plugin(DDCorePlugins SOURCES src/plugins/*.cpp
  LINK_LIBRARIES DDCore)

# This plugins depends on the ROOT GDML readers. Hence, extra library
IF(${ROOT_gdml_FOUND})
  new_dd4hep_add_plugin(DDGDMLPlugins
  SOURCES src/gdml/*.cpp
  LINK_LIBRARIES DDCore ROOT::Interface
  )
ELSE()
  MESSAGE(WARN "ROOT does not include GDML, not building DDCore GDML Plugins")
ENDIF()
    
#---------------------------  Specialized PYTHON plugin ----------------------------
IF(ROOT_python_FOUND)
  new_dd4hep_add_plugin(DDPythonPlugins
  SOURCES src/python/*.cpp
  LINK_LIBRARIES DDCore ROOT::Interface ROOT::PyROOT
  )
ELSE()
  MESSAGE(WARN "ROOT does not include PyRoot, not building DDCore DDPython Plugins")
ENDIF()

#
#
#---Package installation procedure(s) ----------------------------------------------
dd4hep_install_files(FILES python/*.py DESTINATION python)

INSTALL(TARGETS DDCore EXPORT DD4hep DESTINATION lib)
