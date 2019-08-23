#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# Author: F.Gaede (Desy)
#
#==========================================================================
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDRec)

dd4hep_add_dictionary( G__DDRec
  SOURCES ../DDCore/include/ROOT/Warnings.h
  GENERATED src/RecDictionary.h
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  USES DDCore DDParsers
)

file(GLOB DDRec_SOURCES src/*.cpp )

if(DD4HEP_USE_GEAR)
  FIND_PACKAGE(Gear REQUIRED COMPONENTS geartgeo)
  SET(GEAR_SOURCE_FILE src/gear/DDGear.cpp)
  SET_SOURCE_FILES_PROPERTIES(${GEAR_SOURCE_FILE}
    PROPERTIES
    COMPILE_DEFINITIONS DD4HEP_USE_GEAR
    )
endif()

ADD_LIBRARY(DDRec SHARED
  ${DDRec_SOURCES}
  ${GEAR_SOURCE_FILE}
  G__DDRec.cxx
  )

TARGET_INCLUDE_DIRECTORIES(DDRec
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

TARGET_LINK_LIBRARIES(DDRec PUBLIC
  DDCore
  ${GEAR_LIBRARIES}
  ${GEAR_COMPONENT_LIBRARIES}
  )

if(GEAR_FOUND)
  SET(DDREC_PLUGIN_GEAR
    src/gear/createGearForILD.cpp
    src/gear/createGearForCLIC.cpp
    src/gear/createGearForSiD.cpp)
  SET(DDREC_PLUGIN_DEFINITIONS DD4HEP_USE_GEAR)
endif()
  

dd4hep_add_plugin(DDRecPlugins
  SOURCES src/plugins/*.cpp ${DDREC_PLUGIN_GEAR}
  USES DDRec
  DEFINITIONS ${DDREC_PLUGIN_DEFINITIONS}
)

if(DD4HEP_USE_GEAR)
  find_package(Gear REQUIRED COMPONENTS geartgeo)
  add_executable(convertToGear src/convertToGear.cc)
  target_link_libraries(convertToGear ${GEAR_LIBRARIES} ${GEAR_COMPONENT_LIBRARIES} DDRec)
  target_compile_definitions(convertToGear PUBLIC DD4HEP_USE_GEAR)
else()
  find_package(Gear COMPONENTS geartgeo QUIET)
  IF(GEAR_FOUND)
    add_executable(convertToGear src/convertToGear.cc)
    target_link_libraries(convertToGear ${GEAR_LIBRARIES} ${GEAR_COMPONENT_LIBRARIES} DDRec)
    target_compile_definitions(convertToGear PUBLIC DD4HEP_USE_GEAR)
  endif()
endif()

set_target_properties(DDRec DDRecPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

file(GLOB DDRec_python python/*.py)
install(FILES ${DDRec_python} DESTINATION python)
install(DIRECTORY include/DDRec DESTINATION include)
install(TARGETS DDRec DDRecPlugins EXPORT DD4hep LIBRARY DESTINATION lib)
