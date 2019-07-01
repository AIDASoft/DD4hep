cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(GaudiPluginService)
add_library(DD4hepGaudiPluginMgr SHARED src/PluginServiceV1.cpp src/PluginServiceV2.cpp interface/DD4hepV1.cpp interface/DD4hepV2.cpp)
target_compile_definitions(DD4hepGaudiPluginMgr PUBLIC Gaudi=DD4hep_Flavor USE_BOOST_FILESYSTEM)

target_include_directories(DD4hepGaudiPluginMgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )

if( APPLE ) 
  target_compile_definitions(DD4hepGaudiPluginMgr APPLE)
endif()
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-shadow)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-unused)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-unused-parameter)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-deprecated)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-keyword-macro)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-return-type-c-linkage)

target_link_libraries(DD4hepGaudiPluginMgr ${CMAKE_DL_LIBS} Boost::filesystem Boost::system)
SET_TARGET_PROPERTIES(DD4hepGaudiPluginMgr PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

if( NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR ) 
  SET ( GaudiPluginService_LIBRARIES DD4hepGaudiPluginMgr  PARENT_SCOPE )
  SET ( GaudiPluginService_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
endif()

add_executable(listcomponents src/listcomponents.cpp )
target_link_libraries(listcomponents DD4hepGaudiPluginMgr  ${CMAKE_DL_LIBS})
target_compile_options(listcomponents PRIVATE -Wno-deprecated)

install(TARGETS listcomponents DD4hepGaudiPluginMgr
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
INSTALL(TARGETS listcomponents DD4hepGaudiPluginMgr EXPORT DD4hep DESTINATION lib)