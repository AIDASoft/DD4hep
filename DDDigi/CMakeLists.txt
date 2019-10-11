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
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDDigi)
file(GLOB DDDigi_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(DDDigi SHARED ${DDDigi_SOURCES})
add_library(DD4hep::DDDigi ALIAS DDDigi)

target_link_libraries(DDDigi PUBLIC
  DD4hep::DDCore Boost::boost ROOT::Core ROOT::Geom ROOT::GenVector ROOT::RIO)

target_include_directories(DDDigi
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

FIND_PACKAGE(TBB QUIET)
if(TBB_FOUND)
  dd4hep_print( "|++> TBB_INCLUDE_DIR -->  ${TBB_INCLUDE_DIR}")
  dd4hep_print( "|++> TBB_LIBRARY     -->  ${TBB_LIBRARY}")
  dd4hep_print( "|++> TBB found. DDDigi will run multi threaded.")
  target_compile_definitions(DDDigi PUBLIC DD4HEP_USE_TBB)
  target_link_libraries(DDDigi ${TBB_LIBRARY})
  target_include_directories(DDDigi ${TBB_INCLUDE_DIRS})
else()
  dd4hep_print( "|++> TBB not found. DDDigi will only work single threaded.")
endif()

#---------------------------  Legacy libraries -------------------------------------
dd4hep_add_dictionary(G__DDDigi
  SOURCES ../DDCore/include/ROOT/Warnings.h python/DDDigiDict.C
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  USES    DD4hep::DDCore ROOT::Core ROOT::Geom ROOT::GenVector ROOT::RIO Boost::boost
  )
#---------------------------  Plugin library for the digitization framework  -------
dd4hep_add_plugin(DDDigiPlugins
  SOURCES        plugins/*.cpp
  GENERATED      G__DDDigi.cxx
  USES           DD4hep::DDDigi
  )

#---Package installation procedure(s) ----------------------------------------------

set_target_properties(DDDigi DDDigiPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

file(GLOB DDigi_python python/*.py python/*.C)
install(FILES ${DDigi_python} DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})
install(TARGETS DDDigi DDDigiPlugins EXPORT DD4hep LIBRARY DESTINATION lib)
install(DIRECTORY include/DDDigi DESTINATION include)

