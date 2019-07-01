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
# configure Geant4
IF(NOT DD4HEP_USE_GEANT4)
  MESSAGE(STATUS "Not Using geant4, not building DDG4")
  RETURN()
ENDIF()



find_package(Geant4 10.2.2 REQUIRED  gdml ui_all vis_all)
#include( ${Geant4_USE_FILE} ) # do not use the use file, this is not very considerate...

IF((NOT ${Geant4_TLS_MODEL} STREQUAL "global-dynamic") AND NOT ${DD4HEP_IGNORE_GEANT4_TLS})
  MESSAGE(FATAL_ERROR "Geant4 was built with ${Geant4_TLS_MODEL}, DD4hep requires 'global-dynamic'! Ignore this ERROR with DD4HEP_IGNORE_GEANT4_TLS=True ")
ENDIF()

if(Geant4_builtin_clhep_FOUND)
  set(CLHEP "")
else()
  set(CLHEP CLHEP)
endif()

# dd4hep_package(    DDG4
#   USES            [GEANT4 REQUIRED COMPONENTS gdml ui_all vis_all]
#                   [ROOT   REQUIRED COMPONENTS Geom GenVector RIO]
#                    BOOST DDCore
#   OPTIONAL         ${CLHEP} [XercesC QUIET]
#   INCLUDE_DIRS     include
#   INSTALL_INCLUDES include/DDG4 )

dd4hep_print ( "|++> CLHEP_LIBRARIES ${CLHEP_LIBRARIES} ${CLHEP_LIBRARY}" )

#---Add Library---------------------------------------------------------------------
file(GLOB DDG4_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(DDG4 SHARED ${DDG4_SOURCES})
target_link_libraries(DDG4
  PUBLIC
  ${Geant4_LIBRARIES}
  ${Geant4_COMPONENT_LIBRARIES}
  DDCore
  ROOT::Base
  )

#Ensure our own includes come before those of the system
target_include_directories(DDG4 BEFORE
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
  $<TARGET_PROPERTY:DDCore,INTERFACE_INCLUDE_DIRECTORIES>
)

target_include_directories(DDG4 SYSTEM
  PUBLIC
  ${Geant4_INCLUDE_DIRS}
)

#Geant4 CXX Flags are a string with quotes, not a list, so we need to convert to a list...
string(REPLACE " " ";" Geant4_Flags ${Geant4_CXX_FLAGS} ${Geant4_CXX_FLAGS_${CMAKE_BUILD_TYPE}})

# 10.2.2 at least not in 10.5 (check where it switches)
#Geant4_DEFINITIONS already include -D, we jave to get rid of that
SET(G4_DEF_TEMP "")
foreach(def ${Geant4_DEFINITIONS})
  string(REPLACE "-D" "" def ${def})
  LIST(APPEND G4_DEF_TEMP ${def})
endforeach()
SET(Geant4_DEFINITIONS ${G4_DEF_TEMP})
target_compile_options(DDG4
  PUBLIC
  ${Geant4_Flags}
)

target_compile_definitions(DDG4
  PUBLIC
  ${Geant4_DEFINITIONS}
)

#---------------------------  Legacy libraries (for Frank) -------------------------
new_dd4hep_add_plugin(DDG4Legacy  SOURCES legacy/*.cpp
  LINK_LIBRARIES DDG4
  )
#-----------------------------------------------------------------------------------
new_dd4hep_add_dictionary( G__DDG4
  SOURCES python/DDG4Dict.C
  DEPENDS DDCore DDParsers DDG4
  INCLUDES ${Geant4_INCLUDE_DIRS} 
  DEFINITIONS ${Geant4_DEFINITIONS}
  )
#---------------------------  Plugin library for the simulation framework  ---------
new_dd4hep_add_plugin(DDG4Plugins
  SOURCES plugins/*.cpp G__DDG4.cxx
  LINK_LIBRARIES DDG4
  )
#---------------------------  Plugin library for the simulation framework  ---------

FIND_PACKAGE(PythonLibs QUIET)
if(PYTHONLIBS_FOUND)
  MESSAGE(STATUS "Python found, creating DDG4Python Dictionary")
  new_dd4hep_add_dictionary(G__DDG4Python
    SOURCES src/python/DDG4Python.C
    DEPENDS DDCore DDParsers DDG4 ROOT::Base
    INCLUDES ${Geant4_INCLUDE_DIRS}  ${PYTHON_INCLUDE_DIRS}
    DEFINITIONS ${Geant4_DEFINITIONS}
    )
  new_dd4hep_add_dictionary(G__DDPython
    SOURCES tpython/DDPython.C
    INCLUDES ${PYTHON_INCLUDE_DIRS}
    )
  if(ROOT_python_FOUND)
    FIND_PACKAGE(ROOT REQUIRED COMPONENTS PyROOT)
    #---------------------------  Specialized python plugins  --------------------------
    MESSAGE(STATUS "ROOT Has Python, creating DDPython library and plugins")
    add_library(DDPython SHARED G__DDPython.cxx tpython/DDPython.cpp)
    target_link_libraries(DDPython DDG4 ROOT::Base ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES})
    target_include_directories(DDPython PUBLIC ${PYTHON_INCLUDE_DIRS})
    new_dd4hep_add_plugin(DDG4Python 
      SOURCES src/python/*.cpp G__DDG4Python.cxx
      LINK_LIBRARIES DDG4 DDPython
      )
    #---Helper to overcome deficiency of the python executable concerning multi-threading
    add_executable(pyddg4 pyddg4.cpp)
    target_link_libraries(pyddg4 PUBLIC DDPython ROOT::Base ${ROOT_LIBRARIES})
    INSTALL(TARGETS DDPython EXPORT DD4hep DESTINATION lib)
  endif()
else()
  MESSAGE(STATUS "Python not found, not creating DDG4 4Python Dictionaries")
endif()

#---------------------------  Plugin library for the simulation framework  ---------
#---------------------------  LCIO Plugins for new simulation framework  -----------
IF(DD4HEP_USE_LCIO)
  FIND_PACKAGE(LCIO REQUIRED)    
  new_dd4hep_add_plugin(DDG4LCIO
    SOURCES lcio/*.cpp
    LINK_LIBRARIES ${LCIO_LIBRARIES}
    )
ENDIF()
# #---------------------------  DDRec dependent Plugins  -----------------------------
IF(DDRec)
  new_dd4hep_add_plugin(DDG4Reco
    SOURCES reco/*.cpp
    LINK_LIBRARIES DDRec
    )
ENDIF()
#-----------------------------------------------------------------------------------
add_executable(g4gdmlDisplay g4gdmlDisplay.cpp)
#-----------------------------------------------------------------------------------
add_executable(g4FromXML g4FromXML.cpp)
#-----------------------------------------------------------------------------------
target_link_libraries(g4gdmlDisplay DDG4)
target_link_libraries(g4FromXML DDG4)

#---Package installation procedure(s) ----------------------------------------------
dd4hep_install_dir(examples DESTINATION examples/DDG4)
dd4hep_install_files(FILES python/*.py python/*.C DESTINATION python)
install(PROGRAMS python/DDSim/bin/ddsim DESTINATION bin)
dd4hep_install_dir(python/DDSim DESTINATION python)

INSTALL(TARGETS DDG4 EXPORT DD4hep DESTINATION lib)

#Reset ROOT_LIBRARIES
find_package (ROOT 6.08 REQUIRED)