cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(DDRec)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
		    ${DDSegmentation_INCLUDE_DIRS}
		    ${CMAKE_SOURCE_DIR}/DDSurfaces/include
		    ${CMAKE_SOURCE_DIR}/DDCore/include)

file(GLOB sources src/*.cpp)

set(rec_link_libraries DDCore ${DDSegmentation_LIBRARIES} ${ROOT_LIBRARIES} Geom Reflex)

#---------------------------  GEAR wrappers for backward compatibility  -----------
if(DD4HEP_USE_GEAR)

  find_package(GEAR REQUIRED COMPONENTS geartgeo)
  include_directories(SYSTEM ${GEAR_INCLUDE_DIRS})
  add_definitions("-D DD4HEP_USE_GEAR")

  list(APPEND sources src/gear/DDGear.cpp)
  list(APPEND plugin_sources  src/gear/createGearForILD.cpp src/gear/createGearForCLIC.cpp)
  list(APPEND rec_link_libraries ${GEAR_LIBRARIES} ${GEAR_COMPONENT_LIBRARIES})

  add_executable(convertToGear src/convertToGear.cc)

  target_link_libraries(convertToGear ${PackageName} ${DD4hep_LIBRARIES} DDRec
    ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} ${GEAR_LIBRARIES}
    )

  install(TARGETS convertToGear
    RUNTIME DESTINATION bin
  )

endif()
#-------------------------------------------------------------------------------------

add_library(DDRec SHARED ${sources})
target_link_libraries(DDRec ${rec_link_libraries})

# Add the DDRecPlugin, as there are currently only gear plugins, we need to check if we want to create the plugin
if( plugin_sources )
  add_dd4hep_plugin(DDRecPlugins SHARED ${plugin_sources})
  target_link_libraries(DDRecPlugins ${rec_link_libraries} DDRec)
endif()


#SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
SET_TARGET_PROPERTIES(DDRec PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#--- install target-------------------------------------
install(DIRECTORY include/DDRec
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

install(TARGETS DDRec
  LIBRARY DESTINATION lib
  )

# to do: add corresponding uninstall...
#-------------------------------------------------------
