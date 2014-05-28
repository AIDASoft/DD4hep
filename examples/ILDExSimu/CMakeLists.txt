cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

#---------------------------
set( PackageName ILDExSimu )
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
##---Handle the case CLHEP is not included in Geant4------------------------------
#if(NOT Geant4_clhep_FOUND)
#  find_package(CLHEP REQUIRED)
#  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
#  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
#endif()
INCLUDE(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs


find_package(LCIO REQUIRED) 
# add all lcio library pathes to External_LIBRARY_DIRS for the environment script
foreach(lp ${LCIO_LIBRARY_DIRS})
  set( External_LIBRARY_DIRS "${External_LIBRARY_DIRS} ${lp}")
endforeach(lp) 


#-------------------------------------------------------------

include_directories( ${CMAKE_SOURCE_DIR}/include
  include
  ${DD4hep_INCLUDE_DIRS}
  ${ROOT_INCLUDE_DIR}
  ${Geant4_INCLUDE_DIRS}
  ${LCIO_INCLUDE_DIRS}
  )

file(GLOB sources src/*.cpp )
file(GLOB headers include/*.h)

#include(DD4hep_XML_setup)

if(DD4HEP_USE_PYROOT)
  ROOT_GENERATE_DICTIONARY(G__ILDEx ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__ILDEx.cxx)
endif()

add_library(${PackageName} SHARED ${sources})


#MESSAGE( STATUS "********* DD4hep_LIBRARIES : ${DD4hep_LIBRARIES}") 

target_link_libraries(${PackageName} ${DD4hep_LIBRARIES}  
  ${ROOT_LIBRARIES} 
  ${ROOT_COMPONENT_LIBRARIES} 
  ${Geant4_LIBRARIES} 
  ${LCIO_LIBRARIES}
  )

add_executable(ildex_simu ILDExSimu.cpp )

target_link_libraries(ildex_simu  ${PackageName} )
# ${DD4hep_LIBRARIES}  
#  ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} 
#  ${GEAR_LIBRARIES}
#  ${LCIO_LIBRARIES}
#  )

add_executable( test_surfaces src/test_surfaces.cc)

target_link_libraries(test_surfaces  ${PackageName} )
#${DD4hep_LIBRARIES}  
#  ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} 
#  ${GEAR_LIBRARIES}
#  ${LCIO_LIBRARIES}
#  )

#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)
  include(CTest)
  enable_testing()
endif(BUILD_TESTING)

configure_file( ${DD4hep_ROOT}/cmake/run_test_package.sh run_test_${PackageName}.sh @ONLY)
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/run_test_${PackageName}.sh
  DESTINATION bin )

#---Rootmap generation--------------------------------------------------------------
# fg: not needed here (?)
#if(APPLE)
#  dd4hep_generate_rootmap_apple(${PackageName} )
#else()
#  dd4hep_generate_rootmap(${PackageName} )
#endif()


#---- configure run environment ---------------
configure_file( ${DD4hep_ROOT}/cmake/thisdd4hep_package.sh.in  this${PackageName}.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/this${PackageName}.sh
  DESTINATION bin
  )


#--- install target-------------------------------------

install(TARGETS ${PackageName} ildex_simu test_surfaces
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )


# to do: add corresponding uninstall...
#-------------------------------------------------------

####################################
#
#  Add Tests foir ILDExSimu here
# 
####################################

SET( test_name "test_sim_ildexdet" )
ADD_TEST( t_${test_name} "${CMAKE_INSTALL_PREFIX}/bin/run_test_${PackageName}.sh" 
  ildex_simu file:${CMAKE_CURRENT_SOURCE_DIR}/../ILDExDet/compact/ILDEx.xml file:${CMAKE_CURRENT_SOURCE_DIR}/../ILDExDet/compact/geant4.xml ${CMAKE_CURRENT_SOURCE_DIR}/run1.mac )

#----- check for typical expected output:
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION  " Begin of event: 2" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION  "-End of Run-" )

SET( test_name "test_surfaces" )
ADD_TEST( t_${test_name} "${CMAKE_INSTALL_PREFIX}/bin/run_test_${PackageName}.sh" 
  test_surfaces file:${CMAKE_CURRENT_SOURCE_DIR}/../ILDExDet/compact/ILDEx.xml ILDExSimu.slcio)
#----- check for typical expected output:
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "TEST_PASSED" )
SET_TESTS_PROPERTIES( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED" )

