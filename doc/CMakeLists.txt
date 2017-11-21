#############################################
# Doxygen target to generate API reference  #
#############################################

FIND_PACKAGE(Doxygen)

IF(DOXYGEN_FOUND)
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/doc/reference/Doxyfile.in ${CMAKE_BINARY_DIR}/reference/Doxyfile @ONLY)
    ADD_CUSTOM_TARGET(reference
        ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/reference/Doxyfile
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/reference
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
        )
ENDIF(DOXYGEN_FOUND)

###############################################
# LaTeX target to compile the PDF user manual #
###############################################

# Information and the manual on the UseLATEX.cmake package can be found here
# http://www.cmake.org/Wiki/CMakeUserUseLATEX

FIND_PACKAGE(LATEX)

# Only add target if we have LaTeX
IF(LATEX_COMPILER)
    MESSAGE(STATUS "Found LaTeX compiler.")
    SET(LATEX_COMPILER_FLAGS "-shell-escape" "-interaction nonstopmode" "-halt-on-error" "-file-line-error" CACHE STRING "Flags passed to latex.")
    INCLUDE(${CMAKE_SOURCE_DIR}/cmake/LATEX.cmake)

    SET(HTLATEX_COMPILER_TEX4HT_FLAGS "html/dd4hep,charset=utf-8" CACHE STRING "Flags passed to Tex4HT" FORCE)
    SET(HTLATEX_COMPILER_TEX4HT_POSTPROCESSOR_FLAGS "-utf8 -cunihtf" CACHE STRING "Flags passed to the postprocessor of Tex4HT" FORCE)

    ADD_SUBDIRECTORY(usermanuals/DD4hep)
    ADD_SUBDIRECTORY(usermanuals/DDAlign)
    ADD_SUBDIRECTORY(usermanuals/DDCond)
    ADD_SUBDIRECTORY(usermanuals/DDEve)
    ADD_SUBDIRECTORY(usermanuals/DDG4)
    ADD_SUBDIRECTORY(usermanuals/DDRec)
    
ELSE(LATEX_COMPILER)
    MESSAGE(STATUS "No LaTeX found, cannot compile user manual.")
ENDIF(LATEX_COMPILER)
