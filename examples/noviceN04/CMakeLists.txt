cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

#---------------------------
set( PackageName noviceN04 )
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

find_package(Geant4 REQUIRED ui_all vis_all)

INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs


#-------------------------------------------------------------

include_directories( ${CMAKE_SOURCE_DIR}/include
  include 
  ${DD4hep_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  ${Geant4_INCLUDE_DIRS}
#  ${LCIO_INCLUDE_DIRS}
  )

file(GLOB sources src/*.cc src/*.cpp)
file(GLOB headers include/*.hh)

include(DD4hep_XML_setup)

if(DD4HEP_USE_PYROOT)
  ROOT_GENERATE_DICTIONARY(G__ILDEx ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__ILDEx.cxx)
endif()

add_library(${PackageName} SHARED ${sources})

target_link_libraries(${PackageName} ${DD4hep_LIBRARIES}  
  ${ROOT_LIBRARIES} 
  ${ROOT_COMPONENT_LIBRARIES} 
  ${Geant4_LIBRARIES} 
#  ${LCIO_LIBRARIES}
  )

add_executable( exampleN04 exampleN04.cc )

target_link_libraries(exampleN04 ${PackageName}  )


#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)
  include(CTest)
  enable_testing()
endif(BUILD_TESTING)

configure_file( ${DD4hep_ROOT}/cmake/run_test_package.sh run_test_${PackageName}.sh @ONLY)
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/run_test_${PackageName}.sh
  DESTINATION bin )

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


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build N04. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEN04_SCRIPTS
  exampleN04.EMtest.in
  exampleN04.EMtest.large_N.in
  exampleN04.EMtest.out
  exampleN04.in
  exampleN04.out
  vis.mac
  pythia_event.data
  )

foreach(_script ${EXAMPLEN04_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#--- install target-------------------------------------

install(TARGETS ${PackageName} exampleN04
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )


# to do: add corresponding uninstall...
#-------------------------------------------------------


####################################
#
#  Add Tests foir noviceN04 here
# 
####################################

SET( test_name "test_noviceN04" )
ADD_TEST( t_${test_name} "${CMAKE_INSTALL_PREFIX}/bin/run_test_${PackageName}.sh" 
  exampleN04 file:${CMAKE_CURRENT_SOURCE_DIR}/compact/N04.xml file:${CMAKE_CURRENT_SOURCE_DIR}/compact/geant4.xml ${CMAKE_CURRENT_SOURCE_DIR}/exampleN04.in )

#----- check for typical expected output:
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION  " Number of events processed : 3" )
