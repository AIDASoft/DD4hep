
#---Build the VGM Libraries-------------------------------------------------------
include(ExternalProject)
ExternalProject_Add( 
    VGM
	  PREFIX VGM
    SVN_REPOSITORY https://vgm.svn.sourceforge.net/svnroot/vgm/tags/v3-05/vgm
    UPDATE_COMMAND ""
    PATCH_COMMAND patch -p0 -i ${CMAKE_CURRENT_SOURCE_DIR}/AddCMakeFiles.patch
	  INSTALL_DIR ${CMAKE_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DVGM_WITH_XmlVGM:BOOL=OFF -DGeant4_DIR=${Geant4_DIR})

set(VGM_LIBRARIES -L${CMAKE_BINARY_DIR}/lib -lRootGM -lGeant4GM -lBaseVGM)
set(VGM_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

#---Find Geant4-------------------------------------------------------------------
find_package(Geant4 REQUIRED)

#---Includedirs-------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/DDCore/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include 
                    ${ROOT_INCLUDE_DIR}
                    ${Geant4_INCLUDE_DIR}
                    ${VGM_INCLUDE_DIR})

#---Add Library-------------------------------------------------------------------
file(GLOB sources src/*.cpp)
add_library(DD4hepGeant4 SHARED ${sources})
target_link_libraries(DD4hepGeant4 DD4hepCore ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${VGM_LIBRARIES})
add_dependencies(DD4hepGeant4 VGM)


