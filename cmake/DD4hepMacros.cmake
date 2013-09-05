######################################################################
#
# Useful cmake macros for building DD4hep
#  
######################################################################

# taken from cmake4hep (https://svnsrv.desy.de/desy/aidasoft/cmake4hep/trunk) :
 

#---------------------------------------------------------------------------
# helper macro to display standard cmake variables and force write to cache
# otherwise outdated values may appear in ccmake gui
MACRO( DISPLAY_STD_VARIABLES )
#---------------------------------------------------------------------------

    MESSAGE( STATUS )
    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
    MESSAGE( STATUS "Change values with: cmake -D<Variable>=<Value>" )

    IF( DEFINED CMAKE_INSTALL_PREFIX )
        MESSAGE( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
    ENDIF()


    IF( DEFINED CMAKE_BUILD_TYPE )
        MESSAGE( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
    ENDIF()

    IF( DEFINED BUILD_SHARED_LIBS )
        MESSAGE( STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}" )
    ENDIF()

    IF( DEFINED BUILD_TESTING )
        MESSAGE( STATUS "BUILD_TESTING = ${BUILD_TESTING}" )
    ENDIF()

    IF( DEFINED INSTALL_DOC )
        MESSAGE( STATUS "INSTALL_DOC = ${INSTALL_DOC}" )
    ENDIF()

    IF( DEFINED CMAKE_PREFIX_PATH )
        LIST( REMOVE_DUPLICATES CMAKE_PREFIX_PATH )
        #MESSAGE( STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
        MESSAGE( STATUS "CMAKE_PREFIX_PATH =" )
        FOREACH( _path ${CMAKE_PREFIX_PATH} )
            MESSAGE( STATUS "   ${_path};" )
        ENDFOREACH()
        #SET( CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" CACHE PATH "CMAKE_PREFIX_PATH" FORCE )
    ENDIF()

    IF( DEFINED CMAKE_MODULE_PATH )
        LIST( REMOVE_DUPLICATES CMAKE_MODULE_PATH )
        #MESSAGE( STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
        MESSAGE( STATUS "CMAKE_MODULE_PATH =" )
        FOREACH( _path ${CMAKE_MODULE_PATH} )
            MESSAGE( STATUS "   ${_path};" )
        ENDFOREACH()
        SET( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" CACHE PATH "CMAKE_MODULE_PATH" FORCE )
    ENDIF()

    MESSAGE( STATUS "-------------------------------------------------------------------------------" )
    MESSAGE( STATUS )

ENDMACRO( DISPLAY_STD_VARIABLES )



#---------------------------------------------------------------------------
# helper macro for generating project configuration file
MACRO( GENERATE_PACKAGE_CONFIGURATION_FILES )
#---------------------------------------------------------------------------

    FOREACH( arg ${ARGN} )
        IF( ${arg} MATCHES "Config.cmake" )
            IF( EXISTS "${PROJECT_SOURCE_DIR}/cmake/${arg}.in" )
                CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${arg}.in"
                                "${PROJECT_BINARY_DIR}/${arg}" @ONLY
                )
                INSTALL( FILES "${PROJECT_BINARY_DIR}/${arg}" DESTINATION . )
                #IF( EXISTS "${_current_dir}/MacroCheckPackageLibs.cmake" )
                #    INSTALL( FILES "${_current_dir}/MacroCheckPackageLibs.cmake" DESTINATION cmake )
                #ENDIF()
                #IF( EXISTS "${_current_dir}/MacroExportPackageDeps.cmake" )
                #    INSTALL( FILES "${_current_dir}/MacroExportPackageDeps.cmake" DESTINATION cmake )
                #ENDIF()
            ENDIF()
        ENDIF()


        IF( ${arg} MATCHES "ConfigVersion.cmake" )
            # version configuration file
            IF( EXISTS "${PROJECT_SOURCE_DIR}/cmake/${arg}.in" )
                CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${arg}.in"
                                "${PROJECT_BINARY_DIR}/${arg}" @ONLY
                )
                INSTALL( FILES "${PROJECT_BINARY_DIR}/${arg}" DESTINATION . )
                #IF( EXISTS "${_current_dir}/MacroCheckPackageVersion.cmake" )
                #    INSTALL( FILES "${_current_dir}/MacroCheckPackageVersion.cmake" DESTINATION cmake )
                #ENDIF()
            ENDIF( EXISTS "${PROJECT_SOURCE_DIR}/cmake/${arg}.in" )
        ENDIF()

        IF( ${arg} MATCHES "LibDeps.cmake" )
            EXPORT_LIBRARY_DEPENDENCIES( "${arg}" )
            INSTALL( FILES "${PROJECT_BINARY_DIR}/${arg}" DESTINATION lib/cmake )
        ENDIF()

    ENDFOREACH()

ENDMACRO( GENERATE_PACKAGE_CONFIGURATION_FILES )

