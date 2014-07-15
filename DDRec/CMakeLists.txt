cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(DDRec)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
		    ${DDSegmentation_INCLUDE_DIR}
		    ${ROOT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DDSurfaces/include 
		    ${CMAKE_SOURCE_DIR}/DDCore/include)

file(GLOB sources src/*.cpp)


#---------------------------  GEAR wrappers for backward compatibility  -----------
if(DD4HEP_WITH_GEAR)

  find_package(GEAR REQUIRED COMPONENTS geartgeo)

  include_directories(  ${GEAR_INCLUDE_DIRS} )

  set(sources ${sources} src/gear/DDGear.cpp)

  add_definitions("-D DD4HEP_WITH_GEAR")

endif()
#-------------------------------------------------------------------------------------

add_library(DD4hepRec SHARED ${sources} )

# Find the DDSegmentation libraries
find_library(DDSegmentation_LIBS
  NAMES ${DDSegmentation_LIBRARIES}
  PATHS ${DDSegmentation_LIBRARY_DIR}
)

if(DD4HEP_WITH_GEAR)
  target_link_libraries(DD4hepRec ${ROOT_LIBRARIES} ${DDSegmentation_LIBS} DD4hepCore Geom Reflex ${libraries} ${GEAR_LIBRARIES} ${GEAR_COMPONENT_LIBRARIES})

  #------ binary ----------
  
  add_executable( convertToGear src/convertToGear.cc)
  
  target_link_libraries(convertToGear ${PackageName} ${DD4hep_LIBRARIES}  DD4hepRec
    ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES} 
    ${GEAR_LIBRARIES}
    )

else()
  target_link_libraries(DD4hepRec ${ROOT_LIBRARIES} ${DDSegmentation_LIBS} DD4hepCore Geom Reflex ${libraries})
endif(DD4HEP_WITH_GEAR)


SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
SET_TARGET_PROPERTIES( DD4hepRec PROPERTIES VERSION ${DD4hep_VERSION} SOVERSION ${DD4hep_SOVERSION})

#--- install target-------------------------------------
install(DIRECTORY include/DDRec
  DESTINATION include
  PATTERN ".svn" EXCLUDE )

if(DD4HEP_WITH_GEAR)
  install(TARGETS DD4hepRec convertToGear
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
  )
else()
  install(TARGETS DD4hepRec
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
  )
endif()


# to do: add corresponding uninstall...
#-------------------------------------------------------
