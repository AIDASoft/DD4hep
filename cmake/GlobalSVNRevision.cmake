EXECUTE_PROCESS( COMMAND "svnversion"
        OUTPUT_VARIABLE GLOBAL_SVN_REVISION
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        RESULT_VARIABLE _exit_code
	OUTPUT_STRIP_TRAILING_WHITESPACE
        )
  IF( NOT _exit_code EQUAL 0 )
        MESSAGE( STATUS "Couldn't retrieve a version number from SVN to set in documentation.")
        SET(GLOBAL_SVN_REVISION "NoNumberAvailable")
    ENDIF()

MESSAGE(STATUS "Found global svn revision to be ${GLOBAL_SVN_REVISION}")
