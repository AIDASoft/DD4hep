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

  SET(COMPILER_FLAGS -Wall -Wextra -pedantic -Wshadow -Wformat-security -Wno-long-long -Wdeprecated -fdiagnostics-color=auto )

  FOREACH( FLAG ${COMPILER_FLAGS} )
    CHECK_CXX_COMPILER_FLAG( "${FLAG}" CXX_FLAG_WORKS_${FLAG} )
    IF( ${CXX_FLAG_WORKS_${FLAG}} )
      MESSAGE ( STATUS "Adding ${FLAG} to CXX_FLAGS" )
      SET ( CMAKE_CXX_FLAGS "${FLAG} ${CMAKE_CXX_FLAGS} ")
    ELSE()
      MESSAGE ( STATUS "NOT Adding ${FLAG} to CXX_FLAGS" )
    ENDIF()
  ENDFOREACH()

  CHECK_CXX_COMPILER_FLAG("-std=c++14" CXX_FLAG_WORKS_C++14)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX_FLAG_WORKS_C++11)
  CHECK_CXX_COMPILER_FLAG("-ftls-model=global-dynamic" CXX_FLAG_WORKS_FTLS_global-dynamic)

  if (NOT CXX_FLAG_WORKS_C++11)
    message( FATAL_ERROR "The provided compiler does not support the C++11 standard" )
  endif()

  if (NOT CXX_FLAG_WORKS_FTLS_global-dynamic)
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
