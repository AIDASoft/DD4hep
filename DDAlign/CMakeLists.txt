cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
#---------------------------
set( PackageName DDAlign )

#---add additional packages here--------------------------------------------------
find_package( DD4hep ) 
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  ${DD4hep_ROOT}/cmake ) 
include( DD4hep )

find_package( ROOT REQUIRED )
#---Includedirs ------------------------------------------------------------------
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${DD4hep_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR} )

#---DD4hepAlign library --------------------------------------------------------------
file(GLOB sources src/*.cpp)
add_library(DD4hepAlign SHARED ${sources})
target_link_libraries(DD4hepAlign ${DD4hep_LIBRARIES} DD4hepCore ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
else()
  add_definitions(-DDD4HEP_USE_TINYXML)
endif()
SET_TARGET_PROPERTIES( DD4hepAlign PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})
#---DD4hepAlign rootmap --------------------------------------------------------------
dd4hep_generate_rootmap(DD4hepAlign)

install(DIRECTORY include/DDAlign
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepAlign
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
