# - Find Xerces-C
# This module tries to find the Xerces-C library and headers.
# Once done this will define
#
#   XERCESC_FOUND - system has Xerces-C headers and libraries
#   XERCESC_INCLUDE_DIRS - the include directories needed for Xerces-C
#   XERCESC_LIBRARIES - the libraries needed to use Xerces-C
#
# Variables used by this module, which can change the default behaviour and
# need to be set before calling find_package:
#
#   XERCESC_ROOT_DIR            Root directory to Xerces-C installation. Will
#                               be used ahead of CMake default path.
#
# The following advanced variables may be used if the module has difficulty
# locating Xerces-C or you need fine control over what is used.
#
#   XERCESC_INCLUDE_DIR
#
#   XERCESC_LIBRARY
#
# Copyright (c) 2009, Ben Morgan, <Ben.Morgan@warwick.ac.uk>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
include ( FindPackageHandleStandardArgs )

find_package(PythonInterp 2.7 REQUIRED)
find_package(PythonLibs   2.7 REQUIRED)

if (PYTHONLIBS_FOUND)
  get_filename_component(PYTHON_LIBRARY_DIR  ${PYTHON_LIBRARIES} DIRECTORY)
  #get_filename_component(PYTHON_LIBRARY_NAME ${PYTHON_LIBRARIES} NAME_WE)
  set(PYTHON_LIBRARY_DIR  ${PYTHON_LIBRARY_DIR})
  set(PYTHON_LIBRARIES    ${PYTHON_LIBRARIES})
  set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
  message(STATUS "******** PYTHON FOUND  ${PYTHON_LIBRARY_DIR}  -> ${PYTHON_LIBRARIES}")
else()
  message(STATUS "PYTHON ****NOT**** FOUND")
endif()
find_package_handle_standard_args(PYTHON DEFAULT_MSG PYTHON_INCLUDE_DIR)
mark_as_advanced(PYTHON_FOUND PYTHON_INCLUDE_DIRS)
