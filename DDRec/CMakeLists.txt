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
dd4hep_print("|++> Will build DDRec")
dd4hep_add_dictionary( G__DDRec
  SOURCES ../DDCore/include/ROOT/Warnings.h
  GENERATED src/RecDictionary.h
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  USES DDCore DDParsers
)

file(GLOB DDRec_SOURCES src/*.cpp )

if(DD4HEP_USE_GEAR)
  dd4hep_print("|++++> Adding gear to DDRec")
  FIND_PACKAGE(GEAR REQUIRED COMPONENTS geartgeo)
  SET(GEAR_SOURCE_FILE src/gear/DDGear.cpp)
endif()

ADD_LIBRARY(DDRec SHARED
  ${DDRec_SOURCES}
  ${GEAR_SOURCE_FILE}
  G__DDRec.cxx
  )
ADD_LIBRARY(DD4hep::DDRec ALIAS DDRec)

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
  FOREACH(GEAR_INC_DIR ${GEAR_INCLUDE_DIRS})
    TARGET_INCLUDE_DIRECTORIES(DDRec
      SYSTEM PUBLIC
      $<BUILD_INTERFACE:${GEAR_INC_DIR}>
      )
  ENDFOREACH()
  SET(DDREC_PLUGIN_GEAR
    src/gear/createGearForILD.cpp
    src/gear/createGearForCLIC.cpp
    src/gear/createGearForSiD.cpp)
endif()
  

dd4hep_add_plugin(DDRecPlugins
  SOURCES src/plugins/*.cpp ${DDREC_PLUGIN_GEAR}
  USES DDRec
)

if(DD4HEP_USE_GEAR)
  add_executable(convertToGear src/convertToGear.cc)
  target_link_libraries(convertToGear ${GEAR_LIBRARIES} ${GEAR_COMPONENT_LIBRARIES} DDRec)
endif()

set_target_properties(DDRec DDRecPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

file(GLOB DDRec_python python/*.py)
install(FILES ${DDRec_python} DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})
install(DIRECTORY include/DDRec DESTINATION include)
install(TARGETS DDRec DDRecPlugins EXPORT DD4hep LIBRARY DESTINATION lib)
