macro (dd4hep_ensure_out_of_source_build)

  string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" in_source)
  string( REGEX MATCH "${CMAKE_SOURCE_DIR}/" in_source_subdir "${CMAKE_BINARY_DIR}")
  if (in_source OR in_source_subdir)
  message(FATAL_ERROR "
ERROR: In source builds of this project are not allowed (SYSTEM_INSTALL=ON).
A separate build directory is required.
Please create one and run cmake from the build directory.
Also note that cmake has just added files to your source code directory.
We suggest getting a new copy of the source code.
Otherwise, delete `CMakeCache.txt' and the directory `CMakeFiles'.
  ")
  endif ()

endmacro (dd4hep_ensure_out_of_source_build)

