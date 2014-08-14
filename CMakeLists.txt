cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(DD4hep)

# project version
#
#  Use the same version numbers in DDSegmentaion/CmakeLists.txt
#
SET( DD4hep_VERSION_MAJOR 0 )
SET( DD4hep_VERSION_MINOR 9 )
SET( DD4hep_VERSION_PATCH 0 )

SET( DD4hep_VERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )
SET( DD4hep_SOVERSION "${DD4hep_VERSION_MAJOR}.${DD4hep_VERSION_MINOR}" )

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

#---Options-------------------------------------------------------------------------

option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC" OFF)
option(DD4HEP_USE_PYROOT "Enable 'Detector Builders' based on PyROOT" OFF)  # does not work (compile error)
option(DD4HEP_USE_GEANT4 "Enable the simulation part based on Geant4" OFF)
option(DD4HEP_USE_GEAR "Build gear wrapper for backward compatibility" OFF)
option(DD4HEP_USE_LCIO "Build lcio extensions" OFF)

#For backward compatibility for now
IF( DEFINED DD4HEP_WITH_GEANT4 )
  SET ( DD4HEP_USE_GEANT4 ${DD4HEP_WITH_GEANT4} )
ENDIF()

IF( DEFINED DD4HEP_WITH_GEAR )
  SET ( DD4HEP_USE_GEAR ${DD4HEP_WITH_GEAR} )
ENDIF()


#---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "One of: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

#---DD4hep functions and macros -------------------------------------------------------
include(DD4hep)
include( DD4hepMacros ) 

#-----------------------------

find_package(ROOT REQUIRED)
include_directories( SYSTEM ${ROOT_INCLUDE_DIR} )

##checks for xercesc or not and sets up the include_directories
include(DD4hep_XML_setup)

if(DD4HEP_USE_BOOST)
  find_package( Boost REQUIRED ) 
  include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
  add_definitions( -DDD4HEP_USE_BOOST )
endif()

if(DD4HEP_USE_LCIO)
  find_package( LCIO REQUIRED )
  include_directories( SYSTEM ${LCIO_INCLUDE_DIRS} )
endif()

#---Testing-------------------------------------------------------------------------
if(BUILD_TESTING)

  include(CTest)
  enable_testing()
  add_subdirectory(DDTest)

endif(BUILD_TESTING)

#---Documentation-------------------------------------------------------------------

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
IF( INSTALL_DOC )
  INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )
  include(GlobalSVNRevision)
  include(DD4hepDoxygen)
ENDIF()




#---Packages------------------------------------------------------------------------

##------ don't allow to use a pre-built DDSegmentation  ----------------------------
#find_package( DDSegmentation QUIET )
#if( NOT DDSegmentation_FOUND ) 
#  add_subdirectory(DDSegmentation)
#endif()

add_subdirectory(DDSegmentation)
add_subdirectory(DDCore)
add_subdirectory(DDSurfaces)
add_subdirectory(DDRec)
add_subdirectory(DDSense)

if(DD4HEP_USE_GEANT4)

  #--- create a geant4 variables for the thisdd4hep.sh script
  find_package( Geant4 REQUIRED  gdml ui_all vis_all)

  include(${Geant4_USE_FILE})   # this also takes care of geant 4 definitions and include dirs

  #----- if geant4 is build with external CLHEP we also need to export this fact

  string(FIND "${Geant4_THIRD_PARTY_INCLUDE_DIRS}"  "CLHEP" CLHEP_POS )
 
  if( NOT "${CLHEP_POS}" STREQUAL "-1"  )

    set( GEANT4_USE_CLHEP 1 )

    get_filename_component(Geant4_ROOT "${Geant4_INCLUDE_DIR}/../../" ABSOLUTE)

    #---- use FindCLHEP shipped with geant4 -----------
    set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${Geant4_ROOT}/cmake/Modules ) 
    
    if( CLHEP_DIR ) #---- use 'canonical' name -----
      set( CLHEP_ROOT_DIR ${CLHEP_DIR} )
    endif()

    find_package( CLHEP REQUIRED )
    include_directories( SYSTEM ${CLHEP_INCLUDE_DIRS} )

    get_filename_component(CLHEP_LIBRARY_PATH "${CLHEP_LIBRARIES}" PATH)
    
    message( STATUS " *** geant4 was build with external CLHEP *** CLHEP_LIBRARY_PATH = ${CLHEP_LIBRARY_PATH}"  )
  else()
    set( GEANT4_USE_CLHEP 0 )
  endif()

  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
  set(Geant4_LIBRARIES ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})


#------- now configure DDG4 -------
  add_subdirectory(DDG4)

endif()


add_subdirectory(UtilityApps)



#---Configuration-------------------------------------------------------------------
# ----for APPLE scripts have to set the DYLD_LIBRARY_PATH 
if( APPLE ) 
  set( USE_DYLD 1)
endif()

configure_file(cmake/thisdd4hep.csh thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  thisdd4hep.sh @ONLY)
configure_file(cmake/run_test.sh run_test.sh @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/thisdd4hep.csh ${CMAKE_BINARY_DIR}/thisdd4hep.sh
  DESTINATION bin
  )
install(PROGRAMS ${CMAKE_BINARY_DIR}/run_test.sh
  DESTINATION bin
  )

install(DIRECTORY cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.cmake" PATTERN "thisdd4hep_package.sh.in" PATTERN "run*.sh"
  PATTERN ".svn" EXCLUDE
  )

#-----------------------------------------------------------------------------------
display_std_variables()

IF( DEFINED DD4HEP_WITH_GEANT4)
  MESSAGE ( WARNING "DD4HEP_WITH_GEANT4 is deprecated use DD4HEP_USE_GEANT4")
ENDIF()

IF( DEFINED DD4HEP_WITH_GEAR )
  MESSAGE (WARNING "DD4HEP_WITH_GEAR is deprecated use DD4HEP_USE_GEAR")
ENDIF()


##############################################################################
# generate and install following configuration files

generate_package_configuration_files( DD4hepConfig.cmake DD4hepConfigVersion.cmake DD4hepLibDeps.cmake )
