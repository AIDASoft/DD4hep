#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================

##set(DD4HEP_DEBUG_CMAKE 1)
message ( STATUS "INCLUDING DD4hepBuild.cmake" )
include ( CMakeParseArguments )
set ( DD4hepBuild_included ON )

#---------------------------------------------------------------------------------------------------
macro(dd4hep_to_parent_scope val)
  set ( ${val} ${${val}} PARENT_SCOPE )
endmacro(dd4hep_to_parent_scope)

#---------------------------------------------------------------------------------------------------
#  MACRO: dd4hep_set_compiler_flags
#
#  Set compiler flags
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
macro(dd4hep_set_compiler_flags)
  include(CheckCXXCompilerFlag)

  SET(COMPILER_FLAGS -Wall -Wextra -pedantic -Wshadow -Wformat-security -Wno-long-long -Wdeprecated -fdiagnostics-color=auto)

  # AppleClang/Clang specific warning flags
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    set ( COMPILER_FLAGS ${COMPILER_FLAGS} -Winconsistent-missing-override -Wno-c++1z-extensions -Wheader-hygiene )
  endif()

  FOREACH( FLAG ${COMPILER_FLAGS} )
    ## meed to replace the minus or plus signs from the variables, because it is passed
    ## as a macro to g++ which causes a warning about no whitespace after macro
    ## definition
    STRING(REPLACE "-" "_" FLAG_WORD ${FLAG} )
    STRING(REPLACE "+" "P" FLAG_WORD ${FLAG_WORD} )

    CHECK_CXX_COMPILER_FLAG( "${FLAG}" CXX_FLAG_WORKS_${FLAG_WORD} )
    IF( ${CXX_FLAG_WORKS_${FLAG_WORD}} )
      MESSAGE ( STATUS "Adding ${FLAG} to CXX_FLAGS" )
      SET ( CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS} ")
    ELSE()
      MESSAGE ( STATUS "NOT Adding ${FLAG} to CXX_FLAGS" )
    ENDIF()
  ENDFOREACH()

  CHECK_CXX_COMPILER_FLAG("-ftls-model=global-dynamic" CXX_FLAG_WORKS_FTLS_global_dynamic)

  if (CXX_FLAG_WORKS_FTLS_global_dynamic)
    set ( CMAKE_CXX_FLAGS "-ftls-model=global-dynamic ${CMAKE_CXX_FLAGS} ")
  else()
    message( FATAL_ERROR "The provided compiler does not support the flag -ftls-model=global-dynamic" )
  endif()

  find_package(Threads REQUIRED)

  if ( THREADS_HAVE_PTHREAD_ARG OR CMAKE_USE_PTHREADS_INIT )
    set ( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -pthread")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
  elseif ( CMAKE_THREAD_LIBS_INIT )
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  else()
    # Assume standard gcc and pthreads library
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
    message( STATUS "Unknown thread library: CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS}" )
  endif()

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
  elseif ( ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" ))
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  else()
    MESSAGE( WARNING "We do not test with the ${CMAKE_CXX_COMPILER_ID} compiler, use at your own discretion" )
  endif()

 #rpath treatment
 if (APPLE)
   # use, i.e. don't skip the full RPATH for the build tree
   SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

   # when building, don't use the install RPATH already
   # (but later on when installing)
   SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

   # add the automatically determined parts of the RPATH
   # which point to directories outside the build tree to the install RPATH
   SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

   # the RPATH to be used when installing, but only if it's not a system directory
   LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
   IF("${isSystemDir}" STREQUAL "-1")
     SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
   ENDIF("${isSystemDir}" STREQUAL "-1")
 endif()
endmacro(dd4hep_set_compiler_flags)

#---------------------------------------------------------------------------------------------------
#  dd4hep_debug
#
#  Print messages if debug flag is enabled
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_debug msg )
  if(DD4HEP_DEBUG_CMAKE) 
    get_property(pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
    string ( LENGTH "${msg}" lmsg ) 
    if ( ${lmsg} GREATER 1024 )
      string ( SUBSTRING "${msg}" 0 132 pmsg ) 
      message( STATUS "D++> [${pkg}] ${pmsg}" )
    else()
      message( STATUS "D++> [${pkg}] ${msg}" )
    endif()
  endif()
endfunction( dd4hep_debug )

#---------------------------------------------------------------------------------------------------
#  dd4hep_include_directories
#
#  Same as include_directories but treat all external packages as SYSTEM -> no warnings
#
#  \author  A.Sailer
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_include_directories pkg_incs )
  FOREACH( INCDIR ${pkg_incs} )
    string(FIND "${INCDIR}" "${CMAKE_SOURCE_DIR}" out)
    IF("${out}" EQUAL 0)
      INCLUDE_DIRECTORIES( ${INCDIR} )
    ELSE()
      INCLUDE_DIRECTORIES( SYSTEM ${INCDIR} )
    ENDIF()
  ENDFOREACH()
endfunction( dd4hep_include_directories )

#---------------------------------------------------------------------------------------------------
#  dd4hep_print
#
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_print msg )
  message ( STATUS ${msg} )
endfunction ( dd4hep_print )

function ( dd4hep_skipmsg msg )
  message ( STATUS "SKIPPED !!!!!!!!!!!!!!  ${msg}" )
endfunction ( dd4hep_skipmsg )

#---------------------------------------------------------------------------------------------------
#  dd4hep_fatal
#
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_fatal msg )
  get_property(pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
  if ( "${pkg}" STREQUAL "" )
    message ( FATAL_ERROR "++> ${msg}" )
  else()
    message ( FATAL_ERROR "++> [${pkg}] ${msg}" )
  endif()
endfunction ( dd4hep_fatal )

#---------------------------------------------------------------------------------------------------
#  dd4hep_set_version
#
#  Set version structure for building the DD4hep software
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_set_version packageName )
  cmake_parse_arguments ( ARG "" "MAJOR;MINOR;PATCH" "" ${ARGN} )
  if ( NOT "${packageName}" STREQUAL "" )
    project ( ${packageName} )
  else()
    dd4hep_fatal ( "${packageName}: !!! Attempt to define a DD4hep project without a name !!!" )
  endif()
  set ( major ${ARG_MAJOR} )
  set ( minor ${ARG_MINOR} )
  set ( patch ${ARG_PATCH} )
  
  if ( "${major}" STREQUAL "" ) 
    set ( major ${DD4hep_VERSION_MAJOR} )
  endif()
  if ( "${minor}" STREQUAL "" ) 
    set ( minor ${DD4hep_VERSION_MINOR} )
  endif()
  if ( "${patch}" STREQUAL "" ) 
    set ( patch ${DD4hep_VERSION_PATCH} )
  endif()

  if ( NOT ("${major}" STREQUAL "" OR "${minor}" STREQUAL "" OR "${patch}" STREQUAL "") )
    set( ${packageName}_VERSION_MAJOR ${major} PARENT_SCOPE )
    set( ${packageName}_VERSION_MINOR ${minor} PARENT_SCOPE )
    set( ${packageName}_VERSION_PATCH ${patch} PARENT_SCOPE )
    set( ${packageName}_VERSION       "${major}.${minor}" PARENT_SCOPE )
    set( ${packageName}_SOVERSION     "${major}.${minor}" PARENT_SCOPE )
  else()
    dd4hep_fatal ( "${packageName}: No Package versions specified.....->  ( ${major}.${minor}.${patch} )" )
  endif()
endfunction( dd4hep_set_version )

#---------------------------------------------------------------------------------------------------
#  dd4hep_print_options
#
#  Print the current option setup for informational purposes
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
function ( dd4hep_print_options )
  dd4hep_print ( "+-------------------------------------------------------------------------------" )
  dd4hep_print ( "|  DD4hep build setup:                                                          " )
  dd4hep_print ( "|                                                                               " )
  dd4hep_print ( "|  CMAKE_MODULE_PATH:  ${CMAKE_MODULE_PATH}                                     " )
  dd4hep_print ( "|  DD4HEP_USE_XERCESC: ${DD4HEP_USE_XERCESC}                                    " )
  dd4hep_print ( "|  XERCESC_ROOT_DIR:   ${XERCESC_ROOT_DIR}                                      " )
  dd4hep_print ( "|  DD4HEP_USE_LCIO:    ${DD4HEP_USE_LCIO}                                       " )
  dd4hep_print ( "|  LCIO_DIR:           ${LCIO_DIR}                                              " )
  dd4hep_print ( "|  DD4HEP_USE_GEANT4:  ${DD4HEP_USE_GEANT4}                                     " )
  dd4hep_print ( "|  Geant4_DIR:         ${Geant4_DIR}                                            " )
  dd4hep_print ( "|  DD4HEP_USE_PYROOT:  ${DD4HEP_USE_PYROOT}                                     " )
  dd4hep_print ( "|  BUILD_TESTING:      ${BUILD_TESTING}                                         " )
  dd4hep_print ( "|                                                                               " )
  dd4hep_print ( "+-------------------------------------------------------------------------------" )
