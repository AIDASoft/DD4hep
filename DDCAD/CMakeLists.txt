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
SET_PROPERTY(DIRECTORY . PROPERTY PACKAGE_NAME DDCAD)
#
if(${ROOT_VERSION} VERSION_LESS 6.21.01)
  return()
endif()
#
# Open Asset Importer Library
#
find_package(assimp QUIET)
if(TARGET assimp::assimp)
    MESSAGE(STATUS "+++> Use existing assimp implementation")    
else()
    MESSAGE(STATUS "+++> Building assimp")    
    #
    #  Download The Open-Asset-Importer-Lib from GITHUB
    #
    include(ExternalProject)
    find_package(Git REQUIRED)
    set(ASSIMP_C_FLAGS   "")
    set(ASSIMP_CXX_FLAGS "-Wno-class-memaccess -Wno-tautological-compare -Wno-sizeof-pointer-memaccess")
    ExternalProject_Add(
        assimp_project
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/assimp
        INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.0.1
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
                   -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
                   -DCMAKE_CXX_FLAGS=${ASSIMP_CXX_FLAGS}
                   -DCMAKE_C_FLAGS=${ASSIMP_C_FLAGS}
                   -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
		   -DBUILD_SHARED_LIBS=1
		   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_COMMAND     ${CMAKE_BUILD_TOOL} install
	INSTALL_COMMAND   ""

    )
    ExternalProject_Get_Property(assimp_project INSTALL_DIR)
    set(ASSIMP_INCLUDE_DIRS "${INSTALL_DIR}/include")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSIMP_INCLUDE_DIRS})

    add_library(assimp SHARED IMPORTED GLOBAL)
    add_dependencies(assimp assimp_project)
    add_library(assimp::assimp ALIAS assimp)
    set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${INSTALL_DIR}/lib/libassimp.so)
    target_include_directories(assimp INTERFACE ${ASSIMP_INCLUDE_DIRS})
endif()
#
MESSAGE(STATUS "Open-Asset-Importer: ASSIMP_INCLUDE_DIRS:  ${ASSIMP_INCLUDE_DIRS}")
MESSAGE(STATUS "Open-Asset-Importer: ASSIMP_LIBRARIES:     ${ASSIMP_LIBRARIES}")
#
#  For the rest: ensure assimp is built before starting to compile.....
#
dd4hep_add_dictionary(G__DDCAD
  SOURCES ../DDCore/include/ROOT/Warnings.h src/CADDictionary.h
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  USES    DD4hep::DDCore DD4hep::DDParsers assimp::assimp
)

file(GLOB DDCAD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(DDCAD ${DDCAD_SOURCES} G__DDCAD.cxx)
add_library(DD4hep::DDCAD ALIAS DDCAD)
# IF(TARGET assimp_project)
#   MESSAGE(STATUS "have to build assimp first")
#   add_dependencies(G__DDCAD assimp_project)
#   add_dependencies(DDCAD assimp_project)
# ENDIF()

if(TARGET assimp::assimp)
MESSAGE(STATUS "have target assimp::assimp")
endif()

target_include_directories(DDCAD
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(DDCAD PUBLIC DD4hep::DDCore assimp::assimp )

dd4hep_add_plugin(DDCADPlugins
  SOURCES src/plugins/*.cpp
  USES    DD4hep::DDCore DD4hep::DDCAD
)

set_target_properties(DDCAD DDCADPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDCAD DESTINATION include)
install(TARGETS DDCAD DDCADPlugins EXPORT DD4hep
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
