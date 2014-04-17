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

root_generate_dictionary( G__teve src/EvNavHandler.h LINKDEF src/LinkDef.h)
if(DD4HEP_USE_LCIO)
  find_package(LCIO REQUIRED)
  include_directories( ${LCIO_INCLUDE_DIRS} )
  add_executable(teveDisplay src/teve_display.cpp G__teve.cxx src/next_event_lcio.cpp)
else()
  add_executable(teveDisplay src/teve_display.cpp G__teve.cxx src/next_event_dummy.cpp)
endif()
target_link_libraries( teveDisplay DD4hepCore ${ROOT_EVE_LIBRARIES} DD4hepRec ${LCIO_LIBRARIES} ) 

#--- install target-------------------------------------

install(TARGETS geoDisplay geoConverter geoPluginRun teveDisplay
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
# to do: add corresponding uninstall...
#-------------------------------------------------------
