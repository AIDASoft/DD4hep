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

# Open Asset Importer Library
option(WITH_SYS_ASSIMP "WITH_SYS_ASSIMP" OFF)

if(${WITH_SYS_ASSIMP} MATCHES "ON")
    find_package(assimp)
else()
    #
    #  Download The Open-Asset-Importer-Lib	 from GITHUB
    #
    include(ExternalProject)
    find_package(Git REQUIRED)
    set(ASSIMP_C_FLAGS "-Wno-incompatible-pointer-types -Wno-attributes")
    set(ASSIMP_CXX_FLAGS "-Wno-deprecated -Wno-shadow -Wno-unused -Wno-address -Wno-nonnull-compare -Wno-sizeof-pointer-memaccess -Wno-attributes -Wno-catch-value -Wno-int-in-bool-context -Wno-misleading-indentation -Wno-address-of-packed-member -Wno-tautological-compare -Wno-class-memaccess -Wno-parentheses")
    ExternalProject_Add(
        assimp_external
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v4.0.1
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_FLAGS=${ASSIMP_CXX_FLAGS} -DCMAKE_C_FLAGS=${ASSIMP_C_FLAGS} -DCMAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT} -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        
    )
    find_package(assimp REQUIRED)

    IF(NOT TARGET assimp OR NOT TARGET DD4hep::assimp)
      add_library(assimp INTERFACE)
      add_library(DD4hep::assimp ALIAS assimp)
      target_compile_definitions(assimp INTERFACE DD4HEP_USE_ASSIMP)
      target_include_directories(assimp SYSTEM INTERFACE ${ASSIMP_INCLUDE_DIRS})
      target_link_libraries(assimp INTERFACE ${ASSIMP_LIBRARIES})
      INSTALL(TARGETS assimp EXPORT DD4hep DESTINATION lib)
    endif()
    MESSAGE(STATUS "ASSIMP_ROOT_DIR:      ${ASSIMP_ROOT_DIR}")
    MESSAGE(STATUS "ASSIMP_INCLUDE_DIRS:  ${ASSIMP_INCLUDE_DIRS}")
    MESSAGE(STATUS "ASSIMP_CXX_FLAGS:     ${ASSIMP_CXX_FLAGS}")
    MESSAGE(STATUS "ASSIMP_LIBRARIES:     ${ASSIMP_LIBRARIES}")
    MESSAGE(STATUS "ASSIMP_LINK_FLAGS:    ${ASSIMP_LINK_FLAGS}")
    MESSAGE(STATUS "ASSIMP_LDFLAGS_OTHER: ${ASSIMP_LDFLAGS_OTHER}")

endif()

dd4hep_add_dictionary(G__DDCAD
  SOURCES ../DDCore/include/ROOT/Warnings.h src/CADDictionary.h
  LINKDEF ../DDCore/include/ROOT/LinkDef.h
  USES    DD4hep::DDCore DD4hep::DDParsers DD4hep::assimp
  )

file(GLOB DDCAD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(DDCAD ${DDCAD_SOURCES} G__DDCAD.cxx)
add_library(DD4hep::DDCAD ALIAS DDCAD)

target_include_directories(DDCAD
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(DDCAD PUBLIC DD4hep::DDCore DD4hep::assimp -L${CMAKE_INSTALL_PREFIX}/lib -lassimp)

dd4hep_add_plugin(DDCADPlugins
  SOURCES src/plugins/*.cpp
  USES    DD4hep::DDCore DD4hep::DDCAD
  )

set_target_properties(DDCAD DDCADPlugins PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDCAD DESTINATION include)
install(TARGETS DDCAD DDCADPlugins EXPORT DD4hep
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
