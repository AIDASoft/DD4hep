message(STATUS " *** Gaudi listcomponents: Generate map for ${libname} ..." )
  #
  #  Running listcomponents from DD4hep
  #   
  FIND_PROGRAM( Gaudi_listcomponents_CMD listcomponents PATHS ${genmap_install_dir}/../bin ${DD4hep_DIR}/bin
    NO_DEFAULT_PATH NO_CMAKE_PATH)
  ###MESSAGE( STATUS " *** MakeRootMap ${genmap_install_dir}/../bin $ENV{DD4hep_DIR}/bin ${DD4hep_DIR}" )
  MESSAGE( STATUS " *** MakeGaudiMap.cmake run command : ${Gaudi_listcomponents_CMD} -o ${rootmapfile} ${libname}
                 WORKING_DIRECTORY ${genmap_install_dir} "
    ) 
  if(APPLE)
    SET ( ENV{DYLD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{DYLD_LIBRARY_PATH}:$ENV{DD4HEP_LIBRARY_PATH} )
    ##EXECUTE_PROCESS( COMMAND echo DYLD_LIBRARY_PATH = $ENV{DYLD_LIBRARY_PATH} )
  else()
    SET ( ENV{LD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{LD_LIBRARY_PATH} )
  endif()
  # EXECUTE_PROCESS( COMMAND echo LD_LIBRARY_PATH = $ENV{LD_LIBRARY_PATH} )
  EXECUTE_PROCESS( COMMAND 
    ${Gaudi_listcomponents_CMD} -o ${rootmapfile} ${libname}
    WORKING_DIRECTORY ${genmap_install_dir}
    )
