cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set ( CMAKE_MODULE_PATH      ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake ) 
#
include ( DD4hepBuild )
#

#------------- set the default installation directory to be the source directory

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET( CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH
       "install prefix path  - overwrite with -D CMAKE_INSTALL_PREFIX = ..."
       FORCE )
 MESSAGE(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX} - overwrite with -D CMAKE_INSTALL_PREFIX" )
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#
#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC"   OFF)
option(DD4HEP_USE_PYROOT  "Enable 'Detector Builders' based on PyROOT"    OFF)  # does not work (compile error)
option(DD4HEP_USE_GEANT4  "Enable the simulation part based on Geant4"    OFF)
option(DD4HEP_USE_GEAR    "Build gear wrapper for backward compatibility" OFF)
option(DD4HEP_USE_LCIO    "Build lcio extensions"    OFF)
option(BUILD_TESTING      "Enable and build tests"   ON)
option(DD4HEP_USE_CXX14   "Build DD4hep using c++14" OFF)
option(CMAKE_MACOSX_RPATH "Build with rpath on macos" ON)
#
#-----------------------------------------------------------------------------------------------------
#
#  project version (Major,minor,patch)
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  !! Use the same version numbers in DDSegmentation/CMakeLists.txt    !!
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# project version
SET( DD4hep_VERSION_MAJOR 0  )
SET( DD4hep_VERSION_MINOR 24 )
SET( DD4hep_VERSION_PATCH 0  )

dd4hep_set_version(DD4hep
  MAJOR ${DD4hep_VERSION_MAJOR}
  MINOR ${DD4hep_VERSION_MINOR}
  PATCH ${DD4hep_VERSION_PATCH} )

configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/Version.h.in"
  "${PROJECT_SOURCE_DIR}/DDCore/include/DD4hep/Version.h"
  )


dd4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )
#
# Include ROOT
#
find_package ( ROOT REQUIRED COMPONENTS Geom GenVector Eve Gui Graf3d RGL EG)
include(${ROOT_USE_FILE})
message ( STATUS "ROOT_VERSION: ${ROOT_VERSION}" )

#---------------------------------------------------------------------------------------------------
#fg: moved to here from DD4hep.cmake to not force CMAKE_CXX_FLAGS upon dependent packages
dd4hep_set_compiler_flags()

add_subdirectory ( GaudiPluginService)
#---DD4hep functions and macros -------------------------------------------------------
include(DD4hep)
include(DD4hepMacros) 

if( DEFINED DD4HEP_WITH_GEANT4 OR DEFINED DD4HEP_WITH_GEAR )
  dd4hep_print_cmake_options( OPTIONAL "Obsolete options: only the following are allowed:" ERROR 1 )
endif()

#-----------------------------
##checks for xercesc or not and sets up the include_directories
include(DD4hep_XML_setup)

#---Packages------------------------------------------------------------------------
add_subdirectory(DDSegmentation)
add_subdirectory(DDCore)
add_subdirectory(DDSurfaces)
add_subdirectory(DDRec)
add_subdirectory(DDDetectors)

if(DD4HEP_USE_GEANT4)

  #--- create a geant4 variables for the thisdd4hep.sh script
  find_package( Geant4 9.6 REQUIRED  gdml ui_all vis_all)
  
  include(${Geant4_USE_FILE} )   # this also takes care of geant 4 definitions and include dirs
  
  #----- Treatment for Geant4 10.2.1 or before this is obsolete only for backwards compatibility
  if(${Geant4_VERSION} VERSION_LESS 10.2.2)
    message( STATUS "!!! TREATING CLEHP IN LEGACY MODE !!!")
    set ( DD4HEP_USE_CLHEP "OFF" )  
    string(FIND "${Geant4_THIRD_PARTY_INCLUDE_DIRS}"  "CLHEP" CLHEP_POS )
    
    if( (NOT "${CLHEP_POS}" STREQUAL "-1") OR (NOT Geant4_builtin_clhep_FOUND)  )
      set( GEANT4_USE_CLHEP 1 )
      get_filename_component(Geant4_ROOT "${Geant4_INCLUDE_DIR}/../../" ABSOLUTE)
      #---- use FindCLHEP shipped with geant4 -----------
      set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${Geant4_ROOT}/cmake/Modules ) 
      
      if( CLHEP_DIR ) #---- use 'canonical' name -----
	set( CLHEP_ROOT_DIR ${CLHEP_DIR} )
      endif()
      
      find_package( CLHEP REQUIRED )
      include_directories( SYSTEM ${CLHEP_INCLUDE_DIRS} )
      set ( DD4HEP_USE_CLHEP ON )
      
      get_filename_component(CLHEP_LIBRARY_PATH "${CLHEP_LIBRARIES}" PATH)
      message( STATUS " *** geant4 was build with external CLHEP *** CLHEP_LIBRARY_PATH = ${CLHEP_LIBRARY_PATH}"  )
    else()
      set( GEANT4_USE_CLHEP 0 )
    endif()
    
    set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
    set(Geant4_LIBRARIES    ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
  endif()
  #----- End of treatment for Geant4 10.2.1 or before
endif()

#------- now configure DDG4 -------
add_subdirectory(DDG4)

add_subdirectory(DDCond)
add_subdirectory(DDAlign)
add_subdirectory(DDEve)
add_subdirectory(DDDB)

dd4hep_enable_tests( DDTest )
add_subdirectory(UtilityApps)

#set ( CMAKE_MODULE_PATH ${CMAKE_INSTALL_DIR}/cmake ${CMAKE_MODULE_PATH} ) 
#add_subdirectory(DDExamples)

#---Testing-------------------------------------------------------------------------

#---Documentation-------------------------------------------------------------------
OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )
IF( INSTALL_DOC )
  INSTALL( CODE "EXECUTE_PROCESS( COMMAND ${CMAKE_BUILD_TOOL} doc)" )
  include ( GlobalSVNRevision )
  include ( DD4hepDoxygen )
ENDIF()

#---Configuration-------------------------------------------------------------------
# ----for APPLE scripts have to set the DYLD_LIBRARY_PATH 
if( APPLE ) 
  set( USE_DYLD 1)
  set(CMAKE_MACOSX_RPATH 1)
endif()

configure_file(cmake/thisdd4hep.csh bin/thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  bin/thisdd4hep.sh @ONLY)
configure_file(cmake/run_test.sh    bin/run_test.sh @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/bin/thisdd4hep.csh ${CMAKE_BINARY_DIR}/bin/thisdd4hep.sh DESTINATION bin )
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/run_test.sh DESTINATION bin )

install(DIRECTORY cmake
  DESTINATION cmake
  FILES_MATCHING PATTERN "*.cmake" PATTERN "thisdd4hep_package.sh.in" PATTERN "run*.sh"
  PATTERN ".svn" EXCLUDE
  )

install(DIRECTORY examples
  DESTINATION share/DD4hep
  )



#-----------------------------------------------------------------------------------
display_std_variables()

##############################################################################
# generate and install following configuration files
dd4hep_generate_package_configuration_files( DD4hepConfig.cmake )
if(APPLE)
  SET ( ENV{DD4HEP_LIBRARY_PATH} $ENV{DYLD_LIBRARY_PATH} )
else()
  SET ( ENV{DD4HEP_LIBRARY_PATH} ${LD_LIBRARY_PATH} )
  SET ( DD4HEP_LIBRARY_PATH ${LD_LIBRARY_PATH} )
endif()

if(APPLE)
  fill_dd4hep_library_path()
  message(STATUS "DD4HEP_LIBRARY_PATH= $ENV{DD4HEP_LIBRARY_PATH}")
endif()
