#---Add a target to generate API documentation with Doxygen-------------------------
find_package(Doxygen)

if(DOXYGEN_FOUND)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION doc
    )

else()
  message(STATUS "Doxygen command not found. Documentation target (doc) not available")
endif()