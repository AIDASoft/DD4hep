cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

message(" ======================= ")
message(" ${podio_INCLUDE_DIR}")
find_package(podio )
#if(NOT podio_FOUND)

if(DD4HEP_BUILD_DD4POD AND podio_FOUND) 

set(dd4pod_include_dir ${CMAKE_CURRENT_BINARY_DIR})
set(dd4pod_source_dir ${CMAKE_CURRENT_BINARY_DIR}/src)
PODIO_GENERATE_DATAMODEL(dd4pod dd4hep.yaml headers sources OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR})

add_library(DD4pod SHARED ${sources})

target_link_libraries(DD4POD
  PUBLIC podio::podio
  PUBLIC podio::podioRootIO
  PUBLIC DD4hep::DDCore
  PUBLIC DD4hep::DDG4
  )
target_include_directories(DD4POD
  PUBLIC  ${podio_INCLUDE_DIR}
  PUBLIC $<BUILD_INTERFACE:${dd4pod_include_dir}>
  $<INSTALL_INTERFACE:include>)

target_compile_features(DD4pod
  PUBLIC cxx_auto_type
  PUBLIC cxx_trailing_return_types
  PRIVATE cxx_variadic_templates
  PRIVATE cxx_std_17)

message("headers: ${headers}")
PODIO_GENERATE_DICTIONARY(DD4POD ${headers} 
  SELECTION ${dd4pod_source_dir}/selection.xml
  OPTIONS --library ${CMAKE_SHARED_LIBRARY_PREFIX}DD4pod${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
set_target_properties(DD4POD-dictgen PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_sources(DD4POD PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/DD4POD.cxx)

dd4hep_add_plugin(DD4PODIO
    SOURCES 
    plugins/Geant4Output2Podio.cxx
    NOINSTALL
    )
target_link_libraries(DD4PODIO
  PUBLIC DD4POD
  #PUBLIC DDG4_PIDsdet
  )
target_include_directories(DD4pod
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/plugins>
  )

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dd4pod
  DESTINATION include
  FILES_MATCHING PATTERN *.h
  )
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/DD4PODDict.rootmap"
  "${CMAKE_CURRENT_BINARY_DIR}/libDD4POD_rdict.pcm"
  DESTINATION lib)


install(TARGETS DD4pod DD4podIO
  EXPORT DDhep
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
  )

endif()

