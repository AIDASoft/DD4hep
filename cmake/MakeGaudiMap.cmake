message(STATUS " *** Gaudi listcomponents: Generate map for ${libname} ..." )
  #
  #  Running listcomponents from Gaudi
  #   
  FIND_PROGRAM( Gaudi_listcomponents_CMD listcomponents PATHS ${genmap_install_dir}/../bin ${DD4hep_DIR}/bin )
  ###MESSAGE( STATUS " *** MakeRootMap ${genmap_install_dir}/../bin $ENV{DD4hep_DIR}/bin ${DD4hep_DIR}" )
  MESSAGE( STATUS " *** MakeGaudiMap.cmake run command : ${Gaudi_listcomponents_CMD} -o ${rootmapfile} ${libname}
                 WORKING_DIRECTORY ${genmap_install_dir} "
    ) 

  if(APPLE)
    SET ( ENV{DYLD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{DYLD_LIBRARY_PATH}:$ENV{DD4HEP_LIBRARY_PATH}:$ENV{Boost_LIBRARY_DIRS} )
    EXECUTE_PROCESS( COMMAND echo DYLD_LIBRARY_PATH = $ENV{DYLD_LIBRARY_PATH} )
    EXECUTE_PROCESS( COMMAND echo Boost_LIBRARY_DIRS = $ENV{Boost_LIBRARY_DIRS} )
  else()
    SET ( ENV{LD_LIBRARY_PATH} ${genmap_install_dir}:$ENV{LD_LIBRARY_PATH} )
    #SET ( ENV{LD_PRELOAD} /lib64/libglapi.so )
  endif()
  # EXECUTE_PROCESS( COMMAND echo LD_LIBRARY_PATH = $ENV{LD_LIBRARY_PATH} )
  EXECUTE_PROCESS( COMMAND 
    ${Gaudi_listcomponents_CMD} -o ${rootmapfile} ${libname}
    WORKING_DIRECTORY ${genmap_install_dir}
    )
