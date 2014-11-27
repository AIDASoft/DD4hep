#-----------------------------------------------------------------------------------
#  DDDetectors
#
#  Author:  M.Frank
#  Date:    13/11/2014
#
#-----------------------------------------------------------------------------------
#
#---Find ROOT-----------------------------------------------------------------------
find_package( ROOT REQUIRED COMPONENTS Geom Reflex )
#
#---Includedirs---------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
	            ${CMAKE_SOURCE_DIR}/DDRec/include	
	            ${CMAKE_SOURCE_DIR}/DDSurfaces/include	
	            ${CMAKE_SOURCE_DIR}/DDSegmentation/include	
                    ${ROOT_INCLUDE_DIR} )
#
#---Add Library---------------------------------------------------------------------
file(GLOB sources src/*.cpp )
include(DD4hep_XML_setup)

add_library( DD4hepDetectors SHARED ${sources})
target_link_libraries( DD4hepDetectors DD4hepCore DD4hepRec ${ROOT_LIBRARIES} Reflex )
#
#---Rootmap generation--------------------------------------------------------------
dd4hep_generate_rootmap( DD4hepDetectors )
#
#--- install target-----------------------------------------------------------------
install( TARGETS DD4hepDetectors LIBRARY DESTINATION lib )

install( DIRECTORY compact
  DESTINATION DDDetectors
  PATTERN ".svn" EXCLUDE )

# to do: add corresponding uninstall...
#-----------------------------------------------------------------------------------
