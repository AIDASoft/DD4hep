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

#---------------------------------------------------------------------------------------------------
if ( DD4hep_DIR )
  set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DD4hep_DIR}/cmake ) 
endif()
if(CMAKE_INSTALL_PREFIX)
  set ( CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
endif()

#---------------------------------------------------------------------------------------------------
# Main functional include file
if (NOT DD4hepBuild_included)
  include ( DD4hepBuild )
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
  dd4hep_generate_rootmap( ${libraryName} )
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
# Create the .components file needed by the plug-in system.
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap library)

  if(APPLE)
    set(ENV_VAR DYLD_LIBRARY_PATH)
  else()
    set(ENV_VAR LD_LIBRARY_PATH)
  endif()
  SET(ENV{${ENV_VAR}} $<TARGET_FILE_DIR:${library}>:$ENV{${ENV_VAR}}:$ENV{DD4HEP_LIBRARY_PATH} )

  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.components)

  add_custom_command(OUTPUT ${rootmapfile}
                     DEPENDS ${library}
                     COMMAND ${ENV_VAR}=$ENV{${ENV_VAR}} $<TARGET_FILE:DD4hep::listcomponents> -o ${rootmapfile} $<TARGET_FILE:${library}>
                     WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH}
                     )

  add_custom_target(Components_${library} ALL DEPENDS ${rootmapfile})
  SET( install_destination "lib" )
  if( CMAKE_INSTALL_LIBDIR )
    SET( install_destination ${CMAKE_INSTALL_LIBDIR} )
  endif()
  install(FILES $<TARGET_FILE_DIR:${library}>/${rootmapfile}
    DESTINATION ${install_destination}
  )
endfunction()
