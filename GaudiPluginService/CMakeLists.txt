cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(GaudiPluginService)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#add_library(DD4hepGaudiPluginMgr SHARED src/PluginService.cpp src/capi_pluginservice.cpp interface/DD4hep.cpp)
add_library(DD4hepGaudiPluginMgr SHARED src/PluginService.cpp interface/DD4hep.cpp)
add_definitions(-DGaudi=DD4hep_Flavor)
if( APPLE ) 
  add_definitions( -DAPPLE) 
endif()
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-unused-function)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-deprecated)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-shadow)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
target_link_libraries(DD4hepGaudiPluginMgr ${CMAKE_DL_LIBS})
SET_TARGET_PROPERTIES(DD4hepGaudiPluginMgr PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

if( NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR ) 
  SET ( GaudiPluginService_LIBRARIES DD4hepGaudiPluginMgr  PARENT_SCOPE )
  SET ( GaudiPluginService_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
endif()

add_executable(listcomponents src/listcomponents.cpp ) #src/PluginService.cpp)
target_link_libraries(listcomponents DD4hepGaudiPluginMgr  ${CMAKE_DL_LIBS} )
target_compile_options(listcomponents PRIVATE -Wno-deprecated)

install(TARGETS listcomponents DD4hepGaudiPluginMgr
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib )
