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
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDG4)

# configure Geant4
IF(NOT DD4HEP_USE_GEANT4)
  dd4hep_print("Not Using geant4, not building DDG4")
  RETURN()
ENDIF()

#---Add Library---------------------------------------------------------------------
file(GLOB DDG4_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(DDG4 SHARED ${DDG4_SOURCES})

target_link_libraries(DDG4
  PUBLIC
  DDCore
  Geant4::Interface
  )

#Ensure our own includes come before those of the system
target_include_directories(DDG4 BEFORE
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  $<TARGET_PROPERTY:DDCore,INTERFACE_INCLUDE_DIRECTORIES>  # FIXME: Check if this works when using imported target
)

#---------------------------  Legacy libraries (for Frank) -------------------------
dd4hep_add_plugin(DDG4Legacy  SOURCES legacy/*.cpp USES DDG4)
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__DDG4
  SOURCES python/DDG4Dict.C
  USES    DDCore DDParsers DDG4 Geant4::Interface
  )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_plugin(DDG4Plugins
  SOURCES   plugins/*.cpp
  GENERATED G__DDG4.cxx
  USES      ROOT::Core ${CLHEP} DDG4 ${XML_LIBRARIES} DDParsers
  )
#---------------------------  Plugin library for the simulation framework  ---------


find_package(PythonInterp 2)
find_package(PythonLibs 2)
if(PYTHONLIBS_FOUND)
  dd4hep_print("|++> Python found, creating DDG4Python Dictionary")
  dd4hep_add_dictionary(G__DDG4Python
    SOURCES src/python/DDG4Python.C
    USES    DDCore DDParsers DDG4 ROOT::Core Geant4::Interface
    )
  dd4hep_add_dictionary(G__DDPython
    SOURCES  tpython/DDPython.C
    INCLUDES ${PYTHON_INCLUDE_DIRS}
    )
  if(TARGET ROOT::PyROOT)
    #---------------------------  Specialized python plugins  --------------------------
    dd4hep_print("|++> ROOT Has Python, creating DDPython library and plugins")
    add_library(DDPython SHARED G__DDPython.cxx tpython/DDPython.cpp)
    target_link_libraries(DDPython DDG4 ROOT::Core ${PYTHON_LIBRARIES} ROOT::PyROOT)
    target_include_directories(DDPython PUBLIC ${PYTHON_INCLUDE_DIRS})
    IF(${CMAKE_CXX_STANDARD} GREATER 16)
      # python header not cxx17 compatible, gives error in clang
      target_compile_options(DDPython PUBLIC -Wno-register)
    ENDIF()
    dd4hep_add_plugin(DDG4Python 
      SOURCES   src/python/*.cpp
      GENERATED G__DDG4Python.cxx
      USES      DDG4 DDPython
      )
    #---Helper to overcome deficiency of the python executable concerning multi-threading
    add_executable(pyddg4 pyddg4.cpp)
    target_link_libraries(pyddg4 PUBLIC DDPython ROOT::Core ROOT::PyROOT)

    # install these libraries
    set_target_properties(DDPython DDG4Python PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
    INSTALL(TARGETS DDPython pyddg4 DDG4Python EXPORT DD4hep
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin)
  endif()
else()
  dd4hep_print("|+++++> Python not found, not creating DDG4 4Python Dictionaries")
endif()

#---------------------------  Plugin library for the simulation framework  ---------
#---------------------------  LCIO Plugins for new simulation framework  -----------
IF(TARGET LCIO::LCIO)
  dd4hep_add_plugin(DDG4LCIO
    SOURCES lcio/*.cpp
    USES    DDG4 LCIO::LCIO
    )
  install(TARGETS DDG4LCIO EXPORT DD4hep LIBRARY DESTINATION lib)
  set_target_properties(DDG4LCIO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

ENDIF()
# #---------------------------  DDRec dependent Plugins  -----------------------------
#This does not compile at the moment
# IF(TARGET DDRec)
#   dd4hep_add_plugin(DDG4Reco
#     SOURCES reco/*.cpp
#     USES DDCore DDG4 DDRec
#     )
#   install(TARGETS DDG4Reco EXPORT DD4hep LIBRARY DESTINATION lib)
# ENDIF()
#-----------------------------------------------------------------------------------
add_executable(g4gdmlDisplay g4gdmlDisplay.cpp)
#-----------------------------------------------------------------------------------
add_executable(g4FromXML g4FromXML.cpp)
#-----------------------------------------------------------------------------------
target_link_libraries(g4gdmlDisplay DDG4)
target_link_libraries(g4FromXML DDG4)

#---Package installation procedure(s) ----------------------------------------------
file(GLOB DDG4_python python/*.py python/*.C)
install(FILES ${DDG4_python} DESTINATION python)

install(PROGRAMS python/DDSim/bin/ddsim DESTINATION bin)
install(DIRECTORY python/DDSim DESTINATION python)

install(DIRECTORY examples DESTINATION examples/DDG4)

set_target_properties(DDG4 DDG4Plugins DDG4Legacy PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})


install(TARGETS DDG4 DDG4Plugins DDG4Legacy g4gdmlDisplay g4FromXML
  EXPORT DD4hep
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY include/DDG4 DESTINATION include)
