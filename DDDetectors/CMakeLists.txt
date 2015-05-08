#-----------------------------------------------------------------------------------
#  DDDetectors
#
#  Author:  M.Frank
#  Date:    13/11/2014
#
#-----------------------------------------------------------------------------------
#
#---Find ROOT-----------------------------------------------------------------------
find_package( ROOT REQUIRED COMPONENTS Geom )
#---Find and configure DD4hep basics------------------------------------------------
include(DD4hep)
#---Find and configure Xerces-------------------------------------------------------
include(DD4hep_XML_setup)
#
#---Includedirs---------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDRec/include	
	            ${CMAKE_SOURCE_DIR}/DDSurfaces/include	
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${ROOT_INCLUDE_DIR} )
#---Add Library---------------------------------------------------------------------
file(GLOB sources src/*.cpp )

add_dd4hep_plugin( DDDetectors SHARED ${sources})
target_link_libraries( DDDetectors DDCore DDRec ${ROOT_LIBRARIES} )

#
#--- install target-----------------------------------------------------------------
install( TARGETS DDDetectors LIBRARY DESTINATION lib )

install( DIRECTORY compact
  DESTINATION DDDetectors
  PATTERN ".svn" EXCLUDE )

# to do: add corresponding uninstall...
#-----------------------------------------------------------------------------------
