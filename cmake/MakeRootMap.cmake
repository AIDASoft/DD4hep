message(STATUS " *** ROOT_genmap[Root version: ${ROOT_VERSION}]: Generate map for ${libname} ..." )
  #
  #  Running listcomponents from Gaudi
  #   
  FIND_PROGRAM( ROOT_genmap_CMD listcomponents PATHS ${genmap_install_dir}/../bin ${DD4hep_DIR}/bin )
  ###MESSAGE( STATUS " *** MakeRootMap ${genmap_install_dir}/../bin $ENV{DD4hep_DIR}/bin ${DD4hep_DIR}" )
  MESSAGE( STATUS " *** MakeRootMap[${ROOT_VERSION}].cmake run command : ${ROOT_genmap_CMD} -o ${rootmapfile} ${libname}
                 WORKING_DIRECTORY ${genmap_install_dir} "
    ) 

  if(APPLE)
    SET ( ENV{DYLD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{DYLD_LIBRARY_PATH}:$ENV{DD4HEP_LIBRARY_PATH} )
  else()
    SET ( ENV{LD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{LD_LIBRARY_PATH} )
  endif()
  # EXECUTE_PROCESS( COMMAND echo LD_LIBRARY_PATH = $ENV{LD_LIBRARY_PATH} )
  EXECUTE_PROCESS( COMMAND 
    ${ROOT_genmap_CMD} -o ${rootmapfile} ${libname}
    WORKING_DIRECTORY ${genmap_install_dir}
    )


