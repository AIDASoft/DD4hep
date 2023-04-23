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
  dd4hep_print("|++> Not Using geant4, not building DDG4")
  RETURN()
ENDIF()

#---Add Library---------------------------------------------------------------------
file(GLOB DDG4_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(DDG4 ${DDG4_SOURCES})
add_library(DD4hep::DDG4 ALIAS DDG4)

target_link_libraries(DDG4
  PUBLIC
  DD4hep::DDCore
  Geant4::Interface
  )
#
#Ensure our own includes come before those of the system
target_include_directories(DDG4 BEFORE
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
)

if(NOT Geant4_gdml_FOUND)
  dd4hep_print("|++> Geant4 has no GDML library present....do not build corresponding features")
  target_compile_definitions(DDG4 PUBLIC -DGEANT4_NO_GDML)
else()
  dd4hep_print("|++> Geant4 GDML library present....build G4 GDML components")
endif()
#-----------------------------------------------------------------------------------
dd4hep_add_dictionary( G__DDG4
  SOURCES python/DDG4Dict.C
  USES    DD4hep::DDCore DD4hep::DDParsers DD4hep::DDG4 Geant4::Interface
  )
#---------------------------  Plugin library for the simulation framework  ---------
dd4hep_add_plugin(DDG4Plugins
  SOURCES   plugins/*.cpp
  GENERATED G__DDG4.cxx
  USES      DD4hep::DDG4 DD4hep::DDParsers ${XML_LIBRARIES} ROOT::Core ${CLHEP} 
  )
#---------------------------  Plugin library for the simulation framework  ---------

if(TARGET Python::Python AND TARGET ${DD4HEP_ROOT_PYTHON})
  dd4hep_print("|++> Python found, creating DDG4Python Dictionary")
  dd4hep_add_dictionary(G__DDG4Python
    SOURCES src/python/DDG4Python.C
    USES    DD4hep::DDCore DD4hep::DDParsers DD4hep::DDG4 ROOT::Core Geant4::Interface
    )
  dd4hep_add_dictionary(G__DDPython
    SOURCES  tpython/DDPython.C
    USES     DD4hep::DDCore Python::Python
    )
  #---------------------------  Specialized python plugins  --------------------------
  dd4hep_print("|++> ROOT Has Python, creating DDPython library and plugins")
  add_library(DDPython G__DDPython.cxx tpython/DDPython.cpp)
  add_library(DD4hep::DDPython ALIAS DDPython)
  target_link_libraries(DDPython DD4hep::DDG4 ROOT::Core Python::Python ${DD4HEP_ROOT_PYTHON})
  IF(${CMAKE_CXX_STANDARD} GREATER 16)
    # python header not cxx17 compatible, gives error in clang
    target_compile_options(DDPython PUBLIC -Wno-register)
  ENDIF()
  dd4hep_add_plugin(DDG4Python
    SOURCES   src/python/*.cpp
    GENERATED G__DDG4Python.cxx
    USES      DD4hep::DDG4 DD4hep::DDPython
    )
  #---Helper to overcome deficiency of the python executable concerning multi-threading
  add_executable(pyddg4 pyddg4.cpp)
  target_link_libraries(pyddg4 PUBLIC DD4hep::DDPython ROOT::Core ${DD4HEP_ROOT_PYTHON})

  # install these libraries
  set_target_properties(DDPython DDG4Python PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  INSTALL(TARGETS DDPython pyddg4 DDG4Python EXPORT DD4hep
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)
else()
  dd4hep_print("|+++++> Python or PyRoot/ROOTTPython not found, not creating DDG4 Python Dictionaries")
endif()

#---------------------------  Plugin library for the simulation framework  ---------
#---------------------------  LCIO Plugins for new simulation framework  -----------
IF(TARGET LCIO::LCIO)
  dd4hep_add_plugin(DDG4LCIO
    SOURCES lcio/*.cpp
    USES    DD4hep::DDG4 LCIO::LCIO
    )
  install(TARGETS DDG4LCIO EXPORT DD4hep LIBRARY DESTINATION lib)
  set_target_properties(DDG4LCIO PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

ENDIF()

IF(TARGET EDM4HEP::edm4hep)
  dd4hep_add_plugin(DDG4EDM4HEP
    SOURCES edm4hep/*.cpp
    USES    DD4hep::DDG4 EDM4HEP::edm4hep EDM4HEP::edm4hepDict podio::podio podio::podioDict podio::podioRootIO
    )
  install(TARGETS DDG4EDM4HEP EXPORT DD4hep LIBRARY DESTINATION lib)
  set_target_properties(DDG4EDM4HEP PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

ENDIF()

IF(DD4HEP_USE_HEPMC3)
  set(DDG4HepMC3_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hepmc> $<BUILD_INTERFACE:${HEPMC3_INCLUDE_DIR}>)
  set(DDG4HepMC3_USES DD4hep::DDG4 Geant4::Interface ${HEPMC3_LIBRARIES})
  set(DDG4HepMC3_DEFINITIONS)

  IF(ZLIB_FOUND)
    list(APPEND DDG4HepMC3_INCLUDES ${ZLIB_INCLUDE_DIR})
    list(APPEND DDG4HepMC3_USES ZLIB::ZLIB)
    list(APPEND DDG4HepMC3_DEFINITIONS HEPMC3_USE_COMPRESSION HEPMC3_Z_SUPPORT=1)
  ENDIF()
  IF(LIBLZMA_FOUND)
    list(APPEND DDG4HepMC3_INCLUDES ${LIBLZMA_INCLUDE_DIRS})
    list(APPEND DDG4HepMC3_USES ${LIBLZMA_LIBRARIES})
    list(APPEND DDG4HepMC3_DEFINITIONS HEPMC3_USE_COMPRESSION HEPMC3_LZMA_SUPPORT=1)
  ENDIF()
  IF(BZIP2_FOUND)
    list(APPEND DDG4HepMC3_INCLUDES ${BZIP2_INCLUDE_DIRS})
    list(APPEND DDG4HepMC3_USES ${BZIP2_LIBRARIES})
    list(APPEND DDG4HepMC3_DEFINITIONS HEPMC3_USE_COMPRESSION HEPMC3_BZ2_SUPPORT=1)
  ENDIF()

  dd4hep_add_plugin(DDG4HepMC3
    SOURCES hepmc/*.cpp
    USES ${DDG4HepMC3_USES}
    INCLUDES ${DDG4HepMC3_INCLUDES}
    DEFINITIONS ${DDG4HepMC3_DEFINITIONS}
    )
  install(TARGETS DDG4HepMC3 EXPORT DD4hep LIBRARY DESTINATION lib)
  set_target_properties(DDG4HepMC3 PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
  SET(DD4HEP_USE_HEPMC3 ON)
ENDIF()

#
#---------------------------  DDRec dependent Plugins  -----------------------------
#This does not compile at the moment
# IF(TARGET DDRec)
#   dd4hep_add_plugin(DDG4Reco
#     SOURCES reco/*.cpp
#     USES DDCore DDG4 DDRec
#     )
#   install(TARGETS DDG4Reco EXPORT DD4hep LIBRARY DESTINATION lib)
# ENDIF()
#-----------------------------------------------------------------------------------
if(Geant4_gdml_FOUND)
  add_executable(g4gdmlDisplay g4gdmlDisplay.cpp)
  target_link_libraries(g4gdmlDisplay DDG4)
  install(TARGETS g4gdmlDisplay EXPORT DD4hep RUNTIME DESTINATION bin)
endif()
#
#-----------------------------------------------------------------------------------
add_executable(g4FromXML g4FromXML.cpp)
target_link_libraries(g4FromXML DDG4)
#
#---Package installation procedure(s) ----------------------------------------------
file(GLOB DDG4_python python/*.py python/*.C)
install(FILES ${DDG4_python} DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})

get_filename_component(PYTHON_INTERPRETER_BINARY ${Python_EXECUTABLE} NAME)
configure_file(python/DDSim/bin/ddsim.in.py ${CMAKE_BINARY_DIR}/bin/ddsim @ONLY)
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/ddsim DESTINATION bin)

install(PROGRAMS python/bin/checkOverlaps.py DESTINATION bin RENAME checkOverlaps)
install(PROGRAMS python/bin/checkGeometry.py DESTINATION bin RENAME checkGeometry)
install(PROGRAMS python/bin/g4MaterialScan.py DESTINATION bin RENAME g4MaterialScan)
install(PROGRAMS python/bin/g4GeometryScan.py DESTINATION bin RENAME g4GeometryScan)

# configure and install DD4hepSimulation files
file(GLOB_RECURSE DDSIM_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR}/python/DDSim/ python/DDSim/*.py)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/python/DDSim/Helper)
FOREACH(ddsimFile ${DDSIM_FILES})
  dd4hep_debug("Configuring ${ddsimFile}")
  # we pass all DDSim files through configure, because it is easier then only configuring only one of them, because we
  # need all of them to end up in the same directory to install them from there
  configure_file(python/DDSim/${ddsimFile} ${CMAKE_BINARY_DIR}/python/DDSim/${ddsimFile} @ONLY)
ENDFOREACH()

install(DIRECTORY ${CMAKE_BINARY_DIR}/python/DDSim DESTINATION ${DD4HEP_PYTHON_INSTALL_DIR})

install(DIRECTORY examples DESTINATION examples/DDG4)

set_target_properties(DDG4 DDG4Plugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(TARGETS DDG4 DDG4Plugins g4FromXML
  EXPORT DD4hep
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(DIRECTORY include/DDG4 DESTINATION include)
