
if(APPLE)
  #MESSAGE( STATUS " set DYLD_LIBRARY_PATH to  ./:$ENV{DYLD_LIBRARY_PATH} " )
  SET ( ENV{DYLD_LIBRARY_PATH} ./:$ENV{DYLD_LIBRARY_PATH} )
else()
  SET ( ENV{LD_LIBRARY_PATH} ./:$ENV{LD_LIBRARY_PATH} )
endif()

MESSAGE( STATUS " *** MakeROOTMap.cmake run command : genmap -debug -i ${libname} -o ${rootmapfile}
                 WORKING_DIRECTORY ${genmap_install_dir} "
)
EXECUTE_PROCESS( COMMAND 
		 genmap -debug -i ${libname} -o ${rootmapfile}
                 WORKING_DIRECTORY ${genmap_install_dir}
)
