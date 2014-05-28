cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

file(GLOB sources src/*.cpp)

add_library(DDSegmentation SHARED ${sources})

SET_TARGET_PROPERTIES( DDSegmentation PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

install(DIRECTORY include/DDSegmentation
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DDSegmentation
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------