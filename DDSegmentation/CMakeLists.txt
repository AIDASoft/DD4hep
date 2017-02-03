cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(DDSegmentation)
 #fg: version number must be the same as DD4hep !!
set(DDSegmentation_MAJOR_VERSION 0)
set(DDSegmentation_MINOR_VERSION 20)
set(DDSegmentation_PATCH_VERSION 0)
set(DDSegmentation_VERSION "${DDSegmentation_MAJOR_VERSION}.${DDSegmentation_MINOR_VERSION}" )
set(DDSegmentation_SOVERSION "${DDSegmentation_MAJOR_VERSION}.${DDSegmentation_MINOR_VERSION}")

#------------- set the default installation directory to be the source directory

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
    "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..." 
    FORCE )
  MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

file(GLOB sources src/*.cpp)

add_library(DDSegmentation SHARED ${sources})

SET_TARGET_PROPERTIES( DDSegmentation PROPERTIES VERSION ${DDSegmentation_VERSION} SOVERSION ${DDSegmentation_SOVERSION})

#if DDSegmentation is build as part of dd4hep
if( NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR ) 
  SET ( DDSegmentation_LIBRARIES DDSegmentation  PARENT_SCOPE )
  SET ( DDSegmentation_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/DDSegmentation/include PARENT_SCOPE )
  SET ( DDSEGMENTATION_LIBRARIES DDSegmentation  PARENT_SCOPE )
  SET ( DDSEGMENTATION_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/DDSegmentation/include PARENT_SCOPE )
endif()

install(DIRECTORY include/DDSegmentation
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

IF(SYSTEM_INSTALL)
  install(TARGETS DDSegmentation
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
ELSE()
  install(TARGETS DDSegmentation
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
ENDIF() 
# to do: add corresponding uninstall...
#-------------------------------------------------------

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
#export(PACKAGE DDSegmentation)
 
# Create the DDSegmentationConfig.cmake and DDSegmentationConfigVersion files
configure_file(cmake/DDSegmentationConfig.cmake.in ${PROJECT_BINARY_DIR}/DDSegmentationConfig.cmake)

IF(SYSTEM_INSTALL)
  INSTALL(FILES ${PROJECT_BINARY_DIR}/DDSegmentationConfig.cmake
    DESTINATION  ${CMAKE_INSTALL_LIBDIR}/DD4hep )
ELSE()
  INSTALL(FILES ${PROJECT_BINARY_DIR}/DDSegmentationConfig.cmake
    DESTINATION  ${CMAKE_INSTALL_PREFIX} )
ENDIF()

#install(FILES ${PROJECT_SOURCE_DIR}/DDSegmentationConfig.cmake DESTINATION . )
