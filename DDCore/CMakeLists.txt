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
#---Generate ROOT dictionary------------------------------------------------------
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDCore)
dd4hep_print("|++> Will be Building DDCore")

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
  LINKDEF include/ROOT/LinkDef.h
  USES DDParsers ${XML_LIBRARIES}
  )

dd4hep_add_dictionary( G__DD4hepGeo
  SOURCES include/ROOT/Warnings.h src/GeoDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDParsers ${XML_LIBRARIES}
  )

dd4hep_add_dictionary( G__DD4hepProperties
  SOURCES include/ROOT/Warnings.h src/PropertyDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDParsers ${XML_LIBRARIES}
  )

dd4hep_add_dictionary( G__DD4hepSegmentations
  SOURCES include/ROOT/Warnings.h src/SegmentationDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDParsers ${XML_LIBRARIES}
  )

file(GLOB DDCore_SOURCES src/*.cpp src/segmentations/*.cpp src/XML/*.cpp)
if(TARGET Boost::boost)
  dd4hep_print("|++> Boost available, building Json Parsers")
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
  ROOT::Core ROOT::Rint ROOT::Tree ROOT::Physics ROOT::Geom ROOT::GenVector
  ${XML_LIBRARIES}
  )

if(${CMAKE_VERSION} VERSION_GREATER 3.7.99)
  target_compile_features(DDCore
    PUBLIC
    cxx_std_${CMAKE_CXX_STANDARD} # Needs cmake 3.8
    )
endif()

# Generate DDCore plugins---------------------------------------------------------
dd4hep_add_plugin(DDCorePlugins SOURCES src/plugins/*.cpp USES DDCore)

# This plugins depends on the ROOT GDML readers. Hence, extra library
IF(TARGET ROOT::Gdml)
  dd4hep_print("|++> Found Root::GDML: Creating DDGDMLPlugins")
  dd4hep_add_plugin(DDGDMLPlugins
    SOURCES src/gdml/*.cpp
    USES DDCore ROOT::Core ROOT::Gdml
    )
  set_target_properties(DDGDMLPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  install(TARGETS DDGDMLPlugins EXPORT DD4hep LIBRARY DESTINATION lib)
ELSE()
  dd4hep_print("ROOT does not include GDML, not building DDCore GDML Plugins")
ENDIF()
    
#---------------------------  Specialized PYTHON plugin ----------------------------
IF(TARGET ROOT::PyROOT)
  dd4hep_add_plugin(DDPythonPlugins
    SOURCES src/python/*.cpp
    USES DDCore ROOT::Core ROOT::PyROOT
    )
  set_target_properties(DDPythonPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  install(TARGETS DDPythonPlugins EXPORT DD4hep LIBRARY DESTINATION lib)
ELSE()
  dd4hep_print("ROOT does not include PyRoot, not building DDCore DDPython Plugins")
ENDIF()

# install python files
file(GLOB DDCore_Python python/*.py)
install(FILES ${DDCore_Python} DESTINATION python)

# install header files
install(DIRECTORY
  include/DDSegmentation
  include/DD4hep
  include/XML
  include/JSON
  include/Parsers
  include/ROOT
  DESTINATION include
  )

set_target_properties(DDCore DDCorePlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
install(TARGETS DDCore DDCorePlugins EXPORT DD4hep LIBRARY DESTINATION lib)