endfunction ( dd4hep_print_options )

#---------------------------------------------------------------------------------------------------
#  dd4hep_print_cmake_options
#
#  usage() like function to be called if bad cmake arguments are supplied....
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
function( dd4hep_print_cmake_options )
  cmake_parse_arguments ( ARG "" "ERROR" "OPTIONAL" ${ARGN} )  
  if ( NOT "${ARG_OPTIONAL}" STREQUAL "" ) 
    dd4hep_print ( "+---------------------------------------------------------------------------+")
    foreach ( opt ${ARG_OPTIONAL} )
      dd4hep_print ( "| ${opt}" )
    endforeach()
  endif()
  dd4hep_print ( "+--Option name  ------Description ----------------------------------Default-+")
  dd4hep_print ( "|  DD4HEP_USE_XERCESC Enable 'Detector Builders' based on XercesC   OFF     |")
  dd4hep_print ( "|                     Requires XERCESC_ROOT_DIR to be set                   |")
  dd4hep_print ( "|                     or XercesC in CMAKE_MODULE_PATH                       |")
  dd4hep_print ( "|  DD4HEP_USE_GEANT4  Enable the simulation part based on Geant4    OFF     |")
  dd4hep_print ( "|                     Requires Geant_DIR to be set                          |")
  dd4hep_print ( "|                     or Geant4 in CMAKE_MODULE_PATH                        |")
  dd4hep_print ( "|  DD4HEP_USE_LCIO    Build lcio extensions                         OFF     |")
  dd4hep_print ( "|                     Requires LCIO_DIR to be set                           |")
  dd4hep_print ( "|                     or LCIO in CMAKE_MODULE_PATH                          |")
  dd4hep_print ( "|  DD4HEP_USE_GEAR    Build gear wrapper for backward compatibility OFF     |")
  dd4hep_print ( "|  BUILD_TESTING      Enable and build tests                        ON      |")
  dd4hep_print ( "|  DD4HEP_USE_PYROOT  Enable 'Detector Builders' based on PyROOT    OFF     |")
  dd4hep_print ( "+---------------------------------------------------------------------------+")
  if ( NOT "${ARG_ERROR}" STREQUAL "" ) 
    dd4hep_fatal ( "Invalid cmake options supplied!" )
  endif()
endfunction( dd4hep_print_cmake_options )

#---------------------------------------------------------------------------------------------------
#  dd4hep_configure_output
#
#  Setup build and install target directories
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
macro ( dd4hep_configure_output )
  cmake_parse_arguments ( ARG "" "OUTPUT;INSTALL" "" ${ARGN} )

  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "One of: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
  endif()

  if ( NOT "${ARG_OUTPUT}" STREQUAL "" )
    set ( LIBRARY_OUTPUT_PATH    ${ARG_OUTPUT}/lib )
    set ( EXECUTABLE_OUTPUT_PATH ${ARG_OUTPUT}/bin )
  else()
    set ( LIBRARY_OUTPUT_PATH    ${CMAKE_CURRENT_BINARY_DIR}/lib )
    set ( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin )
  endif()
  #------------- set the default installation directory to be the source directory
  dd4hep_debug("|++> dd4hep_configure_output: CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}  CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT=${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}" )
  if ( NOT "${ARG_INSTALL}" STREQUAL "" )
    set ( CMAKE_INSTALL_PREFIX ${ARG_INSTALL} CACHE PATH "Set install prefix path." FORCE )
    dd4hep_print( "DD4hep_configure_output: set CMAKE_INSTALL_PREFIX to ${ARG_INSTALL}" )
  elseif ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
      "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."  FORCE )
    dd4hep_print ( "|++> dd4hep_configure_output: CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
  elseif ( CMAKE_INSTALL_PREFIX )
    dd4hep_print( "|++> dd4hep_configure_output: set CMAKE_INSTALL_PREFIX to ${CMAKE_INSTALL_PREFIX}" )
    set ( CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
  endif()
  dd4hep_debug("|++> Installation goes to: ${CMAKE_INSTALL_PREFIX}  <${ARG_INSTALL}>" )
endmacro ( dd4hep_configure_output )

#---------------------------------------------------------------------------------------------------
#  dd4hep_list_to_string
#
#  Create proper string from list
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
function ( dd4hep_list_to_string result )
  cmake_parse_arguments(ARG "" "PREFIX" "ENTRIES" ${ARGV} )
  set ( vals "" )
  foreach( v ${ARG_ENTRIES} )
    set ( vals "${vals} ${v}" )
  endforeach()
  if ( NOT "${vals}" STREQUAL "" )
  set ( vals "${ARG_PREFIX}${vals}" )
  endif()
  set ( ${result} "${vals}" PARENT_SCOPE )
endfunction ( dd4hep_list_to_string )

#---------------------------------------------------------------------------------------------------
#  dd4hep_make_unique_list
#
#  Create clean list without duplicate entries.
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
function ( dd4hep_make_unique_list name )
  cmake_parse_arguments ( ARG "" "" "VALUES" ${ARGN} )
  set ( vals )
  foreach( v ${ARG_VALUES} )
    set ( vals "${vals} ${v}" )
  endforeach()
  string( REGEX REPLACE " " ";" vals "${vals}" )
  if( NOT "${vals}" STREQUAL "" )
    list ( REMOVE_DUPLICATES vals )
  endif()
  set( ${name} ${vals} PARENT_SCOPE )
endfunction( dd4hep_make_unique_list )

#---------------------------------------------------------------------------------------------------
#  dd4hep_find_packageEx
#
#  \author  M.Frank
#  \version 1.0
#---------------------------------------------------------------------------------------------------
function ( dd4hep_find_packageEx PKG_NAME )
  set ( pkg ${PKG_NAME} )
  string ( TOUPPER "${pkg}" pkg )
  if ( "${pkg}" STREQUAL "GEANT4" )
    set (pkg "Geant4" )
  elseif ( "${pkg}" STREQUAL "BOOST" )
    set (pkg "Boost" )
  elseif ( "${pkg}" STREQUAL "LCIO" )
    set (pkg "LCIO" )
  elseif ( "${pkg}" STREQUAL "XERCESC" )
    set (pkg "XercesC" )
  elseif ( "${pkg}" STREQUAL "DD4HEP" )
    set (pkg "DD4hep" )
  elseif ( "${pkg}" STREQUAL "GIT2" )
    set (pkg "git2" )
  elseif ( "${pkg}" STREQUAL "FMT" )
    set (pkg "fmt" )
  endif()
  dd4hep_debug( "Call find_package( ${pkg}/${PKG_NAME} ${ARGN})" )
  ##MESSAGE(STATUS "Call find_package( ${pkg}/${PKG_NAME} ${ARGN})" )
  ##dd4hep_print ( "Call find_package( ${pkg}/${PKG_NAME} ${ARGN})" )
  if ( "${${pkg}_LIBRARIES}" STREQUAL "" )
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} ${ARG_ARGS} )
  else()
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} ${ARG_ARGS} )
  endif()
  # Propagate values to caller
  string ( TOUPPER "${pkg}" PKG )
  set ( libs ${${pkg}_LIBRARY}     ${${pkg}_LIBRARIES}   ${${pkg}_COMPONENT_LIBRARIES} )
  set ( incs ${${pkg}_INCLUDE_DIR} ${${pkg}_INCLUDE_DIRS} )
  if ( NOT "${pkg}" STREQUAL "${PKG}" )
    set ( libs ${libs}  ${${PKG}_LIBRARIES} ${${PKG}_LIBRARY} ${${PKG}_COMPONENT_LIBRARIES} )
    set ( incs ${incs}  ${${PKG}_INCLUDE_DIRS} ${${PKG}_INCLUDE_DIR} )
  endif()
  ##MESSAGE(STATUS "Call find_package: ${pkg}/${PKG_NAME} -> ${libs}" )
  dd4hep_make_unique_list ( libs VALUES ${libs} )
  dd4hep_make_unique_list ( incs VALUES ${incs} )

  set ( ${PKG}_EXISTS       "ON"       PARENT_SCOPE )
  set ( ${PKG}_LIBRARIES    ${libs}    PARENT_SCOPE )
  set ( ${PKG}_INCLUDE_DIRS ${incs}    PARENT_SCOPE )
  if ( "${incs}" STREQUAL "" )
    dd4hep_fatal ( "Unknown package ${pkg}" )
  endif()
