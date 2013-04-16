cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

##find_package(LCIO REQUIRED) 

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
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/VersatileDiskRowLayoutSeg.cpp)
  list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/VersatileDiskRowLayout.cpp)
endif()

add_library(ILDEx SHARED ${sources})
target_link_libraries(ILDEx DD4hepCore  ${LCIO_LIBRARIES})
#---Rootmap generation--------------------------------------------------------------
dd4hep_generate_rootmap(ILDEx)
