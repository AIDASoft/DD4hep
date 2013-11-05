cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

#---------------------------
set( PackageName ILDExDet )
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
set( ROOT_COMPONENT_LIBRARIES Geom Reflex)

#-------------------------------------------------------------
# add additional packages here


#-------------------------------------------------------------

include_directories( ${CMAKE_SOURCE_DIR}/include
  include
  ${DD4hep_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  )

file(GLOB sources src/*.cpp )
file(GLOB headers include/*.h)

include(DD4hep_XML_setup)

if(DD4HEP_USE_PYROOT)
  ROOT_GENERATE_DICTIONARY(G__ILDEx ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__ILDEx.cxx)
endif()

add_library(${PackageName} SHARED ${sources})

target_link_libraries(${PackageName} ${DD4hep_LIBRARIES}  ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} )

#---Rootmap generation--------------------------------------------------------------

if(APPLE)
  dd4hep_generate_rootmap_apple(${PackageName} )
else()
  dd4hep_generate_rootmap(${PackageName} )
endif()


#---- configure run environment ---------------
configure_file( ${DD4hep_ROOT}/cmake/thisdd4hep_package.sh.in  this${PackageName}.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/this${PackageName}.sh
  DESTINATION bin
  )


#--- install target-------------------------------------

install(TARGETS ${PackageName}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------