endfunction( dd4hep_find_packageEx )

#---------------------------------------------------------------------------------------------------
#  dd4hep_find_package
# 
#  Find dependent packages.
#
#  - Allows to also find internal packages like e.g. DDCore, DDG4 using the internal
#    libarary cache stored in DD4HEP_ALL_PACKAGES. 
#  - If a libary is not explicitly mentioned therein, and the package is not explicitly 
#    switched OFF by a global property of the name "DD4HEP_USE_${name}" or a global 
#    variable "DD4HEP_USE_${name}", dd4hep_find_packageEx is called.
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_find_package name found )
  cmake_parse_arguments(ARG "" "TYPE" "" ${ARGN} )
  string ( TOUPPER ${name} NAME )
  get_property ( all_packages GLOBAL PROPERTY DD4HEP_ALL_PACKAGES )
  set ( found_package -1 )

  if ( NOT "${all_packages}" STREQUAL "" )
    list ( FIND all_packages "${NAME}" found_package )
  endif()

  get_property( use_pkg   GLOBAL PROPERTY DD4HEP_USE_${NAME} )

  # For internal dependencies we rely on the correct order of the included directories
  if ( NOT found_package EQUAL -1 )
    # Resolve dependency from local packages
    get_property(used GLOBAL PROPERTY DD4HEP_USE_${NAME} )
    get_property(incs GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS )
    get_property(libs GLOBAL PROPERTY ${NAME}_LIBRARIES )
    set ( DD4HEP_USE_${NAME}   "ON"    PARENT_SCOPE )
    set ( ${NAME}_LIBRARIES    ${libs} PARENT_SCOPE )
    set ( ${NAME}_INCLUDE_DIRS ${incs} PARENT_SCOPE )
    set ( ${found} "ON" PARENT_SCOPE )
  elseif ( found_package EQUAL -1 AND "${ARG_TYPE}" STREQUAL "INTERNAL" )
    set_property( GLOBAL PROPERTY DD4HEP_USE_${NAME} "OFF" )
    set ( ${found} "OFF" PARENT_SCOPE )
  elseif ( ( DEFINED DD4HEP_USE_${name} AND NOT "${DD4HEP_USE_${name}}" ) OR
           ( DEFINED DD4HEP_USE_${NAME} AND NOT "${DD4HEP_USE_${NAME}}" ) )
    set ( ${found} "OFF" PARENT_SCOPE )
  elseif ( DEFINED use_pkg AND NOT "${use_pkg}" )
    set ( ${found} "OFF" PARENT_SCOPE )
  else()
    #
    # 3 possibilities left:
    # 1) Either use external package cache from previous call (same call args!) or
    # 2) call findPackage again/first time....or
    # 3) package does not exist!
    #
    get_property( pkg_setup GLOBAL PROPERTY ${NAME}_COMPONENTS )
    set ( ARGN ${ARG_UNPARSED_ARGUMENTS} )
    set ( arguments ${NAME} ${ARGN} )
    list ( REMOVE_DUPLICATES arguments )
    list ( REMOVE_ITEM arguments "REQUIRED" "QUIET" )

    if ( "${pkg_setup}" STREQUAL "${arguments}" )
      get_property ( incs   GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS )
      get_property ( libs   GLOBAL PROPERTY ${NAME}_LIBRARIES    )
      get_property ( exists GLOBAL PROPERTY ${NAME}_EXISTS       )
      set ( DD4HEP_USE_${NAME}   "ON"       PARENT_SCOPE )
      set ( ${NAME}_EXISTS       ${exists}  PARENT_SCOPE )
      set ( ${NAME}_LIBRARIES    ${libs}    PARENT_SCOPE )
      set ( ${NAME}_INCLUDE_DIRS ${incs}    PARENT_SCOPE )
      set ( ${found}             "ON"       PARENT_SCOPE )	
    else()
      dd4hep_find_packageEx( ${name} ${ARGN} )
      dd4hep_debug("DD4hep_find_package: DD4HEP_USE_${name}: ${DD4HEP_USE_${NAME}} Exists: ${${NAME}_EXISTS} ARGS:${arguments}")
      if ( "${${NAME}_EXISTS}" )
	dd4hep_debug( "DD4hep_find_package ${NAME} Incs: ${incs}" )
	dd4hep_debug( "DD4hep_find_package ${NAME} Libs: ${libs}" )
	set_property ( GLOBAL PROPERTY ${NAME}_COMPONENTS   ${arguments} )
	set_property ( GLOBAL PROPERTY ${NAME}_INCLUDE_DIRS ${${NAME}_INCLUDE_DIRS} )
	set_property ( GLOBAL PROPERTY ${NAME}_LIBRARIES    ${${NAME}_LIBRARIES} )
	set_property ( GLOBAL PROPERTY ${NAME}_EXISTS       ${${NAME}_EXISTS} )
	dd4hep_to_parent_scope ( DD4HEP_USE_${NAME} )
	dd4hep_to_parent_scope ( ${NAME}_EXISTS )
	dd4hep_to_parent_scope ( ${NAME}_LIBRARIES )
	dd4hep_to_parent_scope ( ${NAME}_INCLUDE_DIRS )
	set ( ${found} "ON" PARENT_SCOPE )
      else()
	set ( ${found} "OFF" PARENT_SCOPE )
      endif()
    endif()
  endif()
endfunction( dd4hep_find_package )

#---------------------------------------------------------------------------------------------------
#  dd4hep_install_dir
#
#  Install all directories as indicated by all unparsed arguments to the 
#  output destination DESTINATION.
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_install_dir )
  cmake_parse_arguments ( ARG "" "DESTINATION" "" ${ARGN} )
  if( NOT "${ARG_UNPARSED_ARGUMENTS}" STREQUAL "" )
    foreach ( d ${ARG_UNPARSED_ARGUMENTS} )
      install ( DIRECTORY ${d}
        DESTINATION ${ARG_DESTINATION}
        PATTERN ".svn" EXCLUDE )
    endforeach()
  endif()
endfunction( dd4hep_install_dir )

#---------------------------------------------------------------------------------------------------
#  dd4hep_install_includes
#
#  Install all include directories as indicated by all unparsed arguments to the 
#  output destination DESTINATION.
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_install_includes package )
  dd4hep_print ( "DD4hep_install_includes[${package}]: ${ARGN}" )
  dd4hep_install_dir( ${ARGN} DESTINATION include )
endfunction()

#---------------------------------------------------------------------------------------------------
#  dd4hep_install_files
#
#  Install all files as indicated by the FILES argument or all unparsed arguments to the 
#  output destination DESTINATION.
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_install_files )
  cmake_parse_arguments ( ARG "" "DESTINATION" "FILES" "PROGRAMS" ${ARGN} )
  foreach ( f ${ARG_PROGRAMS} )
    file ( GLOB sources ${f} )
    install (PROGRAMS ${sources} DESTINATION ${ARG_DESTINATION} )
  endforeach()
  foreach ( f ${ARG_UNPARSED_ARGUMENTS} ${ARG_FILES} )
    file ( GLOB sources ${f} )
    install (FILES ${sources} DESTINATION ${ARG_DESTINATION} )
  endforeach()
endfunction( dd4hep_install_files )

