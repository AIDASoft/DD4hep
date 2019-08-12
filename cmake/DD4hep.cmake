#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================

if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12
endif ()

#---------------------------------------------------------------------------------------------------
if ( DD4hep_DIR )
  set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DD4hep_DIR}/cmake ) 
endif()
if(CMAKE_INSTALL_PREFIX)
  set ( CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
endif()

#---------------------------------------------------------------------------------------------------
# Main functional include file
if ( "${DD4hepBuild_included}" STREQUAL "" )
  include ( DD4hepBuild )
  include ( DD4hep_XML_setup )
endif()

##---------------------------------------------------------------------------------------------------
##
##  OLDER STUFF: To be kept for backwards compatibility ....
##
#---------------------------------------------------------------------------------------------------
# add_dd4hep_plugin ( libraryName )
#
# generates the rootmap and installs the library
# all other arguments ( SHARED, ${sources} ) are collected in ${ARGN}
#---------------------------------------------------------------------------------------------------

function( add_dd4hep_plugin libraryName )
  ADD_LIBRARY ( ${libraryName}  ${ARGN} )

  if(APPLE)
    dd4hep_generate_rootmap_apple( ${libraryName} )
  else()
    dd4hep_generate_rootmap( ${libraryName} )
  endif()

  install( TARGETS ${libraryName} LIBRARY DESTINATION lib )
endfunction()

#---------------------------------------------------------------------------------------------------
# dd4hep_instantiate_package
# calls all the function/includes/configurations that are needed to be done to create dd4hep plugins
#---------------------------------------------------------------------------------------------------
function ( dd4hep_instantiate_package PackageName )
  MESSAGE (STATUS "Instantiating the DD4hep package ${PackageName}" )

  IF ( NOT ${DD4hep_FOUND} )
    MESSAGE ( FATAL "DD4HEP was not found" )
  ENDIF()

  INCLUDE( DD4hepMacros )

  #---- configure run environment ---------------
  configure_file( ${DD4hep_DIR}/cmake/thisdd4hep_package.sh.in  this${PackageName}.sh @ONLY)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/this${PackageName}.sh
    DESTINATION bin
    )


endfunction()
#---------------------------------------------------------------------------------------------------
# dd4hep_generate_rootmap(library)
#
# Create the .rootmap file needed by the plug-in system.
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap library)
  if(APPLE)
    dd4hep_generate_rootmap_apple( ${library} )
  else()
    dd4hep_generate_rootmap_notapple( ${library} )
  endif()
endfunction()
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap_notapple library)
  if ( NOT DD4hep_DIR )
    SET ( DD4hep_DIR ${CMAKE_SOURCE_DIR} )
  endif()
  find_package(ROOT QUIET)
  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.components)

  set(libname ${CMAKE_SHARED_MODULE_PREFIX}${library}${CMAKE_SHARED_LIBRARY_SUFFIX})
  #message(STATUS "DD4hep_DIR = ${DD4hep_DIR}" )

  find_package(Boost COMPONENTS filesystem)
  GET_FILENAME_COMPONENT(boost_dir ${Boost_FILESYSTEM_LIBRARY} DIRECTORY)
  message(STATUS " *** listcomponents: boost_dir: ${boost_dir} " )
  add_custom_command(TARGET ${library}
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -Dlibname=${libname} -Drootmapfile=${rootmapfile}
                             -Dgenmap_install_dir=${LIBRARY_OUTPUT_PATH}
                             -DROOT_VERSION=${ROOT_VERSION}
                             -DDD4hep_DIR=${DD4hep_DIR}
                             -DLD_PATH=${boost_dir}
                             -P ${DD4hep_DIR}/cmake/MakeGaudiMap.cmake)

  #add_custom_command(OUTPUT ${rootmapfile}
  #                   COMMAND ${CMAKE_COMMAND} -Dlibname=${libname} -Drootmapfile=${rootmapfile}
  #                           -Dgenmap_install_dir=${LIBRARY_OUTPUT_PATH}
  #                           -DROOT_VERSION=${ROOT_VERSION}
  #                           -DDD4hep_DIR=${DD4hep_DIR}
  #                           -P ${DD4hep_DIR}/cmake/MakeRootMap.cmake
  #                   DEPENDS ${library})
  ##add_custom_target(${library}Rootmap ALL DEPENDS ${rootmapfile})

  SET( install_destination "lib" )
  if( CMAKE_INSTALL_LIBDIR )
    SET( install_destination ${CMAKE_INSTALL_LIBDIR} )
  endif()

  install(FILES ${LIBRARY_OUTPUT_PATH}/${rootmapfile}
    DESTINATION ${install_destination}
  )
endfunction()
#
#
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap_apple library)
  # for now do the same for apple that is done for the rest
  dd4hep_generate_rootmap_notapple( ${library} )
endfunction()
#---------------------------------------------------------------------------------------------------
# dd4hep_install_library(library)
#
# Install library
#---------------------------------------------------------------------------------------------------
function(dd4hep_install_library library)
  set(installfile ${library}.install)

  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.rootmap)

  set(libname ${CMAKE_SHARED_MODULE_PREFIX}${library}${CMAKE_SHARED_MODULE_SUFFIX})

  add_custom_command(OUTPUT ${rootmapfile}
		     SET ( ENV{LD_LIBRARY_PATH} ./:$ENV{LD_LIBRARY_PATH} )
		     COMMAND echo
			     ${library} ${LIBRARY_OUTPUT_DIR}
		     DEPENDS ${library})
  add_custom_target(${library}Install ALL DEPENDS ${installfile})
endfunction()
