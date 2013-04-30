cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)


#----- check if LCIO is available
find_package(LCIO QUIET) 

if( NOT LCIO_FOUND ) # downlaod and build LCIO

  INCLUDE( ExternalProject )

  set( lcio_download_url SVN_REPOSITORY "svn://svn.freehep.org/lcio/trunk" )
  
  set( lcio_cmake_args
    "-DINSTALL_JAR=OFF"
    "-DBUILD_LCIO_EXAMPLES=OFF"
    "-DBUILD_ROOTDICT=OFF"
    "-DBUILD_F77_TESTJOBS=OFF"
    )

  ExternalProject_Add( LCIO
    ${lcio_download_url}
    CMAKE_ARGS ${lcio_cmake_args}
    PREFIX ${DD4hep_SOURCE_DIR}/dependencies
    )

  MESSAGE( STATUS "===============  Adding LCIO as external project  ====================" ) 
  MESSAGE( STATUS "   to use an existing LCIO installation please specify -D LCIO_DIR=path_to_lcio !! " )
  MESSAGE( STATUS "======================================================================" ) 
  
  set(LCIO_INCLUDE_DIRS "${DD4hep_SOURCE_DIR}/dependencies/src/LCIO/include" )
  
  set(LCIO_LIBRARIES 
    "${DD4hep_SOURCE_DIR}/dependencies/src/LCIO/lib/${CMAKE_SHARED_LIBRARY_PREFIX}sio${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${DD4hep_SOURCE_DIR}/dependencies/src/LCIO/lib/${CMAKE_SHARED_LIBRARY_PREFIX}lcio${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )

endif()
#-------------- lcio -------------------


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/DDCore/include 
  ${ROOT_INCLUDE_DIR}
  ${LCIO_INCLUDE_DIRS})

file(GLOB sources src/*.cpp src/compact/*.cpp)
file(GLOB headers include/*.h)

include(DD4hep_XML_setup)

if(DD4HEP_USE_PYROOT)
  ROOT_GENERATE_DICTIONARY(G__ILDEx ${headers} LINKDEF include/ROOT/LinkDef.h)
  list(APPEND sources G__ILDEx.cxx)
#fg: removing these causes undefined symbols in libILdEx.so/.dylib
#  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/VersatileDiskRowLayoutSeg.cpp)
#  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/VersatileDiskRowLayout.cpp)
endif()

add_library(ILDEx SHARED ${sources})
target_link_libraries(ILDEx DD4hepCore  ${LCIO_LIBRARIES})
#---Rootmap generation--------------------------------------------------------------
dd4hep_generate_rootmap(ILDEx)
