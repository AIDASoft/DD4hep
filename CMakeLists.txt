cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
set ( CMAKE_MODULE_PATH      ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake ) 
#
include ( DD4hepBuild )
#
#---Options-------------------------------------------------------------------------
option(DD4HEP_USE_XERCESC "Enable 'Detector Builders' based on XercesC"   OFF)
option(DD4HEP_USE_PYROOT  "Enable 'Detector Builders' based on PyROOT"    OFF)  # does not work (compile error)
option(DD4HEP_USE_GEANT4  "Enable the simulation part based on Geant4"    OFF)
option(DD4HEP_USE_GEAR    "Build gear wrapper for backward compatibility" OFF)
option(DD4HEP_USE_LCIO    "Build lcio extensions"    OFF)
option(BUILD_TESTING      "Enable and build tests"   ON)
option(DD4HEP_USE_CXX11   "Build DD4hep using c++11" OFF)
#
#-----------------------------------------------------------------------------------------------------
#
#  project version (Major,minor,patch)
#  Use the same version numbers in DDSegmentaion/CmakeLists.txt
#
dd4hep_set_version(DD4hep MAJOR 0 MINOR 14 PATCH 0)
dd4hep_configure_output( OUTPUT "${PROJECT_BINARY_DIR}" INSTALL "${CMAKE_INSTALL_PREFIX}" )
#
#
#  No Relex library present for ROOT 6
#
find_package ( ROOT REQUIRED COMPONENTS Geom GenVector )
if ( ${ROOT_VERSION_MAJOR} GREATER 5 )
  set ( DD4HEP_USE_CXX11 ON ) 
  message ( STATUS "ROOT_VERSION_MAJOR: ${ROOT_VERSION_MAJOR} --> Using GaudiPluginService..." )
  add_subdirectory ( GaudiPluginService)  
endif()
#---------------------------------------------------------------------------------------------------
#fg: moved to here from DD4hep.cmake to not force CMAKE_CXX_FLAGS upon dependent packages
if ( DD4HEP_USE_CXX11 )
  set ( CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wshadow -Wformat-security -Wno-long-long -Wdeprecated")
  add_definitions(-DDD4HEP_USE_CXX11)
else()
  set( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wshadow -Wformat-security -Wno-long-long -Wdeprecated")
endif()

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
  set ( DD4HEP_USE_CLHEP "OFF" )
  #--- create a geant4 variables for the thisdd4hep.sh script
  find_package( Geant4 9.6 REQUIRED  gdml ui_all vis_all)

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
    set ( DD4HEP_USE_CLHEP ON )

    get_filename_component(CLHEP_LIBRARY_PATH "${CLHEP_LIBRARIES}" PATH)
    message( STATUS " *** geant4 was build with external CLHEP *** CLHEP_LIBRARY_PATH = ${CLHEP_LIBRARY_PATH}"  )
  else()
    set( GEANT4_USE_CLHEP 0 )
  endif()

  set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS})
  set(Geant4_LIBRARIES    ${Geant4_LIBRARIES} ${CLHEP_LIBRARIES})
endif()

#------- now configure DDG4 -------
add_subdirectory(DDG4)

add_subdirectory(DDCond)
add_subdirectory(DDAlign)
add_subdirectory(DDEve)

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
endif()

configure_file(cmake/thisdd4hep.csh bin/thisdd4hep.csh @ONLY)
configure_file(cmake/thisdd4hep.sh  bin/thisdd4hep.sh @ONLY)
configure_file(cmake/run_test.sh    bin/run_test.sh @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/bin/thisdd4hep.csh ${CMAKE_BINARY_DIR}/bin/thisdd4hep.sh DESTINATION bin )
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/run_test.sh DESTINATION bin )

install(DIRECTORY cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.cmake" PATTERN "thisdd4hep_package.sh.in" PATTERN "run*.sh"
  PATTERN ".svn" EXCLUDE
  )

#-----------------------------------------------------------------------------------
display_std_variables()

##############################################################################
# generate and install following configuration files
generate_package_configuration_files( DD4hepConfig.cmake DD4hepConfigVersion.cmake DD4hepLibDeps.cmake )
