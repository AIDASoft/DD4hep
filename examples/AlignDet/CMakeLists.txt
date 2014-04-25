cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
#---------------------------
set( PackageName AlignDet )
#---------------------------

project(${PackageName})

find_package( DD4hep ) 

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include 
                     ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${CMAKE_SOURCE_DIR}/DDSegmentation/include
		     ${ROOT_INCLUDE_DIR})

include(DD4hep_XML_setup)
file(GLOB sources src/*.cpp)

add_library(AlignDet SHARED ${sources})
target_link_libraries(AlignDet DD4hepCore ${ROOT_LIBRARIES} Rint Reflex)
dd4hep_generate_rootmap(AlignDet)

#--- install target-------------------------------------

install(TARGETS ${PackageName}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
install(DIRECTORY compact
  DESTINATION examples/${PackageName}
  PATTERN ".svn" EXCLUDE )

# to do: add corresponding uninstall...
#-------------------------------------------------------
