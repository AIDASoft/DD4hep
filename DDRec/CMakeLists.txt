cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(DDRec)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DDSegmentation/include )

file(GLOB sources src/*.cpp)

add_library(DD4hepRec SHARED ${sources} )

target_link_libraries(DD4hepRec ${ROOT_LIBRARIES} DDSegmentation Geom Reflex ${libraries})

SET( CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
SET_TARGET_PROPERTIES( DD4hepRec PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#--- install target-------------------------------------
install(DIRECTORY include/DDRec
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DD4hepRec
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )

# to do: add corresponding uninstall...
#-------------------------------------------------------
