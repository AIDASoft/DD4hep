#---------------------------------------------------------------------------------------------------
if(DD4HEP_USE_CXX11)
  SET( CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wno-long-long -Wdeprecated -Wformat-security -Wshadow")
  ADD_DEFINITIONs(-DDD4HEP_USE_CXX11)
else()
  SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
endif()

add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3)

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

  install( TARGETS ${libraryName}
    LIBRARY DESTINATION lib
    )

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
  configure_file( ${DD4hep_ROOT}/cmake/thisdd4hep_package.sh.in  this${PackageName}.sh @ONLY)

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

function(dd4hep_generate_rootmap_notapple library)

  if ( NOT DD4hep_ROOT )
    SET ( DD4hep_ROOT ${CMAKE_SOURCE_DIR} )
  endif()

  find_package(ROOT QUIET)
if(DD4HEP_NO_REFLEX)
  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.components)
else()
  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.rootmap)
endif()

  set(libname ${CMAKE_SHARED_MODULE_PREFIX}${library}${CMAKE_SHARED_LIBRARY_SUFFIX})

  add_custom_command(OUTPUT ${rootmapfile}
                     COMMAND ${CMAKE_COMMAND} -Dlibname=${libname} -Drootmapfile=${rootmapfile}
                             -Dgenmap_install_dir=${LIBRARY_OUTPUT_PATH}
                             -P ${DD4hep_ROOT}/cmake/MakeRootMap.cmake
                     DEPENDS ${library})

  add_custom_target(${library}Rootmap ALL DEPENDS ${rootmapfile})

  install(FILES ${LIBRARY_OUTPUT_PATH}/${rootmapfile}
    DESTINATION lib
  )
endfunction()



#
#
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
