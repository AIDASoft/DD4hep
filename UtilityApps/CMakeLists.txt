cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
                     ${CMAKE_SOURCE_DIR}/DDSegmentation/include)

#-----------------------------------------------------------------------------------
add_executable(geoDisplay src/display.cpp)
target_link_libraries(geoDisplay DD4hepCore)
#-----------------------------------------------------------------------------------
add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DD4hepCore)
#-----------------------------------------------------------------------------------
add_executable(geoPluginRun src/plugin_runner.cpp)
target_link_libraries(geoPluginRun DD4hepCore)

#--- install target-------------------------------------

install(TARGETS geoDisplay geoConverter geoPluginRun
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------