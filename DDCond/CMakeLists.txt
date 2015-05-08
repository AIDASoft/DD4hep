cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
#---------------------------
set( PackageName DDCond )

#---add additional packages here--------------------------------------------------
if(NOT DD4HEP_BUILD_ALL)
find_package( DD4hep ) 
endif()
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#---Includedirs ------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )

#---DD4hepCond library --------------------------------------------------------------
file(GLOB sources src/*.cpp)
add_library(DD4hepCond SHARED ${sources})
target_link_libraries(DD4hepCond DDCore ${ROOT_LIBRARIES})

if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
else()
  add_definitions(-DDD4HEP_USE_TINYXML)
endif()
SET_TARGET_PROPERTIES( DD4hepCond PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#---DD4hepCond rootmap --------------------------------------------------------------
dd4hep_generate_rootmap(DD4hepCond)

install(DIRECTORY include/DDCond
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepCond
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
