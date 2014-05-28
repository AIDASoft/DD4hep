cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

#---------------------------
set( PackageName DDReconstruction )
#---------------------------

project(${PackageName})

# project version
SET( ${PackageName}_VERSION_MAJOR 0 )
SET( ${PackageName}_VERSION_MINOR 0 )
SET( ${PackageName}_VERSION_PATCH 1 )

SET( ${PackageName}_VERSION "${${PackageName}_VERSION_MAJOR}.${${PackageName}_VERSION_MINOR}" )
SET( ${PackageName}_SOVERSION "${${PackageName}_VERSION_MAJOR}.${${PackageName}_VERSION_MINOR}" )

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${CMAKE_SOURCE_DIR}/cmake ) 
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#------------- set the default installation directory to be the source directory

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH  
    "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..." 
    FORCE )
  MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)


#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

find_package( DD4hep ) 

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#find_package( ROOT REQUIRED COMPONENTS Geom Reflex)
#set( ROOT_COMPONENT_LIBRARIES Geom Reflex)

#-------------------------------------------------------------
# add additional packages here


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )

file(GLOB sources src/*.cpp)

add_library(DDReconstruction SHARED ${sources})

target_link_libraries(DDReconstruction ${DD4hep_LIBRARIES} ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

add_executable(IDDecoderTest src/IDDecoderTest.cpp ${sources})

target_link_libraries(IDDecoderTest ${PackageName} ${DD4hep_LIBRARIES} DDReconstruction ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})

#SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
#SET_TARGET_PROPERTIES( DDReconstruction PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDReconstruction
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DDReconstruction
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
