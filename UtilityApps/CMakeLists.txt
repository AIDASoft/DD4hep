cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)

include_directories( ${CMAKE_SOURCE_DIR}/DDCore/include 
                     ${ROOT_INCLUDE_DIR}
                     ${CMAKE_SOURCE_DIR}/DDSegmentation/include
                     ${CMAKE_SOURCE_DIR}/DDSurfaces/include
                     ${CMAKE_SOURCE_DIR}/DDRec/include )

#-----------------------------------------------------------------------------------
add_executable(geoDisplay src/display.cpp)
target_link_libraries(geoDisplay DD4hepCore )
#-----------------------------------------------------------------------------------
add_executable(geoConverter src/converter.cpp)
target_link_libraries(geoConverter DD4hepCore)
#-----------------------------------------------------------------------------------
add_executable(geoPluginRun src/plugin_runner.cpp)
target_link_libraries(geoPluginRun DD4hepCore)
#-----------------------------------------------------------------------------------
add_executable(teveDisplay src/teve_display.cpp)
target_link_libraries(teveDisplay DD4hepCore ${ROOT_EVE_LIBRARIES} DD4hepRec )

#--- install target-------------------------------------

install(TARGETS geoDisplay geoConverter geoPluginRun teveDisplay
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