#---------------------------------------------------------------------------------------------------
#  dd4hep_unpack_package_opts
#
#  INTERNAL routine not to be used directly by clients
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_unpack_package_opts name opt )
  set ( nam_pkg  )
  set ( req_pkg  )
  set ( typ_pkg  )
  set ( com_pkg )
  set ( comp_pkg )
  set ( srcs_pkg )
  set ( defs_pkg )
  if ( "${opt}" MATCHES "\\[" )
    string ( REGEX REPLACE "\\[;" "" opt "${opt}" )
    string ( REGEX REPLACE "\\["  "" opt "${opt}" )
    string ( REGEX REPLACE ";\\]" "" opt "${opt}" )
    string ( REGEX REPLACE "\\]"  "" opt "${opt}" )
    string ( REPLACE ";" ";" all_opt "${opt}" )
    dd4hep_debug ( "unpack ${name} : ${opt}" )
    foreach( e ${all_opt} )
      if( "${nam_pkg}" STREQUAL "" )
        set ( nam_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "REQUIRED" )
        set ( req_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "INTERNAL" )
        set ( typ_pkg ${e} )
        unset ( act_pkg )
      elseif ( ${e} STREQUAL "EXTERNAL" )
        set ( typ_pkg ${e} )
        unset ( act_pkg )
      elseif ( "${com_pkg}" STREQUAL "" AND ${e} STREQUAL "COMPONENTS" )
        set ( com_pkg ${e} )
        set ( act_pkg ${e} )
      elseif ( "${src_pkg}" STREQUAL "" AND ${e} STREQUAL "SOURCES" )
        set ( act_pkg ${e} )
      elseif ( "${def_pkg}" STREQUAL "" AND ${e} STREQUAL "DEFINITIONS" )
        set ( act_pkg ${e} )
      elseif ( "${act_pkg}" STREQUAL "COMPONENTS" )
        set ( comp_pkg ${comp_pkg} ${e} )
      elseif ( "${act_pkg}" STREQUAL "SOURCES" )
        set ( srcs_pkg ${srcs_pkg} ${e} )
      elseif ( "${act_pkg}" STREQUAL "DEFINITIONS" )
        set ( defs_pkg ${defs_pkg} ${e} )
      endif()
    endforeach()
  else()
    set ( nam_pkg ${opt} )
    set ( req_pkg REQUIRED )
  endif()
  string ( TOUPPER "${nam_pkg}" nam_pkg )
  set ( ${name}_NAME        ${nam_pkg}   PARENT_SCOPE )
  set ( ${name}_REQUIRED    ${req_pkg}   PARENT_SCOPE )
  set ( ${name}_TYPE        ${typ_pkg}   PARENT_SCOPE )
  set ( ${name}_COMPONENT   ${com_pkg}   PARENT_SCOPE )
  set ( ${name}_COMPONENTS  ${comp_pkg}  PARENT_SCOPE )
  set ( ${name}_SOURCES     ${srcs_pkg}  PARENT_SCOPE )
  set ( ${name}_DEFINITIONS ${defs_pkg}  PARENT_SCOPE )
  dd4hep_debug ( "unpack ${name} : ${nam_pkg} ${req_pkg} ${com_pkg} comp: ${comp_pkg} src: ${srcs_pkg}" )
endfunction ( dd4hep_unpack_package_opts )

#---------------------------------------------------------------------------------------------------
#  dd4hep_get_dependency_opts
#
#  INTERNAL routine not to be used directly by clients
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_get_dependency_opts local_incs local_libs local_uses pkg )
  #
  #  If the variables <package>_INCLUDE_DIRS and <package>_LIBRARIES were not set
  #  at the parent level, check if a corresponding property exists....
  #
  string ( TOUPPER "${pkg}" pkg )
  if ( "${${pkg}_INCLUDE_DIRS}" STREQUAL "" )
    get_property ( ${pkg}_INCLUDE_DIRS GLOBAL PROPERTY ${pkg}_INCLUDE_DIRS )
  endif()
  if ( "${${pkg}_LIBRARIES}" STREQUAL "" )
    get_property(${pkg}_LIBRARIES    GLOBAL PROPERTY ${pkg}_LIBRARIES )
  endif()
  set ( libs "${${pkg}_LIBRARIES}" )
  string ( REGEX REPLACE "  " " " libs "${libs}" )
  string ( REGEX REPLACE " " ";"  libs "${libs}" )

  set ( incs "${${pkg}_INCLUDE_DIRS}" )
  string ( REGEX REPLACE "  " " " incs "${incs}" )
  string ( REGEX REPLACE " " ";"  incs "${incs}" )
  
  set ( uses "${${pkg}_USES}" )
  string ( REGEX REPLACE "  " " " uses "${uses}" )
  string ( REGEX REPLACE " " ";"  uses "${uses}" )
  
  set ( ${local_incs} ${incs} PARENT_SCOPE )
  set ( ${local_libs} ${libs} PARENT_SCOPE )
  set ( ${local_uses} ${uses} PARENT_SCOPE )
endfunction ( dd4hep_get_dependency_opts )

#---------------------------------------------------------------------------------------------------
#  dd4hep_handle_optional_sources
#
#  INTERNAL routine not to be used directly by clients
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_handle_optional_sources tag optionals missing uses sources )
  get_property ( pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
  set (miss)
  set (src)
  set (use)
  foreach ( opt ${optionals} )
    dd4hep_unpack_package_opts ( USE ${opt} )
    dd4hep_debug ( "unpack DD4HEP_USE_${USE_NAME}=${DD4HEP_USE_${USE_NAME}} <> ${use_pkg} -- ${opt}" )
    dd4hep_debug ( "|++> ${tag} find_package( ${USE_NAME} ARGS: ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} ${USE_TYPE} )")
    dd4hep_find_package( ${USE_NAME} pkg_found 
      ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} 
      TYPE ${USE_TYPE} )
    if ( DEFINED pkg_found AND NOT "${pkg_found}" )
      dd4hep_print ( "|    ${tag}  ...optional: Skip sources ${USE_SOURCES} [requires ${USE_NAME}]" )
    elseif ( DEFINED pkg_found AND "${pkg_found}" )
      dd4hep_print ( "|    ${tag}  ...optional: ADD sources ${USE_SOURCES} [dependent on ${USE_NAME}]" )
      file ( GLOB opt_sources ${USE_SOURCES} )
      set ( src ${src} ${opt_sources} )
      set ( use ${use} ${opt} )
    elseif ( "${USE_REQUIRED}" STREQUAL "REQUIRED" )
      set ( miss ${miss} ${USE_NAME} )
    else()
      dd4hep_print ( "|    ${tag}  ...optional: Skip sources ${USE_SOURCES} [requires ${USE_NAME}]" )
    endif()
  endforeach()
  set ( ${missing} ${miss} PARENT_SCOPE )
  set ( ${uses}    ${use}  PARENT_SCOPE )
  set ( ${sources} ${src}  PARENT_SCOPE )
endfunction(dd4hep_handle_optional_sources)

