cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

project(GaudiPluginService)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(GaudiPluginService SHARED src/PluginService.cpp src/capi_pluginservice.cpp)
target_link_libraries(GaudiPluginService ${CMAKE_DL_LIBS})
SET_TARGET_PROPERTIES(GaudiPluginService PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

if( NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR ) 
  SET ( GaudiPluginService_LIBRARIES GaudiPluginService  PARENT_SCOPE )
  SET ( GaudiPluginService_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
endif()

add_executable(listcomponents src/listcomponents.cpp src/PluginService.cpp)
target_link_libraries(listcomponents ${CMAKE_DL_LIBS})

#--- install target-------------------------------------
install(DIRECTORY Gaudi
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS listcomponents GaudiPluginService
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib )
