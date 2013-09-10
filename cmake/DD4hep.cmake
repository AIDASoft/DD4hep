#---------------------------------------------------------------------------------------------------
# dd4hep_generate_rootmap(library)
#
# Create the .rootmap file needed by the plug-in system.
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap library)

  if ( NOT DD4hep_ROOT )
    SET ( DD4hep_ROOT ${CMAKE_SOURCE_DIR} )
  endif()

  find_package(ROOT QUIET)
  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.rootmap)

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
#FG: the following function works nicely on MacOS - for dd4hep and examples 
#    but not on SL or Ubuntu ...
#
function(dd4hep_generate_rootmap_apple library)
  find_package(ROOT QUIET)
  find_package(DD4hep QUIET)

  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.rootmap)

  set(libname ${CMAKE_SHARED_MODULE_PREFIX}${library}${CMAKE_SHARED_LIBRARY_SUFFIX})


#---------------------------------------------------------------------------------------
if( DD4hep_FOUND )  
  # we are building an external tool and need to source ${DD4hep_ROOT}/bin/thisdd4hep.sh
  
  add_custom_command(OUTPUT ${rootmapfile}
    COMMAND cd ${LIBRARY_OUTPUT_PATH} && 
    . ${DD4hep_ROOT}/bin/thisdd4hep.sh &&
    genmap ${ROOT_genmap_CMD} -i ${libname} -o ${rootmapfile}
    DEPENDS ${library})
  
else()  # we are building DD4hep itself - only need thisroot.sh
  
  add_custom_command(OUTPUT ${rootmapfile}
    COMMAND cd ${LIBRARY_OUTPUT_PATH} && 
    . ${ROOT_ROOT}/bin/thisroot.sh &&
    genmap ${ROOT_genmap_CMD} -i ${libname} -o ${rootmapfile}
    DEPENDS ${library})
endif()


add_custom_target(${library}Rootmap ALL DEPENDS ${rootmapfile})

install(FILES ${LIBRARY_OUTPUT_PATH}/${rootmapfile}
  DESTINATION lib
  )
#--------------------------------------------------------------------------------------
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