#---------------------------------------------------------------------------------------------------
#  dd4hep_use_package
#
#  INTERNAL routine not to be used directly by clients
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_use_package print_prefix inName outName )
  cmake_parse_arguments( ARG "" "NAME" "USES;OPTIONAL" ${ARGN} )
  get_property ( pkg DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME )
  #
  get_property ( pkg_incs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_INCLUDE_DIRS   )
  get_property ( pkg_libs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_LINK_LIBRARIES )
  get_property ( pkg_uses DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY ${inName}_USES )
  #
  set ( missing )
  set ( used_uses ${pkg_uses} )
  set ( used_incs ${pkg_incs} )
  set ( used_libs ${pkg_libs} )
  #
  foreach( use ${ARG_USES} )
    if( "${use}" MATCHES "\\[" )
      dd4hep_unpack_package_opts ( USE ${use} )
      dd4hep_find_package( ${USE_NAME} pkg_found ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} )
      set ( use ${USE_NAME} )
    else()
      dd4hep_find_package( ${use} pkg_found )
    endif()
    if ( "${pkg_found}" )
      dd4hep_debug ( "${print_prefix} package_libs: ${${use}_LIBRARIES}" )
      set ( use ${use} )
    elseif ( NOT "{pkg_found}" )
      dd4hep_print ( "|++> ${print_prefix} ...Missing package: ${use} [Ignored]" )
      unset ( use )
    elseif ( "${USE_REQUIRED}" STREQUAL "REQUIRED" )
      dd4hep_fatal ( "${print_prefix} ...Missing package: ${use} [Fatal REQUIRED]" )
      set ( missing ${missing} ${use} )
      unset ( use )
    else()
      dd4hep_print ( "|    ${print_prefix}  ...optional: Skip sources ${USE_SOURCES} [Usage ${use} not defined]" )
      unset ( use )
    endif()
    set ( used_uses ${used_uses} ${use} )
  endforeach()
  #
  #
  #
  foreach ( use ${ARG_UNPARSED_ARGUMENTS} ${ARG_OPTIONAL} )
    if ( "${use}" MATCHES "\\[" )
      dd4hep_unpack_package_opts ( USE ${use} )
      dd4hep_find_package ( ${USE_NAME} pkg_found ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} )
      set ( use ${USE_NAME} )
      set ( src "sources ${USE_SOURCES}" )
    else()
      dd4hep_find_package ( ${use} pkg_found )
      set ( src ${use} )
    endif()
    if ( NOT "${pkg_found}" )
      dd4hep_print ( "|    ${print_prefix}  ...optional: Skip optional ${src} [${use} Not defined]" )
      unset ( use )
    endif()
    set ( used_uses ${used_uses} ${use} )
  endforeach()
  #
  set (all_used ${used_uses} )
  foreach ( use ${used_uses} )
    get_property ( dependent_uses GLOBAL PROPERTY ${use}_USES )
    foreach ( dependent ${dependent_uses} )
      list ( FIND used_uses ${dependent} already_used )
      if ( ${already_used} EQUAL -1 )
	dd4hep_debug ( "${print_prefix} ADD DEPENDENCY:  ${use} -->  ${dependent}" )
	set ( all_used ${all_used} ${dependent} )
      else()
	dd4hep_debug(  "${print_prefix} ALREADY DEPENDS: ${use} -->  ${dependent}")
	set ( all_used ${all_used} ${dependent} )
      endif()
    endforeach()
  endforeach()
  #
  set ( used_uses ${all_used} )
  foreach ( use ${used_uses} )
    dd4hep_get_dependency_opts( local_incs local_libs local_uses ${use} )
    set ( used_incs ${used_incs} ${local_incs} )
    set ( used_libs ${used_libs} ${local_libs} )
    dd4hep_make_unique_list ( used_incs VALUES ${used_incs} )
    dd4hep_make_unique_list ( used_libs VALUES ${used_libs} )
    dd4hep_debug ( "${print_prefix} DEPENDENCY: ${use} Lib: ${local_libs} " )
    dd4hep_debug ( "${print_prefix} DEPENDENCY: ${use} Inc: ${local_incs} " )
  endforeach()
  #
  dd4hep_make_unique_list ( used_incs VALUES ${used_incs} )
  dd4hep_make_unique_list ( used_libs VALUES ${used_libs} )
  dd4hep_make_unique_list ( used_uses VALUES ${used_uses} )
  dd4hep_make_unique_list ( missing   VALUES ${missing} )
  #
  if ( "${used_uses}" STREQUAL "" AND "${missing}" STREQUAL "" )
    dd4hep_print ( "|++> ${print_prefix} Uses  DEFAULTS" )
  elseif ( "${missing}" STREQUAL "" )
    dd4hep_print ( "|++> ${print_prefix} Uses: ${used_uses}" )
  else()
    dd4hep_debug ( "${print_prefix} Uses:   ${used_uses} " )
    dd4hep_debug ( "${print_prefix} Missing:${missing} " )
  endif()
  dd4hep_debug ( "${print_prefix} Dep.Incs:${used_incs} " )
  dd4hep_debug ( "${print_prefix} Dep.Libs:${used_libs} " )
  #
  set ( ${outName}_MISSING        ${missing}   PARENT_SCOPE )
  set ( ${outName}_INCLUDE_DIRS   ${used_incs} PARENT_SCOPE )
  set ( ${outName}_LINK_LIBRARIES ${used_libs} PARENT_SCOPE )
  set ( ${outName}_USES           ${used_uses} PARENT_SCOPE )
endfunction()

#---------------------------------------------------------------------------------------------------
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_package_properties name upper enabled )
  get_property(n DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME)
  string ( TOUPPER "${n}" N )
  get_property(e GLOBAL PROPERTY DD4HEP_USE_${N} )
  set ( ${name} ${n} PARENT_SCOPE )
  set ( ${upper} ${N} PARENT_SCOPE )
  set ( ${enabled} ${e} PARENT_SCOPE )
endfunction()

