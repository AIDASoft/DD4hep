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


include ( CMakeParseArguments )

if(DD4hepBuild_included)
  RETURN()
endif()
message(STATUS "Including DD4hepBuild.cmake")
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

  SET(COMPILER_FLAGS -Wshadow -Wformat-security -Wno-long-long -Wdeprecated -fdiagnostics-color=auto -Wall -Wextra -pedantic)

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
      dd4hep_debug("|DDD> Adding ${FLAG} to CXX_FLAGS" )
      SET ( CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS} ")
    ELSE()
      dd4hep_debug("|DDD> NOT Adding ${FLAG} to CXX_FLAGS" )
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

 #---RPATH options-------------------------------------------------------------------------------
 #  When building, don't use the install RPATH already (but later on when installing)
 set(CMAKE_SKIP_BUILD_RPATH FALSE)         # don't skip the full RPATH for the build tree
 set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # use always the build RPATH for the build tree
 set(CMAKE_MACOSX_RPATH TRUE)              # use RPATH for MacOSX
 set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # point to directories outside the build tree to the install RPATH

 # Check whether to add RPATH to the installation (the build tree always has the RPATH enabled)
 if(APPLE)
   set(CMAKE_INSTALL_NAME_DIR "@rpath")
   set(CMAKE_INSTALL_RPATH "@loader_path/../lib")    # self relative LIBDIR
   # the RPATH to be used when installing, but only if it's not a system directory
   list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
   if("${isSystemDir}" STREQUAL "-1")
     set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
   endif("${isSystemDir}" STREQUAL "-1")
 elseif(DD4HEP_SET_RPATH)
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # install LIBDIR
   # the RPATH to be used when installing, but only if it's not a system directory
   list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
   if("${isSystemDir}" STREQUAL "-1")
     set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
   endif("${isSystemDir}" STREQUAL "-1")
 else()
   set(CMAKE_SKIP_INSTALL_RPATH TRUE)           # skip the full RPATH for the install tree
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
#---------------------------------------------------------------------------------------------------
#  dd4hep_install_dir: DEPRECATED, used in examples
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
  cmake_parse_arguments(ARG "" "OUTPUT" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( "${use_test}" STREQUAL "" )
    dd4hep_print ( "*** Will not build/execute test ${test_name}. Missing dependencies: ${missing} ")
  else()

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
      set_property( TEST t_${test_name} APPEND PROPERTY DEPENDS t_${_dep} )
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
  get_filename_component(ROOT_LIBDIR "${ROOT_Core_LIBRARY}" DIRECTORY)
  set( _libpath ${ROOT_LIBDIR} )
  if ( NOT "${Boost_LIBRARY_DIRS}" STREQUAL "" )
    list( APPEND _libpath "${Boost_LIBRARY_DIRS}" )
  else()
    dd4hep_print("|++> The boost library path cannot be determined. Problems maybe ahead.")
  endif()
  if ( ${DD4HEP_USE_GEANT4} )
    get_filename_component(Geant4_LIBDIR "${Geant4_DIR}" DIRECTORY)
    list( PREPEND _libpath "${Geant4_LIBDIR}" )
  endif()

  if(${DD4HEP_USE_LCIO})
    list( PREPEND _libpath "${LCIO_DIR}/lib" )
  endif()

  if(DD4HEP_USE_GEANT4 AND DD4HEP_USE_CLHEP)
    # Using external CLHEP (not Geant4's builtin)
    get_target_property(_loc CLHEP::CLHEP LOCATION)
    get_filename_component(_dir "${_loc}" DIRECTORY)
    list( PREPEND _libpath "${_dir}" )
  endif()

  if(XERCESC_LIB_DIR)
    list( PREPEND _libpath "${XERCESC_LIB_DIR}" )
  endif()

  list( PREPEND _libpath "${CMAKE_BINARY_DIR}/lib" )
  list( REMOVE_DUPLICATES _libpath )
  # Exclude common system library directories. These are automatically searched anyway,
  # and prepending them before the locations of the externals may cause trouble.
  list( REMOVE_ITEM _libpath "/usr/lib" "/usr/local/lib" )
  string( REGEX REPLACE ";" ":" _libpath "${_libpath}" )
  set( ENV{DD4HEP_LIBRARY_PATH} "${_libpath}" )
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
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir})

  add_custom_command(OUTPUT ${dictionary}.cxx ${output_dir}/${dictionary}_rdict.pcm
    COMMAND ${ROOT_rootcling_CMD}
    ARGS -f ${dictionary}.cxx -s ${output_dir}/${dictionary} -inlineInputHeader
    ${ARG_OPTIONS}
   "$<$<BOOL:$<JOIN:${comp_defs},>>:-D$<JOIN:${comp_defs},;-D>>"
   "$<$<BOOL:$<JOIN:${inc_dirs},>>:-I$<JOIN:${inc_dirs},;-I>>"
   "$<JOIN:${headers},;>" "$<JOIN:${linkdefs},;>"

   DEPENDS ${headers} ${linkdefs}
   COMMAND_EXPAND_LISTS
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
  dd4hep_print("|++> Building Plugin: ${binary}")
  dd4hep_debug("|++++> SOURCES ${SOURCES}")
  dd4hep_debug("|++++> USES ${ARG_USES}")
  dd4hep_debug("|++++> INCLUDES ${ARG_INCLUDES}")
  dd4hep_debug("|++++> DEFINITIONS ${ARG_DEFINITIONS}")
  IF(NOT DEFINED BUILD_SHARED_LIBS)
    SET(STATIC_OR_SHARED SHARED)
  ENDIF()
  add_library(${binary} ${STATIC_OR_SHARED} ${SOURCES} ${ARG_GENERATED})
  target_link_libraries(${binary} PUBLIC ${ARG_USES})
  target_include_directories(${binary} PUBLIC ${ARG_INCLUDES})
  target_compile_definitions(${binary} PUBLIC ${ARG_DEFINITIONS})
  IF(BUILD_SHARED_LIBS OR NOT DEFINED BUILD_SHARED_LIBS)
    dd4hep_generate_rootmap(${binary})
  ENDIF()
  if(NOT ${ARG_NOINSTALL})
    set(install_destination "lib")
    if(CMAKE_INSTALL_LIBDIR)
      set(install_destination ${CMAKE_INSTALL_LIBDIR})
    endif()
    install(TARGETS ${binary}
      ARCHIVE DESTINATION ${install_destination}
      LIBRARY DESTINATION ${install_destination}
      )
  endif()
endfunction(dd4hep_add_plugin)


#
# Macro to set up ROOT:: targets so that we can use the same code for root 6.8 and for root 6.10 and beyond
# This is called in dd4hep, and in DD4hepConfig
#
macro(DD4HEP_SETUP_ROOT_TARGETS)

  #Check if Python version detected matches the version used to build ROOT
  SET(Python_FIND_FRAMEWORK LAST)
  IF((TARGET ROOT::PyROOT OR TARGET ROOT::ROOTTPython) AND ${ROOT_VERSION} VERSION_GREATER_EQUAL 6.19)
    # cmake 3.17.1 and 3.17.2 don't include python patch level in PYTHON_VERSION
    IF(CMAKE_VERSION VERSION_EQUAL 3.17.1 OR CMAKE_VERSION VERSION_EQUAL 3.17.2)
      string(REGEX MATCH [23]\.[0-9]+ REQUIRE_PYTHON_VERSION ${ROOT_PYTHON_VERSION})
    ELSE()
      SET(REQUIRE_PYTHON_VERSION ${ROOT_PYTHON_VERSION})
    ENDIF()
    dd4hep_debug("D++> Python version used for building ROOT ${ROOT_PYTHON_VERSION}" )
    dd4hep_debug("D++> Required python version ${REQUIRE_PYTHON_VERSION}")
    FIND_PACKAGE(Python ${REQUIRE_PYTHON_VERSION} EXACT REQUIRED COMPONENTS Development)
  ELSE()
    FIND_PACKAGE(Python COMPONENTS Development)
  ENDIF()

  SET(DD4HEP_PYTHON_INSTALL_DIR lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages)

  # root python changes target name in 6.22
  IF(TARGET ROOT::PyROOT)
    SET(DD4HEP_ROOT_PYTHON ROOT::PyROOT)
  ELSEIF(TARGET ROOT::ROOTTPython)
    # New "pyroot" in 6.22
    SET(DD4HEP_ROOT_PYTHON ROOT::ROOTTPython)
  ENDIF()
  #ROOT CXX Flags are a string with quotes, not a list, so we need to convert to a list...
  string(REPLACE " " ";" DD4HEP_ROOT_CXX_FLAGS ${ROOT_CXX_FLAGS})

  IF(NOT TARGET ROOT::Core)
    #in ROOT before 6.10 there is no ROOT namespace, so we create ROOT::Core ourselves
    ADD_LIBRARY(ROOT::Core INTERFACE IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(ROOT::Core
      PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${DD4HEP_ROOT_CXX_FLAGS}"
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
  ELSEIF(${ROOT_VERSION} VERSION_GREATER_EQUAL 6.12 AND ${ROOT_VERSION} VERSION_LESS 6.14)
    # Root 6.12 exports ROOT::Core, but does not assign include directories to the target
    SET_TARGET_PROPERTIES(ROOT::Core
      PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${DD4HEP_ROOT_CXX_FLAGS}"
      INTERFACE_INCLUDE_DIRECTORIES ${ROOT_INCLUDE_DIRS}
      )

  ENDIF()

  dd4hep_debug("ROOT Libraries ${ROOT_LIBRARIES}")
  dd4hep_debug("ROOT CXX_FLAGS ${DD4HEP_ROOT_CXX_FLAGS}")
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
  # stdc++fs needed in gcc8, no lib for gcc9.1, c++fs for llvm
  FOREACH(FS_LIB_NAME stdc++fs "" c++fs )
    dd4hep_debug("|++++> linking against ${FS_LIB_NAME}")
    try_compile(HAVE_FILESYSTEM ${CMAKE_BINARY_DIR}/try ${DD4hep_DIR}/cmake/TryFileSystem.cpp
      CXX_STANDARD ${CMAKE_CXX_STANDARD}
      CXX_EXTENSIONS False
      OUTPUT_VARIABLE HAVE_FS_OUTPUT
      LINK_LIBRARIES ${FS_LIB_NAME}
      )
    dd4hep_debug("|++++> ${HAVE_FS_OUTPUT}")
    IF(HAVE_FILESYSTEM)
      dd4hep_print("|++> Compiler supports filesystem when linking against ${FS_LIB_NAME}")
      SET(FS_LIBRARIES ${FS_LIB_NAME})
      BREAK()
    ENDIF()
    dd4hep_debug("|++++> Compiler not compatible when linking against ${FS_LIB_NAME}")
  ENDFOREACH()

  IF(NOT HAVE_FILESYSTEM)
    dd4hep_print("|++> Compiler does not have filesystem support, falling  back to Boost::filesystem")
    FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS filesystem system)
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
      ADD_LIBRARY(CLHEP::CLHEP INTERFACE IMPORTED GLOBAL)
      SET_TARGET_PROPERTIES(CLHEP::CLHEP
        PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Geant4_INCLUDE_DIRS}"
      )
      TARGET_LINK_LIBRARIES(CLHEP::CLHEP INTERFACE Geant4::G4clhep)

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

    #Geant4_DEFINITIONS already include -D, we have to get rid of that so we can join things when creating dictionaries
    SET(G4_DEF_TEMP "")
    foreach(def ${Geant4_DEFINITIONS})
      string(REPLACE "-D" "" def ${def})
      LIST(APPEND G4_DEF_TEMP ${def})
    endforeach()
    SET(DD4HEP_Geant4_DEFINITIONS ${G4_DEF_TEMP})
    UNSET(G4_DEF_TEMP)

    ADD_LIBRARY(Geant4::Interface INTERFACE IMPORTED GLOBAL)

    SET_TARGET_PROPERTIES(Geant4::Interface
      PROPERTIES
      INTERFACE_COMPILE_OPTIONS "${Geant4_Flags}"
      INTERFACE_COMPILE_DEFINITIONS "${DD4HEP_Geant4_DEFINITIONS}"
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
