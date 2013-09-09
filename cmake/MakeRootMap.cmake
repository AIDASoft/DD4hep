SET ( ENV{LD_LIBRARY_PATH} ./:$ENV{LD_LIBRARY_PATH} )
EXECUTE_PROCESS( COMMAND 
		 genmap -i ${libname} -o ${rootmapfile}
                 WORKING_DIRECTORY ${genmap_install_dir}
)