#---------------------------------------------------------------------------------------------------
#  dd4hep_add_regular_library
#
#  Arguments      -> See function dd4hep_add_library
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_add_regular_library library )
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Package library[${pkg}] -> ${library}" )
  if ( NOT "${enabled}" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    dd4hep_add_library( ${library} building ${ARGN} PRINT ${tag} )
    if ( "${building}" )
      dd4hep_debug ( "add_package_library -> ${library} ${PKG}_LIBRARIES:${pkg_libs}" )
    else()
      dd4hep_fatal ( "Package library[${pkg}] -> ${binary} Cannot be built! This is an ERROR condition." )    
    endif()
  endif()
endfunction(dd4hep_add_regular_library)

#---------------------------------------------------------------------------------------------------
#  dd4hep_add_package_library
#
#  Package libraries are automatically added to the package's link libraries.
#  Package plugins and plugins depending on a package automically link against 
#  these libraries.
#
#  Arguments      -> See function dd4hep_add_library
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_add_package_library library )
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Package library[${pkg}] -> ${library}" )
  if ( NOT "${enabled}" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    dd4hep_add_library( ${library} building ${ARGN} PRINT ${tag} )
    if ( "${building}" )
      get_property(pkg_libs GLOBAL PROPERTY PROPERTY ${PKG}_LIBRARIES )
      dd4hep_make_unique_list ( pkg_libs VALUES ${pkg_libs} ${library} )
      set_property(GLOBAL PROPERTY ${PKG}_LIBRARIES ${pkg_libs} )
      # Test and print if correct
      get_property(pkg_libs GLOBAL PROPERTY PROPERTY ${PKG}_LIBRARIES )
      dd4hep_debug ( "add_package_library -> ${library} ${PKG}_LIBRARIES:${pkg_libs}" )
    else()
      dd4hep_fatal ( "Package library[${pkg}] -> ${binary} Cannot be built! This is an ERROR condition." )    
    endif()
  endif()
endfunction(dd4hep_add_package_library)

#---------------------------------------------------------------------------------------------------
#  dd4hep_add_executable
#
#  Arguments
#  ---------
#  binary        -> plugin name
#  SOURCES        -> list of source files. Will be expanded to absolute names
#
#  The following variables EXTEND the package defaults
#  INCLUDE_DIRS   -> Additional include directories need to compile the binary
#  LINK_LIBRARIES -> Additional libraries needed to link the binary
#  USES           -> Required package dependencies
#  OPTIONAL       -> Optional package dependency
#                    if required e.g. [LCIO REQUIRED SOURCES aaa.cpp] the plugin will NOT be built
#                    else        e.g. [LCIO          SOURCES aaa.cpp] the plugin will be built, but
#                                     the source files will be omitted.
#  DEFINITIONS    -> Optional compiler definitions to compile the sources
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_add_executable binary )
  #set ( DD4HEP_DEBUG_CMAKE "ON" )
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Executable[${pkg}] -> ${binary}" )
  if ( NOT "${enabled}" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    cmake_parse_arguments ( ARG "" "" "SOURCES;GENERATED;LINK_LIBRARIES;INCLUDE_DIRS;USES;OPTIONAL" ${ARGN})
    dd4hep_debug( "+------------------------------------------- <<<< EXECUTABLE ${pkg}:${binary} >>>> Version: ${${pkg}_VERSION}" )
    if ( NOT "${ARG_OPTIONAL}" STREQUAL "" )
      dd4hep_handle_optional_sources ( ${tag} "${ARG_OPTIONAL}" optional_missing optional_uses optional_sources )
    endif()
    #
    if ( NOT "${optional_missing}" STREQUAL "" )
      dd4hep_print ( "|++> ${tag} SKIPPED. Missing optional dependencies: ${optional_missing}" )
    else()
      set ( uses ${ARG_USES} ${optional_uses} )
      dd4hep_use_package ( ${tag} PACKAGE LOCAL 
        USES     "${uses}"
        OPTIONAL "${ARG_OPTIONAL}" )
      if ( "${LOCAL_MISSING}" STREQUAL "" )
        dd4hep_debug ( "${tag} Executable uses:     ${ARG_USES} -- ${uses}" )
        dd4hep_debug ( "${tag} Executable optional: ${ARG_OPTIONAL}" )
        #
        get_property ( pkg_library GLOBAL PROPERTY ${PKG}_LIBRARIES )
        #
        #  Sources may also be supplied without argument tag:
        if( "${ARG_SOURCES}" STREQUAL "")
	  set ( ARG_SOURCES ${ARG_UNPARSED_ARGUMENTS} )
        endif()
        #  Prepare flags for cint generated sources:
        foreach ( f in  ${ARG_GENERATED} )
          set_source_files_properties( ${f} PROPERTIES COMPILE_FLAGS "-Wno-unused-function -Wno-overlength-strings" GENERATED TRUE )
        endforeach()
        #
        set ( sources ${ARG_GENERATED} ${ARG_SOURCES} ${optional_sources} )
        #
        if( NOT "${sources}" STREQUAL "")
	  set (incs ${LOCAL_INCLUDE_DIRS} ${ARG_INCLUDE_DIRS} )
	  set (libs ${pkg_library} ${LOCAL_LINK_LIBRARIES} ${ARG_LINK_LIBRARIES} )
	  dd4hep_make_unique_list ( incs    VALUES ${incs} )
	  dd4hep_make_unique_list ( libs    VALUES ${libs} )
	  dd4hep_make_unique_list ( sources VALUES ${sources} )
	  #
	  dd4hep_debug ( "${tag} Libs:${libs}" )
	  dd4hep_include_directories( "${incs}" )
	  add_executable( ${binary} ${sources} )
	  target_link_libraries( ${binary} ${libs} )
	  #
	  #  Install the binary to the destination directory
	  install(TARGETS ${binary} 
	    LIBRARY DESTINATION lib 
	    RUNTIME DESTINATION bin )
        else()
	  dd4hep_print ( "|++> ${tag} SKIPPED. No sources to build [Use constraint]" )
        endif()
      else()
        dd4hep_fatal ( "${tag} SKIPPED. Missing dependencies: ${LOCAL_MISSING}" )
      endif()
    endif()
  endif()
endfunction(dd4hep_add_executable)

#---------------------------------------------------------------------------------------------------
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
macro( dd4hep_enable_tests )
  cmake_parse_arguments(MACRO_ARG "" "" "" ${ARGV} )
  if (BUILD_TESTING)
    set ( BUILDNAME "${CMAKE_SYSTEM}-${CMAKE_CXX_COMPILER}-${CMAKE_BUILD_TYPE}" CACHE STRING "set build string for cdash")
    dd4hep_list_to_string( _dir_entries PREFIX "DIRS:" ENTRIES ${MACRO_ARG_UNPARSED_ARGUMENTS} )
    dd4hep_print ( "|++> Enable CTest environment....BUILD:${BUILD_TESTING} ${_dir_entries}" )
    include(CTest)
    enable_testing ()
    if ( NOT "${MACRO_ARG_UNPARSED_ARGUMENTS}" STREQUAL "" )
      foreach ( _dir ${MACRO_ARG_UNPARSED_ARGUMENTS} )
        add_subdirectory ( ${_dir} )
      endforeach()
    endif()
  endif()
endmacro( dd4hep_enable_tests )

#---------------------------------------------------------------------------------------------------
macro ( dd4hep_configure_scripts _pkg )
  cmake_parse_arguments(MACRO_ARG "DEFAULT_SETUP;WITH_TESTS" "RUN_SCRIPT" "" ${ARGV} )
  # PackageName is a variable required by existing LC build scripts. 
  # Set it here and unset it at the end of the scope...
  set( PackageName ${_pkg} )
  dd4hep_list_to_string( _dir_entries PREFIX "DIRS:" ENTRIES ${MACRO_ARG_UNPARSED_ARGUMENTS} )
  dd4hep_print ( "|++> Setting up test environment for ${PackageName}: Testing:${BUILD_TESTING} Setup:${MACRO_ARG_DEFAULT_SETUP} With Tests(${MACRO_ARG_WITH_TESTS}): ${_dir_entries}" )
  if ( (NOT "${MACRO_ARG_DEFAULT_SETUP}" STREQUAL "") OR (NOT "${_pkg}" STREQUAL "") )
    configure_file( ${DD4hep_DIR}/cmake/run_test_package.sh ${EXECUTABLE_OUTPUT_PATH}/run_test_${_pkg}.sh @ONLY)
    INSTALL(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/run_test_${_pkg}.sh DESTINATION bin )
    #---- configure run environment ---------------
    configure_file( ${DD4hep_DIR}/cmake/thisdd4hep_package.sh.in  ${EXECUTABLE_OUTPUT_PATH}/this${_pkg}.sh @ONLY)
    install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/this${_pkg}.sh DESTINATION bin )
    #--- install target-------------------------------------
    if ( IS_DIRECTORY scripts )
      dd4hep_install_dir ( compact scripts DESTINATION examples/${_pkg} )
    endif()
    if ( IS_DIRECTORY compact )
      dd4hep_install_dir ( compact DESTINATION examples/${_pkg} )
    endif()
  endif()
  dd4hep_enable_tests ( ${MACRO_ARG_UPARSED_ARGUMENTS} )
  unset( PackageName )
endmacro( dd4hep_configure_scripts )

#---------------------------------------------------------------------------------------------------
#  dd4hep_add_test_reg
#
#  Add test with regular expression output parsing.
#  BUILD_EXEC:  Add and build executable with the same name as the test (Default NONE)
#  OUTPUT       Output file of the test (Default: empty)
#  COMMAND      Command to execute
#  EXEC_ARGS    Arguments to the command
#  REGEX_PASS   Regular expression to indicate that the test succeeded
#  REGEX_FAIL   Regular expression to indicate that the test failed
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( dd4hep_add_test_reg test_name )
  cmake_parse_arguments(ARG "BUILD_EXEC" "OUTPUT" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( "${use_test}" STREQUAL "" )
    dd4hep_print ( "*** Will not build/execute test ${test_name}. Missing dependencies: ${missing} ")
  else()
    if ( ${ARG_BUILD_EXEC} )
      #dd4hep_print ( "Building executable:  ${test_name} SOURCES src/${test_name}.cc")
      dd4hep_add_executable ( ${test_name} SOURCES src/${test_name}.cc )
    endif()

    set ( cmd ${ARG_COMMAND} )
    if ( "${cmd}" STREQUAL "" )
      set ( cmd ${CMAKE_INSTALL_PREFIX}/bin/run_test.sh ${test_name} )
    endif()

    set ( passed ${ARG_REGEX_PASS} ${ARG_REGEX_PASSED} )
    if ( "${passed}" STREQUAL "NONE" )
      unset ( passed )
    elseif ( "${passed}" STREQUAL "" )
      set ( passed "TEST_PASSED" )
    endif()

    set ( failed ${ARG_REGEX_FAIL} ${ARG_REGEX_FAILED} )
    if ( "${failed}" STREQUAL "NONE" )
      unset ( failed )
    endif()
    set ( output ${ARG_OUTPUT} )

    set ( args ${ARG_EXEC_ARGS} )
    if ( "${args}" STREQUAL "" )
      set ( args ${test_name} )
    endif()
    add_test(NAME t_${test_name} COMMAND ${cmd} ${output} ${args} ${output} )
    if ( NOT "${passed}" STREQUAL "" )
      set_tests_properties( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${passed}" )
    endif()
    if ( NOT "${failed}" STREQUAL "" )
      set_tests_properties( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${failed}" )
    endif()
    # Set test dependencies if present
    foreach ( _dep ${ARG_DEPENDS} )
      set_tests_properties( t_${test_name} PROPERTIES DEPENDS t_${_dep} )
    endforeach()
  endif()
endfunction()

#---------------------------------------------------------------------------------------------------
#  fill_dd4hep_library_path
#
#
#  \author  M.Petric
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function ( fill_dd4hep_library_path )
  string(REGEX REPLACE "/lib/libCore.*" "" ROOT_ROOT ${ROOT_Core_LIBRARY})
  SET( ENV{DD4HEP_LIBRARY_PATH} ${ROOT_ROOT}/lib )
  if ( NOT "${Boost_LIBRARY_DIRS}" STREQUAL "" )
    SET( ENV{DD4HEP_LIBRARY_PATH} $ENV{DD4HEP_LIBRARY_PATH}:${Boost_LIBRARY_DIRS} )
  else()
    dd4hep_print("|++> The boost library path cannot be determined. Problems maybe ahead.")
  endif()
  if ( ${DD4HEP_USE_GEANT4} )
    string(REGEX REPLACE "/lib/Geant4.*" "" Geant4_ROOT ${Geant4_DIR})
    SET( ENV{DD4HEP_LIBRARY_PATH} ${Geant4_ROOT}/lib:$ENV{DD4HEP_LIBRARY_PATH} )
  endif()

  if(${DD4HEP_USE_LCIO})
    SET( ENV{DD4HEP_LIBRARY_PATH} ${LCIO_DIR}/lib:$ENV{DD4HEP_LIBRARY_PATH} )
  endif()

  SET( ENV{DD4HEP_LIBRARY_PATH} ${CLHEP_ROOT_DIR}/lib:$ENV{DD4HEP_LIBRARY_PATH} )

  if(${DD4HEP_USE_XERCESC})
    SET( ENV{DD4HEP_LIBRARY_PATH} ${XERCESC_ROOT_DIR}/lib:$ENV{DD4HEP_LIBRARY_PATH} )
  endif()

  SET( ENV{DD4HEP_LIBRARY_PATH} ${CMAKE_BINARY_DIR}/lib:$ENV{DD4HEP_LIBRARY_PATH} )
endfunction()


#---------------------------------------------------------------------------------------------------
#  dd4hep_add_dictionary
#
#  Arguments
#  ---------
#  dictionary     -> dictionary name
#  SOURCES        -> Globbing expression to find source files
#  EXCLUDE        -> Files to exclude if they are found via SOURCES
#  LINKDEF        -> Last entry when calling rootcling
#  USES           -> Libraries needed to link the binary
#
#  OPTIONS        -> Options to pass to rootcling
#  INCLUDES       -> Additional include directories need to compile the binary
#  DEFINITIONS    -> Additional compiler definitions to compile the sources
#  OUTPUT         -> 
#
#  \author  A.Sailer
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function(dd4hep_add_dictionary dictionary )
  cmake_parse_arguments(ARG "" "" "SOURCES;EXCLUDE;LINKDEF;OPTIONS;USES;DEFINITIONS;INCLUDES;OUTPUT" ${ARGN} )
  dd4hep_print ( "|++++> Building dictionary ... ${dictionary}" )

  file(GLOB headers ${ARG_SOURCES})
  file(GLOB excl_headers ${ARG_EXCLUDE})

  foreach( f ${excl_headers} )
    list( REMOVE_ITEM headers ${f} )
    dd4hep_debug ( "|++        exclude: ${f}" )
  endforeach()

  foreach( f ${headers} )
    dd4hep_debug ( "|++        ${dictionary} is using: ${f}" )
  endforeach()
  
  set(inc_dirs ${CMAKE_CURRENT_SOURCE_DIR}/include)
  foreach(inc ${ARG_INCLUDES})
    LIST(APPEND inc_dirs ${inc})
  endforeach()

  set(comp_defs)
  foreach(def ${ARG_DEFINITIONS})
    LIST(APPEND comp_defs ${def})
  endforeach()

  foreach(DEP ${ARG_USES})
    # Get INCLUDE DIRECTORIES from Dependencies
    LIST(APPEND inc_dirs $<TARGET_PROPERTY:${DEP},INTERFACE_INCLUDE_DIRECTORIES>)
    # Get COMPILE DEFINITIONS from Dependencies
    LIST(APPEND comp_defs $<TARGET_PROPERTY:${DEP},INTERFACE_COMPILE_DEFINITIONS>)
  endforeach()

  #
  file ( GLOB linkdefs ${ARG_LINKDEF} )
  #
  dd4hep_debug("|++        Linkdef:    '${linkdefs}'" ) 
  dd4hep_debug("|++        Definition: '${comp_defs}'" ) 
  dd4hep_debug("|++        Include:    '${inc_dirs}'" ) 
  dd4hep_debug("|++        Files:      '${headers}'" ) 
  dd4hep_debug("|++        Unparsed:   '${ARG_UNPARSED_ARGUMENTS}'" ) 
  dd4hep_debug("|++        Sources:    '${CMAKE_CURRENT_SOURCE_DIR}'" ) 
  #
  set ( output_dir ${CMAKE_CURRENT_BINARY_DIR}/../lib )
  if ( NOT "${ARG_OUTPUT}" STREQUAL "" )
    set ( output_dir ${ARG_OUTPUT} )
  endif()

  SET(COMP_DEFS )
  file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${dictionary}_arguments
    CONTENT "${ROOT_rootcling_CMD} -cint -f ${dictionary}.cxx -s ${output_dir}/${dictionary} -inlineInputHeader -c -p ${ARG_OPTIONS} -std=c++${CMAKE_CXX_STANDARD}  \
   $<$<BOOL:${comp_defs}>:-D$<JOIN:${comp_defs}, -D>> \
   $<$<BOOL:${inc_dirs}>:-I$<JOIN:${inc_dirs}, -I>> \
   $<JOIN:${headers}, >  $<JOIN:${linkdefs}, >"
    )
  add_custom_command(OUTPUT ${dictionary}.cxx ${output_dir}/${dictionary}_rdict.pcm
    COMMAND bash ${dictionary}_arguments
    DEPENDS ${headers} ${linkdefs}
    )
  add_custom_target(${dictionary}
    DEPENDS ${dictionary}.cxx ${output_dir}/${dictionary}_rdict.pcm
    )

  set_source_files_properties(${dictionary}.cxx ${output_dir}/${dictionary}_rdict.pcm
    PROPERTIES
    GENERATED TRUE
    COMPILE_FLAGS "-Wno-unused-function -Wno-overlength-strings"
    )

  #  Install the binary to the destination directory
  install(FILES ${output_dir}/${dictionary}_rdict.pcm DESTINATION lib)

endfunction()


#---------------------------------------------------------------------------------------------------
#
#  dd4hep_add_plugin 
#
#  Arguments
#  ---------
#  binary         -> plugin name
#  SOURCES        -> Globbing expression to find source files
#  GENERATED      -> List of source files
#  USES           -> Libraries needed to link the binary
#
#  INCLUDES       -> Additional include directories need to compile the binary
#  DEFINITIONS    -> Optional compiler definitions to compile the sources
#  NOINSTALL      -> If set do not install the plugin
#  \author  A.Sailer
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function(dd4hep_add_plugin binary)
  cmake_parse_arguments(ARG "NOINSTALL" "" "SOURCES;GENERATED;USES;INCLUDES;DEFINITIONS" ${ARGN})
  if ( ${ARG_NOINSTALL} )
    set(NOINSTALL NOINSTALL)
  endif()
  file(GLOB SOURCES ${ARG_SOURCES})

  add_library(${binary} SHARED ${SOURCES} ${ARG_GENERATED})
  target_link_libraries(${binary} PUBLIC ${ARG_USES})
  target_include_directories(${binary} PUBLIC ${ARG_INCLUDES})
  target_compile_definitions(${binary} PUBLIC ${ARG_DEFINITIONS})
  dd4hep_generate_rootmap(${binary})
  if(NOT ${ARG_NOINSTALL})
    set(install_destination "lib")
    if(CMAKE_INSTALL_LIBDIR)
      set(install_destination ${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS ${binary} LIBRARY DESTINATION ${install_destination})
  endif()
endfunction(dd4hep_add_plugin)


#
# Macro to set up ROOT:: targets so that we can use the same code for root 6.8 and for root 6.10 and beyond
# This is called in dd4hep, and in DD4hepConfig
#
macro(DD4HEP_SETUP_ROOT_TARGETS)

  #ROOT CXX Flags are a string with quotes, not a list, so we need to convert to a list...
  string(REPLACE " " ";" ROOT_CXX_FLAGS ${ROOT_CXX_FLAGS})

  IF(NOT TARGET ROOT::Core)
    #in ROOT before 6.10 there is no ROOT namespace, so we create ROOT::Core ourselves
    ADD_LIBRARY(ROOT::Core INTERFACE IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(ROOT::Core
      PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${ROOT_CXX_FLAGS}"
      INTERFACE_INCLUDE_DIRECTORIES ${ROOT_INCLUDE_DIRS}
      )
    # there is also no dependency between the targets
    TARGET_LINK_LIBRARIES(ROOT::Core INTERFACE Core)
    # we list here the targets we use in dd4hep, as later versions of root have the namespace, we do not have to to this
    # for ever
    foreach(LIB PyROOT Geom GenVector Eve Graf3d RGL Gui RIO MathCore MathMore EG EGL Rint Tree Hist Physics Gdml)
      IF(TARGET ${LIB})
        ADD_LIBRARY(ROOT::${LIB} INTERFACE IMPORTED GLOBAL)
        TARGET_LINK_LIBRARIES(ROOT::${LIB} INTERFACE ${LIB} ROOT::Core)
      ENDIF()
    endforeach()
  ENDIF(NOT TARGET ROOT::Core)

  dd4hep_debug("ROOT Libraries ${ROOT_LIBRARIES}")
  dd4hep_debug("ROOT CXX_FLAGS ${ROOT_CXX_FLAGS}")
  dd4hep_debug("ROOT INCL DIRS ${ROOT_INCLUDE_DIRS}")
  dd4hep_debug("ROOT_VERSION: ${ROOT_VERSION}" )

ENDMACRO()

#
# Do some processing of the imported Boost Targets
# Some libraries are only needed for cxx std 14
# we also have to make sure the boost library location is known in that case
#
#

MACRO(DD4HEP_SETUP_BOOST_TARGETS)

  SET_TARGET_PROPERTIES(Boost::boost
  PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS BOOST_SPIRIT_USE_PHOENIX_V3
  )

  # Try to compile with filesystem header linking against different FS libraries
  SET(HAVE_FILESYSTEM False)
  dd4hep_debug("|++> Checking if compiler supports filesystem library")
  FOREACH(FS_LIB_NAME stdc++fs c++fs )
    dd4hep_debug("|++++> linking against ${FS_LIB_NAME}")
    try_compile(HAVE_FILESYSTEM ${CMAKE_BINARY_DIR}/try ${CMAKE_CURRENT_LIST_DIR}/cmake/TryFileSystem.cpp
      CXX_STANDARD ${CMAKE_CXX_STANDARD}
      CXX_EXTENSIONS False
      OUTPUT_VARIABLE HAVE_FS_OUTPUT
      LINK_LIBRARIES ${FS_LIB_NAME}
      )
    dd4hep_debug("|++++> ${HAVE_FS_OUTPUT}")
    IF(HAVE_FILESYSTEM)
      dd4hep_print("|++> Compiler supports filesystem, linking against ${FS_LIB_NAME}")
      SET(FS_LIBRARIES ${FS_LIB_NAME})
      BREAK()
    ENDIF()
    dd4hep_debug("|++++> Compiler not compatible when linking against ${FS_LIB_NAME}")
  ENDFOREACH()

  IF(NOT HAVE_FILESYSTEM)
    dd4hep_print("|++> Compiler does not have filesystem support, falling  back to Boost::filesystem")
    FIND_PACKAGE(Boost 1.49 REQUIRED COMPONENTS filesystem system)
    SET(FS_LIBRARIES Boost::filesystem Boost::system)
    SET_TARGET_PROPERTIES(Boost::filesystem
      PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS USE_BOOST_FILESYSTEM
    )
  GET_TARGET_PROPERTY(BOOST_FILESYSTEM_LOC Boost::filesystem IMPORTED_LOCATION)
  GET_FILENAME_COMPONENT(BOOST_DIR ${BOOST_FILESYSTEM_LOC} DIRECTORY)
ENDIF()


ENDMACRO()

#
# Do some processing of the imported Geant4 Targets, CLHEP treatment etc.
# Will only be done once, repeated calls should have no effect
#
MACRO(DD4HEP_SETUP_GEANT4_TARGETS)

  # only do this once
  IF(NOT TARGET Geant4::Interface)
    
    #include( ${Geant4_USE_FILE} ) # do not use the use file, this is not very considerate...
    IF((NOT ${Geant4_TLS_MODEL} STREQUAL "global-dynamic") AND NOT ${DD4HEP_IGNORE_GEANT4_TLS})
      MESSAGE(FATAL_ERROR "Geant4 was built with ${Geant4_TLS_MODEL}, DD4hep requires 'global-dynamic'! Ignore this ERROR with DD4HEP_IGNORE_GEANT4_TLS=True ")
    ENDIF()

    if(Geant4_builtin_clhep_FOUND)
      dd4hep_debug("Using Geant4 internal CLHEP")
      set(CLHEP "")
    else()
      IF(NOT TARGET CLHEP::CLHEP)
        FIND_PACKAGE(CLHEP REQUIRED)
      ENDIF()
      set(CLHEP CLHEP::CLHEP)
      dd4hep_debug("Using External CLHEP")
      dd4hep_debug("CLHEP Libraries ${CLHEP_LIBRARIES}")
      dd4hep_debug("CLHEP CXX_FLAGS ${CLHEP_CXX_FLAGS}")
      dd4hep_debug("CLHEP INCL DIRS ${CLHEP_INCLUDE_DIRS}")
      dd4hep_debug("CLHEP_VERSION: ${CLHEP_VERSION}" )
    endif()

    dd4hep_debug("Geant4 Libraries ${Geant4_LIBRARIES}")
    dd4hep_debug("Geant4 CXX_FLAGS ${Geant4_CXX_FLAGS}")
    dd4hep_debug("Geant4 INCL DIRS ${Geant4_INCLUDE_DIRS}")
    dd4hep_debug("Geant4_VERSION: ${Geant4_VERSION}" )

    # Geant4 CXX Flags are a string with quotes, not a list, so we need to convert to a list...
    # Geant4::10.2.2 at least, not in 10.5 (check where it switches)
    string(REPLACE " " ";" Geant4_Flags ${Geant4_CXX_FLAGS} ${Geant4_CXX_FLAGS_${CMAKE_BUILD_TYPE}})
    SET(Geant4_CXX_FLAGS ${Geant4_Flags})

    #Geant4_DEFINITIONS already include -D, we jave to get rid of that so we can join things when creating dictionaries
    SET(G4_DEF_TEMP "")
    foreach(def ${Geant4_DEFINITIONS})
      string(REPLACE "-D" "" def ${def})
      LIST(APPEND G4_DEF_TEMP ${def})
    endforeach()
    SET(Geant4_DEFINITIONS ${G4_DEF_TEMP})
    UNSET(G4_DEF_TEMP)

    ADD_LIBRARY(Geant4::Interface INTERFACE IMPORTED GLOBAL)

    SET_TARGET_PROPERTIES(Geant4::Interface
      PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${Geant4_CXX_FLAGS}"
      INTERFACE_COMPILE_DEFINITIONS "${Geant4_DEFINITIONS}"
      INTERFACE_INCLUDE_DIRECTORIES "${Geant4_INCLUDE_DIRS}"
      )

    IF(CLHEP)
      dd4hep_debug("Adding CLHEP to Geant4::Interface Dependencies")
      TARGET_LINK_LIBRARIES(Geant4::Interface INTERFACE ${CLHEP})
    ENDIF()

    # Geant4_LIBRARIES are imported targets, we just add them all to our own interface library for convenience
    # Geant4 Libraries do not (yet) use a namespace
    foreach(LIB ${Geant4_LIBRARIES})
      TARGET_LINK_LIBRARIES(Geant4::Interface INTERFACE ${LIB})
    endforeach()

    dd4hep_debug("Geant4 Libraries ${Geant4_LIBRARIES};${Geant4_COMPONENT_LIBRARIES}")
    dd4hep_debug("Geant4 Location ${Geant4_LOCATION}")
    dd4hep_debug("Geant4 Defintitions ${Geant4_DEFINITIONS}")
    dd4hep_debug("Geant4 CXX_FLAGS ${Geant4_CXX_FLAGS}")
    dd4hep_debug("Geant4 INCL DIRS ${Geant4_INCLUDE_DIRS}")
    dd4hep_debug("Geant4_VERSION: ${Geant4_VERSION}" )

  ENDIF()
ENDMACRO()

#
# Create Interface library for LCIO
#
MACRO(DD4HEP_SETUP_LCIO_TARGETS)
  IF(NOT TARGET LCIO::LCIO)
    ADD_LIBRARY(LCIO::LCIO INTERFACE IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(LCIO::LCIO
      PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${LCIO_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${LCIO_LIBRARIES}"
      )
  ENDIF()
ENDMACRO()
