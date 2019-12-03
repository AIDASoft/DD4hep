cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(GaudiPluginService)
find_package(ROOT REQUIRED)

add_library(DD4hepGaudiPluginMgr src/PluginServiceV1.cpp src/PluginServiceV2.cpp interface/DD4hepV1.cpp interface/DD4hepV2.cpp)
add_library(DD4hep::DD4hepGaudiPluginMgr ALIAS DD4hepGaudiPluginMgr)

target_compile_definitions(DD4hepGaudiPluginMgr PUBLIC Gaudi=DD4hep_Flavor)

target_include_directories(DD4hepGaudiPluginMgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )

if( APPLE ) 
  target_compile_definitions(DD4hepGaudiPluginMgr PUBLIC APPLE)
endif()
target_compile_features(DD4hepGaudiPluginMgr PRIVATE
  $<TARGET_PROPERTY:ROOT::Core,INTERFACE_COMPILE_FEATURES>
)
target_compile_options(DD4hepGaudiPluginMgr PRIVATE -Wno-shadow
  -Wno-unused
  -Wno-unused-parameter
  -Wno-deprecated
  -Wno-keyword-macro
  -Wno-return-type-c-linkage
  )

target_link_libraries(DD4hepGaudiPluginMgr PUBLIC ${FS_LIBRARIES} ${CMAKE_DL_LIBS})
SET_TARGET_PROPERTIES(DD4hepGaudiPluginMgr PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

add_executable(listcomponents src/listcomponents.cpp )
add_executable(DD4hep::listcomponents ALIAS listcomponents)
target_link_libraries(listcomponents PUBLIC DD4hepGaudiPluginMgr ${FS_LIBRARIES} ${CMAKE_DL_LIBS})
target_compile_options(listcomponents PRIVATE -Wno-deprecated)
target_compile_features(listcomponents PRIVATE
  $<TARGET_PROPERTY:ROOT::Core,INTERFACE_COMPILE_FEATURES>
)

INSTALL(TARGETS listcomponents DD4hepGaudiPluginMgr EXPORT DD4hep
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
