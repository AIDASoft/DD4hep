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
file(GLOB DDDigi_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/noise/*.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/src/segmentations/*.cpp)
add_library(DDDigi ${DDDigi_SOURCES})
add_library(DD4hep::DDDigi ALIAS DDDigi)

target_link_libraries(DDDigi PUBLIC
  DD4hep::DDCore Boost::boost ROOT::Core ROOT::Geom ROOT::GenVector ROOT::RIO)

target_include_directories(DDDigi
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
#
if(DD4HEP_USE_TBB)
  dd4hep_print( "|++> TBB_IMPORTED_TARGETS -->  ${TBB_IMPORTED_TARGETS}")
  dd4hep_print( "|++> TBB found. DDDigi will run multi threaded.")
  target_compile_definitions(DDDigi PUBLIC DD4HEP_USE_TBB)
  target_link_libraries(DDDigi PUBLIC ${TBB_IMPORTED_TARGETS})
else()
  dd4hep_print( "|++> TBB not used. DDDigi will only work single threaded.")
endif()
#
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
#
#----------------  I/O Plugin library to read/write DDG4 and edm4hep files ---------
set(DDDigiIO_USES        "DD4hep::DDDigi;DD4hep::DDCore")
set(DDDigiIO_SOURCES     "io/DigiIO.cpp")
set(DDDigiIO_GENERATED)
set(DDDigiIO_DEFINITIONS "DD4HEP_BUILD_DDDIGI_IO=1")
if(DD4HEP_USE_GEANT4)
  dd4hep_add_dictionary(G__DDDigi_DDG4_IO
    SOURCES ../DDCore/include/ROOT/Warnings.h io/DDG4IO.cpp
    LINKDEF ../DDCore/include/ROOT/LinkDef.h
    USES    DD4hep::DDG4 DD4hep::DDCore
  )
  list(APPEND DDDigiIO_DEFINITIONS DD4HEP_USE_DDG4=1)
  list(APPEND DDDigiIO_GENERATED   G__DDDigi_DDG4_IO.cxx)
  list(APPEND DDDigiIO_SOURCES     "io/DDG4IO.cpp;io/DigiDDG4Input.cpp")
  list(APPEND DDDigiIO_USES        DD4hep::DDG4)
else()
  dd4hep_print( "|++> Geant4 not used. DDDigi will not be able to read DDG4 output.")
endif()
#
if(DD4HEP_USE_EDM4HEP)
  list(APPEND DDDigiIO_SOURCES      io/Digi2edm4hep.cpp)
  list(APPEND DDDigiIO_DEFINITIONS  DD4HEP_USE_EDM4HEP=1)
  list(APPEND DDDigiIO_USES         "EDM4HEP::edm4hep;EDM4HEP::edm4hepDict;podio::podio;podio::podioDict;podio::podioRootIO")
else()
  dd4hep_print( "|++> EDM4HEP not used. DDDigi will not be able to write EDM4HEP output.")
endif()
#
list(REMOVE_DUPLICATES DDDigiIO_USES)
dd4hep_add_plugin(DDDigi_IO
    SOURCES        ${DDDigiIO_SOURCES}
    GENERATED      ${DDDigiIO_GENERATED}
    USES           ${DDDigiIO_USES}
  )
  target_compile_definitions(DDDigi_IO PRIVATE ${DDDigiIO_DEFINITIONS})
  set_target_properties(DDDigi_IO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  install(TARGETS DDDigi_IO EXPORT DD4hep ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
#
#---Package installation procedure(s) ----------------------------------------------
set_target_properties(DDDigi DDDigiPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#
file(GLOB DDDigi_python python/*.py python/*.C)
install(FILES ${DDDigi_python} DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})
#
install(TARGETS DDDigi DDDigiPlugins EXPORT DD4hep
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
install(DIRECTORY include/DDDigi DESTINATION include)
