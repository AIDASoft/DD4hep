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

file(GLOB DDCore_SOURCES src/*.cpp src/segmentations/*.cpp src/XML/*.cpp)

if(${CMAKE_CXX_STANDARD} LESS 17)
  list(APPEND EXCLUDE_HEADER include/DD4hep/SpecParRegistry.h)
  list(APPEND EXCLUDE_HEADER include/DD4hep/Filter.h)
  list(FILTER DDCore_SOURCES EXCLUDE REGEX Filter\.cpp|SpecParRegistry\.cpp )
endif()

# Build the generic DD4hep dictionary
dd4hep_add_dictionary( G__DD4hep
  SOURCES include/ROOT/Warnings.h
  include/DD4hep/*.h
  include/DD4hep/detail/*.h
  include/XML/*.h
  src/RootDictionary.h
  EXCLUDE include/DD4hep/DetFactoryHelper.h
  include/DD4hep/Any.h
  include/DD4hep/Factories.h
  include/DD4hep/Plugins.h
  include/DD4hep/IoStreams.h
  include/DD4hep/SurfaceInstaller.h
  include/DD4hep/GrammarParsed.h
  include/DD4hep/GrammarUnparsed.h
  include/XML/UnicodeValues.h
  include/XML/tinyxml.h
  include/XML/tinystring.h
  ${EXCLUDE_HEADER}
  LINKDEF include/ROOT/LinkDef.h
  USES DDCore DDParsers ${XML_LIBRARIES} ${TBB_IMPORTED_TARGETS}
  DEFINITIONS DD4HEP_MUST_USE_DETECTORIMP_H
  )

# Build the DD4hep dictionary for all geometry instantiations
dd4hep_add_dictionary( G__DD4hepGeo
  SOURCES include/ROOT/Warnings.h src/GeoDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDCore DDParsers ${XML_LIBRARIES}
  )

# Build the DD4hep dictionary for property type instantiations
dd4hep_add_dictionary( G__DD4hepProperties
  SOURCES include/ROOT/Warnings.h src/PropertyDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDCore DDParsers ${XML_LIBRARIES}
  )

# Build the DD4hep dictionary for the segmentation instantiations
dd4hep_add_dictionary( G__DD4hepSegmentations
  SOURCES include/ROOT/Warnings.h src/SegmentationDictionary.h
  LINKDEF include/ROOT/LinkDef.h
  USES DDCore DDParsers ${XML_LIBRARIES}
  )

if(TARGET Boost::boost)
  dd4hep_print("|++> Boost available, building Json Parsers")
  file(GLOB DDCore_BOOST_SOURCES src/JSON/*.cpp)
endif()

add_library(DDCore ${DDCore_SOURCES} ${DDCore_BOOST_SOURCES}
  G__DD4hep.cxx
  G__DD4hepSegmentations.cxx
  G__DD4hepProperties.cxx
  G__DD4hepGeo.cxx
  )
add_library(DD4hep::DDCore ALIAS DDCore)

if(DD4HEP_USE_TBB AND ${CMAKE_CXX_STANDARD} GREATER_EQUAL 17)
  target_compile_definitions(DDCore PUBLIC DD4HEP_USE_TBB)
  target_link_libraries(DDCore PUBLIC TBB::tbb)
endif()

target_include_directories(DDCore
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  )

target_link_libraries(DDCore
  PUBLIC
  DD4hep::DDParsers
  ROOT::Core ROOT::Rint ROOT::Tree ROOT::Physics ROOT::Geom ROOT::GenVector
  ${XML_LIBRARIES} ${CMAKE_DL_LIBS}
  )

if(${CMAKE_VERSION} VERSION_GREATER 3.7.99)
  target_compile_features(DDCore
    PUBLIC
    cxx_std_${CMAKE_CXX_STANDARD} # Needs cmake 3.8
    )
endif()

# Generate DDCore plugins---------------------------------------------------------
dd4hep_add_plugin(DDCorePlugins SOURCES src/plugins/*.cpp USES DDCore)

# Graphics DDCore plugins---------------------------------------------------------
dd4hep_add_plugin(DDCoreGraphics
  SOURCES src/graphics/*.cpp
  USES DDCore ROOT::Core ROOT::Rint ROOT::Gui ROOT::Hist ROOT::HistPainter)

# This plugins depends on the ROOT GDML readers. Hence, extra library
IF(TARGET ROOT::Gdml)
  dd4hep_print("|++> Found Root::GDML: Creating DDGDMLPlugins")
  dd4hep_add_plugin(DDGDMLPlugins
    SOURCES src/gdml/*.cpp
    USES DD4hep::DDCore ROOT::Core ROOT::Gdml
    )
  set_target_properties(DDGDMLPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  LIST(APPEND OPTIONAL_TARGETS DDGDMLPlugins)
ELSE()
  dd4hep_print("ROOT does not include GDML, not building DDCore GDML Plugins")
ENDIF()

#---------------------------  Specialized PYTHON plugin ----------------------------
IF(TARGET ROOT::ROOTTPython)
  dd4hep_add_plugin(DDPythonPlugins
    SOURCES src/python/*.cpp
    USES DDCore ROOT::Core ROOT::ROOTTPython
    )
  set_target_properties(DDPythonPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  LIST(APPEND OPTIONAL_TARGETS DDPythonPlugins)
ELSE()
  dd4hep_print("ROOT does not include Python support, not building DDCore DDPython Plugins")
ENDIF()

# install python files
file(GLOB DDCore_Python python/*.py)
install(FILES ${DDCore_Python} DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})

install(PROGRAMS python/bin/checkOverlaps.py DESTINATION bin RENAME checkOverlaps)
install(PROGRAMS python/bin/checkGeometry.py DESTINATION bin RENAME checkGeometry)

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
install(TARGETS DDCore DDCorePlugins ${OPTIONAL_TARGETS} EXPORT DD4hep
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  )
