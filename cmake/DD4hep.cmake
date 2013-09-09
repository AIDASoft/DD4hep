#---------------------------------------------------------------------------------------------------
# dd4hep_generate_rootmap(library)
#
# Create the .rootmap file needed by the plug-in system.
#---------------------------------------------------------------------------------------------------
function(dd4hep_generate_rootmap library)
  find_package(ROOT QUIET)
  set(rootmapfile ${CMAKE_SHARED_MODULE_PREFIX}${library}.rootmap)

  set(libname ${CMAKE_SHARED_MODULE_PREFIX}${library}${CMAKE_SHARED_LIBRARY_SUFFIX})
  add_custom_command(OUTPUT ${rootmapfile}
                     COMMAND ${CMAKE_COMMAND} -Dlibname=${libname} -Drootmapfile=${rootmapfile}
                             -Dgenmap_install_dir=${LIBRARY_OUTPUT_PATH}
                             -P ${CMAKE_SOURCE_DIR}/cmake/MakeRootMap.cmake
                     DEPENDS ${library})
  add_custom_target(${library}Rootmap ALL DEPENDS ${rootmapfile})

  install(FILES ${LIBRARY_OUTPUT_PATH}/${rootmapfile}
    DESTINATION lib
  )

  # Notify the project level target
  #gaudi_merge_files_append(Rootmap ${library}Rootmap ${CMAKE_CURRENT_BINARY_DIR}/${library}.rootmap)
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
