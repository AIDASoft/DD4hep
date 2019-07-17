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

add_library(DDEve_Interface INTERFACE)
target_include_directories(DDEve_Interface
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(DDEve_Interface INTERFACE ROOT::Core ROOT::Rint ROOT::Eve ROOT::Gui ROOT::Graf3d ROOT::RGL ROOT::EG)

new_dd4hep_add_dictionary(G__DDEve
  SOURCES ../DDCore/include/ROOT/Warnings.h include/DDEve/*.h
  EXCLUDE include/DDEve/Utilities.h
  include/DDEve/ParticleActors.h
  include/DDEve/HitActors.h
  include/DDEve/Factories.h
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  DEPENDS DDCore
  )

if(TARGET LCIO::LCIO)
  SET(DDEVE_LCIO_SOURCES lcio/LCIOEventHandler.cpp)
  SET(DDEVE_LCIO_LINK LCIO::LCIO)
endif()

new_dd4hep_add_plugin(DDEvePlugins SOURCES src/*.cpp ${DDEVE_LCIO_SOURCES}
  GENERATED  G__DDEve.cxx
  LINK_LIBRARIES DDCore DDEve_Interface ${DDEVE_LCIO_LINK}
  )

if(DD4HEP_USE_GEANT4)
  new_dd4hep_add_dictionary(G__DDG4IO
    SOURCES ../DDCore/include/ROOT/Warnings.h DDEve/DDG4IO.C
    LINKDEF ../DDCore/include/ROOT/LinkDef.h
    DEPENDS DDG4 
    )
  add_library(DDG4IO SHARED DDEve/IO.cpp G__DDG4IO.cxx)
  target_link_libraries(DDG4IO DDG4 DDEve_Interface)
  set_target_properties(DDG4IO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  install(TARGETS DDG4IO EXPORT DD4hep LIBRARY DESTINATION lib)
endif()

add_executable(ddeve DDEve/DDEve.cpp)
target_link_libraries(ddeve DDEve_Interface)


set_target_properties(DDEvePlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})


install(DIRECTORY DDEve DESTINATION examples )
install(DIRECTORY include/DDEve DESTINATION include)
install(TARGETS ddeve DDEvePlugins DDEve_Interface EXPORT DD4hep
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )