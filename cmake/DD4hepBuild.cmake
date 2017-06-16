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
message ( STATUS "INCLUDING DD4hepBuild.... c++11:${DD4HEP_USE_CXX11} c++14:${DD4HEP_USE_CXX14}" )

include ( CMakeParseArguments )
set ( DD4hepBuild_included ON )
##set ( DD4HEP_DEBUG_CMAKE ON )
##set ( CMAKE_CTEST_COMMAND ${CMAKE_CTEST_COMMAND} --test-output-size-passed 4096 )

#---------------------------------------------------------------------------------------------------
macro(dd4hep_to_parent_scope val)
  set ( ${val} ${${val}} PARENT_SCOPE )
endmacro(dd4hep_to_parent_scope)

find_package(Threads REQUIRED)

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

  CHECK_CXX_COMPILER_FLAG("-std=c++14" CXX_FLAG_WORKS_CXX14)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_FLAG_WORKS_CXX11)
  CHECK_CXX_COMPILER_FLAG("-ftls-model=global-dynamic" CXX_FLAG_WORKS_FTLS_global_dynamic)

  if (NOT CXX_FLAG_WORKS_CXX11)
    message( FATAL_ERROR "The provided compiler does not support the C++11 standard" )
  endif()

  if (NOT CXX_FLAG_WORKS_FTLS_global_dynamic)
    message( FATAL_ERROR "The provided compiler does not support the flag -ftls-model=global-dynamic" )
  endif()

  if ( DD4HEP_USE_CXX14 )
    set ( CMAKE_CXX_FLAGS "-std=c++14 -ftls-model=global-dynamic ${CMAKE_CXX_FLAGS} ")
    set ( DD4HEP_USE_CXX11 OFF ) 
    set ( DD4HEP_USE_STDCXX 14 )
    add_definitions(-DDD4HEP_USE_STDCXX=14)
  else()
    set ( CMAKE_CXX_FLAGS "-std=c++11 -ftls-model=global-dynamic ${CMAKE_CXX_FLAGS} ")
    set ( DD4HEP_USE_CXX14 OFF )
    set ( DD4HEP_USE_STDCXX 11 )
    add_definitions(-DDD4HEP_USE_STDCXX=11)
  endif()

  if ( THREADS_HAVE_PTHREAD_ARG )
    set ( CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} -pthread")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pthread")
  elif ( CMAKE_THREAD_LIBS_INIT )
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  endif()

  if( ("${CMAKE_CXX_COMPILER_ID}" EQUAL "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" EQUAL "GNU") )
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  endif()

  if("${CMAKE_CXX_COMPILER_ID}" EQUAL "AppleClang")
    SET ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
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
  if( NOT "${DD4HEP_DEBUG_CMAKE}" STREQUAL "" ) 
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
  dd4hep_print ( "|  DD4HEP_USE_BOOST:   ${DD4HEP_USE_BOOST}  DD4HEP_USE_Boost:${DD4HEP_USE_Boost}" )
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
  dd4hep_print ( "|  DD4HEP_USE_CXX14   Build DD4hep using c++14                      OFF     |")
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
  dd4hep_debug( "dd4hep_configure_output: CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}  CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT=${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}" )
  if ( NOT "${ARG_INSTALL}" STREQUAL "" )
    set ( CMAKE_INSTALL_PREFIX ${ARG_INSTALL} CACHE PATH "Set install prefix path." FORCE )
    dd4hep_print( "dd4hep_configure_output: set CMAKE_INSTALL_PREFIX to ${ARG_INSTALL}" )
  elseif ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
      "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."  FORCE )
    dd4hep_print ( "dd4hep_configure_output: CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
  elseif ( CMAKE_INSTALL_PREFIX )
    dd4hep_print( "dd4hep_configure_output: set CMAKE_INSTALL_PREFIX to ${CMAKE_INSTALL_PREFIX}" )
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
  endif()
  dd4hep_debug( "Call find_package( ${pkg}/${PKG_NAME} ${ARGN})" )
  ##dd4hep_print ( "Call find_package( ${pkg}/${PKG_NAME} ${ARGN})" )
  if ( "${${pkg}_LIBRARIES}" STREQUAL "" )
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} ${ARG_ARGS} )
  else()
    cmake_parse_arguments(ARG "" "" "ARGS" ${ARGN} )
    find_package( ${pkg} QUIET ${ARG_ARGS} )
  endif()
  # Propagate values to caller
  string ( TOUPPER "${pkg}" PKG )
  set ( libs ${${pkg}_LIBRARY}     ${${pkg}_LIBRARIES}   ${${pkg}_COMPONENT_LIBRARIES} )
  set ( incs ${${pkg}_INCLUDE_DIR} ${${pkg}_INCLUDE_DIRS} )
  if ( NOT "${pkg}" STREQUAL "${PKG}" )
    set ( libs ${libs}  ${${PKG}_LIBRARIES} ${${PKG}_LIBRARY} ${${PKG}_COMPONENT_LIBRARIES} )
    set ( incs ${incs}  ${${PKG}_INCLUDE_DIRS} ${${PKG}_INCLUDE_DIR} )
  endif()
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
  elseif ( "${DD4HEP_USE_${name}}" STREQUAL "OFF" OR "${DD4HEP_USE_${NAME}}" STREQUAL "OFF" )
    set ( ${found} "OFF" PARENT_SCOPE )
  elseif ( "${use_pkg}" STREQUAL "OFF" )
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
      dd4hep_debug("dd4hep_find_package: DD4HEP_USE_${name}: ${DD4HEP_USE_${NAME}} Exists: ${${NAME}_EXISTS} ARGS:${arguments}")
      if ( "${${NAME}_EXISTS}" STREQUAL "ON" )
	dd4hep_debug( "dd4hep_find_package ${NAME} Incs: ${incs}" )
	dd4hep_debug( "dd4hep_find_package ${NAME} Libs: ${libs}" )
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
  dd4hep_print ( "dd4hep_install_includes[${package}]: ${ARGN}" )
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
    if ( "${pkg_found}" STREQUAL "OFF" )
      dd4hep_print ( "|    ${tag}  ...optional: Skip sources ${USE_SOURCES} [requires ${USE_NAME}]" )
    elseif ( "${pkg_found}" STREQUAL "ON" )
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
    if ( "${pkg_found}" STREQUAL "ON" )
      dd4hep_debug ( "${print_prefix} package_libs: ${${use}_LIBRARIES}" )
      set ( use ${use} )
    elseif ( "{pkg_found}" STREQUAL "OFF" )
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
    if ( NOT "${pkg_found}" STREQUAL "ON" )
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
#  dd4hep_package
#
#  Arguments
#  ---------
#  packageName      -> name of the package
#  INCLUDE_DIRS     -> Include directories needed to compile package binaries
#  INSTALL_INCLUDES -> Installation directive for header files
#  LINK_LIBRARIES   -> Libraries needed to link the binary
#  USES             -> Required package dependencies
#  OPTIONAL         -> Optional package dependency e.g. [LCIO REQUIRED COMPONENTS]
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_package packageName )
  #cmake_parse_arguments(ARG "" "LIBRARY;MAJOR;MINOR;PATCH;OUTPUT;INSTALL" "USES;OPTIONAL;LINK_LIBRARIES;INCLUDE_DIRS;INSTALL_INCLUDES" ${ARGN})
  cmake_parse_arguments(ARG "" "LIBRARY;MAJOR;MINOR;PATCH" "USES;OPTIONAL;LINK_LIBRARIES;INCLUDE_DIRS;INSTALL_INCLUDES" ${ARGN})
  set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_NAME ${packageName} )
  string ( TOUPPER "${packageName}" PKG_NAME )

  set ( missing_uses )
  foreach ( use ${ARG_USES} )
    dd4hep_unpack_package_opts ( TEST ${use} )
    dd4hep_find_package ( ${TEST_NAME} pkg_found )
    if ( "${pkg_found}" STREQUAL "OFF" )
      set ( missing_uses ${missing_uses} ${TEST_NAME} )
    endif()
  endforeach()

  if ( NOT "${missing_uses}" STREQUAL "" )
    dd4hep_print ( "+-------------------------------------------------------------------------" )
    dd4hep_skipmsg ( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
    dd4hep_skipmsg ( "PACKAGE ${packageName} / ${PKG_NAME} SKIPPED! Missing dependency: ${missing_uses}"     )
    dd4hep_skipmsg ( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" )
    set ( use "OFF" )
    set_property ( GLOBAL PROPERTY DD4HEP_USE_${PKG_NAME} ${use} )
  else()
    dd4hep_set_version ( ${packageName} MAJOR ${ARG_MAJOR} MINOR ${ARG_MINOR} PATCH ${ARG_PATCH} )
    dd4hep_to_parent_scope ( ${packageName}_VERSION_MAJOR )
    dd4hep_to_parent_scope ( ${packageName}_VERSION_MINOR )
    dd4hep_to_parent_scope ( ${packageName}_VERSION_PATCH )
    dd4hep_to_parent_scope ( ${packageName}_VERSION )
    dd4hep_to_parent_scope ( ${packageName}_SOVERSION )
    set ( vsn "Version: ( ${${packageName}_VERSION_MAJOR}.${${packageName}_VERSION_MINOR}.${${packageName}_VERSION_PATCH} )" )
    dd4hep_print ( "+------------------------------------------- <<<< PACKAGE ${packageName} >>>> ${vsn}" )

    dd4hep_use_package ( "Package[${packageName}]" PACKAGE PACKAGE 
      USES     ${ARG_USES} 
      OPTIONAL ${ARG_OPTIONAL} )

    if ( "${PACKAGE_MISSING}" STREQUAL "" )
      #
      #
      #
      #dd4hep_configure_output ( OUTPUT "${ARG_OUTPUT}" INSTALL "${ARG_INSTALL}" )
      #
      #
      set (used_incs  ${PACKAGE_INCLUDE_DIRS} )
      set (used_libs  ${PACKAGE_LINK_LIBRARIES} )
      #
      #  Define the include directories including dependent packages
      #
      list(APPEND used_incs ${CMAKE_CURRENT_SOURCE_DIR}/include )
      foreach( inc ${ARG_INCLUDE_DIRS} )
        list( APPEND used_incs ${CMAKE_CURRENT_SOURCE_DIR}/${inc} )
      endforeach()
      #
      #  Build the list of link libraries required to build the package library and plugins etc.
      #
      dd4hep_make_unique_list ( used_libs VALUES ${used_libs} ${ARG_LINK_LIBRARIES} )
      dd4hep_make_unique_list ( used_incs VALUES ${used_incs} )
      #
      #  Save the variables in the context of the current source directory (ie. THIS package)
      #
      set ( use "ON" )
      set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_INCLUDE_DIRS   ${used_incs} )
      set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_LINK_LIBRARIES ${used_libs} )

      set_property ( GLOBAL PROPERTY ${PKG_NAME}_LIBRARIES    ${used_libs} )
      set_property ( GLOBAL PROPERTY DD4HEP_USE_${PKG_NAME}   ${use} )
      set_property ( GLOBAL PROPERTY ${PKG_NAME}_INCLUDE_DIRS ${used_incs} )
      set_property ( GLOBAL PROPERTY ${PKG_NAME}_USES         ${PACKAGE_USES} )
      #
      #  Add package to 'internal' package list
      #
      get_property ( all_packages GLOBAL PROPERTY DD4HEP_ALL_PACKAGES )
      set ( all_packages ${PKG_NAME} ${all_packages} )
      set_property ( GLOBAL PROPERTY DD4HEP_ALL_PACKAGES ${all_packages} )
      get_property ( use GLOBAL PROPERTY DD4HEP_USE_${PKG_NAME} )
      #
      #  Some debugging:
      #
      dd4hep_debug ( "Property:  DD4HEP_USE_${PKG_NAME}=${use}" )
      dd4hep_debug ( "Used Libs: ${used_libs}" )
      dd4hep_debug ( "Used Incs: ${used_incs}" )
      #
      #  Define the installation pathes of the headers.
      #
      dd4hep_install_dir ( ${ARG_INSTALL_INCLUDES} DESTINATION include )
    else()
      dd4hep_fatal ( "Missing package dependencies: ${PACKAGE_MISSING}" )
    endif()
  endif()
  #
endfunction()

#---------------------------------------------------------------------------------------------------
#  dd4hep_add_library
#
#  Arguments
#  ---------
#  binary         -> plugin name
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
function( dd4hep_add_library binary building )
  cmake_parse_arguments ( ARG "NODEFAULTS" "" "SOURCES;GENERATED;LINK_LIBRARIES;INCLUDE_DIRS;USES;OPTIONAL;DEFINITIONS;PRINT" ${ARGN} )
  dd4hep_package_properties( pkg PKG_NAME enabled )
  set ( tag "Library[${pkg}] -> ${binary}" )
  if ( NOT "${ARG_PRINT}" STREQUAL "" )
    set ( tag ${ARG_PRINT} )
  endif()

  if ( "${enabled}" STREQUAL "OFF" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    dd4hep_debug( "+------------------------------------------- <<<< LIBRARY ${pkg}:${binary} >>>> Version: ${${pkg}_VERSION}" )
    set ( building_binary "OFF" )

    if ( NOT "${ARG_OPTIONAL}" STREQUAL "" )
      dd4hep_handle_optional_sources ( ${tag} "${ARG_OPTIONAL}" optional_missing optional_uses optional_sources )
    endif()

    if ( NOT "${optional_missing}" STREQUAL "" )
      dd4hep_print ( "|++> ${tag} (optional) skipped. Missing dependency: ${optional_missing}" )
    else()
      if ( ${ARG_NODEFAULTS} )
        set ( LOCAL_LINK_LIBRARIES )
        set ( LOCAL_LINK_LIBRARIES )
        set ( LOCAL_DEFINITIONS )
      else()
        dd4hep_use_package( "${tag}" PACKAGE LOCAL 
          USES     ${ARG_USES} ${optional_uses}
          OPTIONAL ${ARG_OPTIONAL} )
      endif()
      if ( NOT "${LOCAL_MISSING}" STREQUAL "" )
        dd4hep_print ( "|++> ${tag} skipped. Missing dependency: ${missing}" )
      endif()
      #
      dd4hep_make_unique_list( pkg_incs VALUES ${LOCAL_INCLUDE_DIRS}   ${ARG_INCLUDE_DIRS} )
      dd4hep_make_unique_list( pkg_libs VALUES ${LOCAL_LINK_LIBRARIES} ${ARG_LINK_LIBRARIES} )
      dd4hep_make_unique_list( pkg_defs VALUES ${LOCAL_DEFINITIONS}    ${ARG_DEFINITIONS} )
      #
      file ( GLOB   sources ${ARG_SOURCES} )
      list ( APPEND sources ${optional_sources} )
      if ( NOT "${ARG_GENERATED}" STREQUAL "" )
        #
        # root-cint produces warnings of type 'unused-function' disable them on generated files
        foreach ( f in  ${ARG_GENERATED} )
          set_source_files_properties( ${f} PROPERTIES COMPILE_FLAGS -Wno-unused-function GENERATED TRUE )
        endforeach()
        list ( APPEND sources ${ARG_GENERATED} )
      endif()
      #
      #
      if ( NOT "${sources}" STREQUAL "" )
        dd4hep_make_unique_list ( sources  VALUES ${sources} )
        dd4hep_debug( "${tag} ${sources}")
        #
        dd4hep_include_directories( "${pkg_incs}" )
        add_definitions ( ${pkg_defs} )
        #
        add_library ( ${binary} SHARED ${sources} )
        target_link_libraries ( ${binary} ${pkg_libs} )
        if ( "${${pkg}_VERSION}" STREQUAL "" OR "${${pkg}_SOVERSION}" STREQUAL "" )
          dd4hep_fatal ( "BAD Package versions: VERSION[${pkg}_VERSION] ${${pkg}_VERSION} SOVERSION[${pkg}_SOVERSION] ${${pkg}_SOVERSION} " )
        endif()
        ##dd4hep_print ( "set_target_properties ( ${binary} PROPERTIES VERSION ${${pkg}_VERSION} SOVERSION ${${pkg}_SOVERSION})")
        set_target_properties ( ${binary} PROPERTIES VERSION ${${pkg}_VERSION} SOVERSION ${${pkg}_SOVERSION})
        #
        install ( TARGETS ${binary}  
	  LIBRARY DESTINATION lib 
	  RUNTIME DESTINATION bin)
        set ( building_binary "ON" )
      else()
        dd4hep_print ( "|++> ${tag} Skipped. No sources to be compiled [Use constraint]" )
      endif()
    endif()
  endif()
  set ( ${building} ${building_binary} PARENT_SCOPE )
endfunction(dd4hep_add_library)

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
  if ( "${enabled}" STREQUAL "OFF" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    dd4hep_add_library( ${library} building ${ARGN} PRINT ${tag} )
    if ( "${building}" STREQUAL "ON" )
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
  if ( "${enabled}" STREQUAL "OFF" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    dd4hep_add_library( ${library} building ${ARGN} PRINT ${tag} )
    if ( "${building}" STREQUAL "ON" )
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
#
#  dd4hep_add_plugin 
#
#  Arguments
#  ---------
#  binary         -> plugin name
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
function( dd4hep_add_plugin binary )
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Plugin[${pkg}] -> ${binary}" )
  if ( "${enabled}" STREQUAL "OFF" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    cmake_parse_arguments(ARG "" "" "SOURCES;GENERATED;LINK_LIBRARIES;INCLUDE_DIRS;USES;OPTIONAL;DEFINITIONS" ${ARGN})
    get_property(pkg_lib  GLOBAL PROPERTY ${PKG}_LIBRARIES )
    dd4hep_add_library( ${binary} building
      PRINT          ${tag}
      SOURCES        ${ARG_SOURCES}
      GENERATED      ${ARG_GENERATED}
      LINK_LIBRARIES ${ARG_LINK_LIBRARIES} ${pkg_lib}
      INCLUDE_DIRS   ${ARG_INCLUDE_DIRS} 
      USES           ${ARG_USES}
      OPTIONAL       "${ARG_OPTIONAL}"
      DEFINITIONS    ${ARG_DEFINITIONS} )
    #
    # Generate ROOTMAP if the plugin will be built:
    if ( "${building}" STREQUAL "ON" )
      dd4hep_generate_rootmap( ${binary} )
    endif()
  endif()
endfunction(dd4hep_add_plugin)

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
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Executable[${pkg}] -> ${binary}" )
  if ( "${enabled}" STREQUAL "OFF" )
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
          set_source_files_properties( ${f} PROPERTIES COMPILE_FLAGS -Wno-unused-function GENERATED TRUE )
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
#  dd4hep_add_dictionary
#
#
#  \author  M.Frank
#  \version 1.0
#
#---------------------------------------------------------------------------------------------------
function( dd4hep_add_dictionary dictionary )
  dd4hep_package_properties( pkg PKG enabled )
  set ( tag "Dictionary[${pkg}] -> ${dictionary}" )
  if ( "${enabled}" STREQUAL "OFF" )
    dd4hep_skipmsg ( "${tag} DISBALED -- package is not built!" )
  else()
    cmake_parse_arguments(ARG "" "" "SOURCES;EXCLUDE;LINKDEF;OPTIONS;OPTIONAL;INCLUDES" ${ARGN} )
    dd4hep_print ( "|++> ${tag} Building dictionary ..." ) 
    if("${ARG_LINKDEF}" STREQUAL "")
      set(ARG_LINKDEF "${CMAKE_SOURCE_DIR}/DDCore/include/ROOT/LinkDef.h")
    endif()
    #
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
    endif()
    if ( NOT "${LOCAL_MISSING}" STREQUAL "" )
      dd4hep_print ( "|++> ${tag} skipped. Missing dependency: ${missing}  --> FATAL ERROR. Build should fail!" )
    endif()
    #
    get_property(incs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY PACKAGE_INCLUDE_DIRS)
    get_property(defs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_DEFINITIONS)
    get_property(opts DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_OPTIONS)
    dd4hep_make_unique_list( incs VALUES ${LOCAL_INCLUDE_DIRS}   ${incs} )
    dd4hep_make_unique_list( defs VALUES ${LOCAL_DEFINITIONS}    ${defs} )
    #
    file( GLOB headers ${ARG_SOURCES} )
    file( GLOB excl_headers ${ARG_EXCLUDE} )
    foreach( f ${excl_headers} )
      list( REMOVE_ITEM headers ${f} )
      dd4hep_print ( "|++        exclude: ${f}" )
    endforeach()
    #
    set ( inc_dirs -I${CMAKE_CURRENT_SOURCE_DIR}/include )
    foreach ( inc ${ARG_INCLUDES} )
      set ( inc_dirs ${inc_dirs} -I${inc} )
    endforeach()
    foreach ( inc ${incs} )
      file ( GLOB inc ${inc} )
      set ( inc_dirs ${inc_dirs} -I${inc} )
    endforeach()
    #
    file ( GLOB linkdefs ${ARG_LINKDEF} )
    #
    set ( comp_defs )
    foreach ( def ${defs} )
      set ( comp_defs ${comp_defs} -D${def} )
    endforeach()
    #
    dd4hep_make_unique_list ( sources   VALUES ${headers} )
    dd4hep_make_unique_list ( linkdefs  VALUES ${linkdefs} )
    dd4hep_make_unique_list ( inc_dirs  VALUES ${inc_dirs} )
    dd4hep_make_unique_list ( comp_defs VALUES ${comp_defs} )
    #
    dd4hep_debug ( "${tag}  Linkdef: '${linkdefs}'" ) 
    dd4hep_debug ( "${tag}  Compile: '${comp_defs};${inc_dirs}'" ) 
    dd4hep_debug ( "${tag}  Files:   '${headers}'" ) 
    dd4hep_debug ( "${tag}  Unparsed:'${ARG_UNPARSED_ARGUMENTS}'" ) 
    dd4hep_debug ( "${tag}  Sources: '${CMAKE_CURRENT_SOURCE_DIR}'" ) 
    #
    add_custom_command(OUTPUT ${dictionary}.cxx
      COMMAND ${ROOT_rootcling_CMD} -cint -f ${dictionary}.cxx
      -s ${CMAKE_CURRENT_BINARY_DIR}/../lib/${dictionary} -inlineInputHeader -c -p ${ARG_OPTIONS} ${comp_defs} -std=c++${DD4HEP_USE_STDCXX} ${inc_dirs} ${headers} ${linkdefs}
      DEPENDS ${headers} ${linkdefs} )
    #  Install the binary to the destination directory
    #set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/../lib/${dictionary}_rdict.pcm PROPERTIES GENERATED TRUE )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../lib/${dictionary}_rdict.pcm DESTINATION lib)
    #set_source_files_properties( ${dictionary}.h ${dictionary}.cxx PROPERTIES GENERATED TRUE )
    endif()
endfunction()

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
  cmake_parse_arguments(ARG "BUILD_EXEC" "OUTPUT" "COMMAND;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED;REQUIRES" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  #foreach(opt ${ARG_REQUIRES} )
  #  dd4hep_unpack_package_opts ( USE ${opt} )
  #  dd4hep_find_package( ${USE_NAME} pkg_found 
  #    ARGS ${USE_REQUIRED} ${USE_COMPONENT} ${USE_COMPONENTS} 
  #    TYPE ${USE_TYPE} )
  #  if ( "${pkg_found}" STREQUAL "OFF" )
  #    set( missing ${missing} ${USE_NAME} )
  #    unset ( use_test )
  #  endif()
  #endforeach()

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
