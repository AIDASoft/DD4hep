# - Try to find the Geant4 library and headers
# Usage of this module is as follows
#
# == Using installation of Geant4: ==
#
#     find_package(Geant4 9.5)
#     if(Geant4_FOUND)
#         include_directories(${Geant4_INCLUDE_DIRS})
#         add_executable(foo foo.cc)
#         target_link_libraries(foo ${Geant4_LIBRARIES})
#     endif()
#
# You can provide a minimum version number that should be used.
# If you provide this version number and specify the REQUIRED attribute,
# this module will fail if it can't find a Geant4 of the specified version
# or higher. If you further specify the EXACT attribute, then this module 
# will fail if it can't find a Geant4 with a version eaxctly as specified.
#
# ===========================================================================
# Variables used by this module which can be used to change the default
# behaviour, and hence need to be set before calling find_package:
#
#  Geant4_ROOT_DIR       The preferred installation prefix for searching for 
#                        Geant4. Set this if the module has problems finding 
#                        the proper Geant4 installation.
#
# If you don't supply Geant4_ROOT_DIR, the module will search on the standard
# system paths. On UNIX, the module will also try to find the clhep-config
# program in the PATH, and if found will use the prefix supplied by this
# program as a HINT on where to find the CLHEP headers and libraries.
#
# ============================================================================
# Variables set by this module:
#
#  Geant4_FOUND           System has Geant4.
#  Geant4_INCLUDE_DIRS    Geant4 include directories: not cached.
#  Geant4_LIBRARIES       Link to these to use the Geant4 library: not cached.
#
# Additional components
#  Geant4_<component>_FOUND
#  Geant4_<component>_LIBRARY
# ===========================================================================


#----------------------------------------------------------------------------
# Enable re-search if known Geant4_ROOT_DIR changes?
#
if(NOT "${Geant4_ROOT_DIR}" STREQUAL "${Geant4_INTERNAL_ROOT_DIR}")
    if(Geant4_INTERNAL_ROOT_DIR AND NOT Geant4_FIND_QUIETLY)
        message(STATUS "Geant4_ROOT_DIR Changed, Rechecking for Geant4")
    endif()

    set(Geant4_INTERNAL_ROOT_DIR ${Geant4_ROOT_DIR} 
        CACHE INTERNAL "Last value supplied for where to locate Geant4")
    set(Geant4_INCLUDE_DIR Geant4_INCLUDE_DIR-NOTFOUND)
    set(Geant4_LIBRARY Geant4_LIBRARY-NOTFOUND)
    set(Geant4_LIBRARIES )
    set(Geant4_INCLUDE_DIRS )
    set(Geant4_FOUND FALSE)
endif()

#----------------------------------------------------------------------------
# - If we already found Geant4, be quiet
#
if(Geant4_INCLUDE_DIR AND Geant4_LIBRARY)
    set(Geant4_FIND_QUIETLY TRUE)
endif()

#----------------------------------------------------------------------------
# Set up HINTS on where to look for CLHEP
# If we're on UNIX, see if we can find clhep-config and use its --prefix
# as an extra hint.
#
set(_Geant4_root_hints ${Geant4_ROOT_DIR} ${Geant4_ROOT_DIR}/.. )

#----------------------------------------------------------------------------
# Find the Geant4 headers
# Use G4Version.hh as locator
find_path(Geant4_INCLUDE_DIR G4Version.hh 
    HINTS ${_Geant4_root_hints}
    PATH_SUFFIXES include share/include include/Geant4
    DOC "Path to the Geant4 headers" 
)

#----------------------------------------------------------------------------
# Find the CLHEP library
# Prefer lib64 if available.
find_library(Geant4_LIBRARY G4global
    HINTS ${_Geant4_root_hints}
    PATH_SUFFIXES lib64 lib
    DOC "Path to the G4global library"
)

#----------------------------------------------------------------------------
# Extract the Geant4 version from G4Version.hh
# Versions COMPATIBLE if RequestedVersion > FoundVersion
# Also check if versions exact

if(Geant4_INCLUDE_DIR)
    set(Geant4_VERSION 0)
    file(READ "${Geant4_INCLUDE_DIR}/G4Version.hh" _Geant4_VERSION_CONTENTS)
    string(REGEX REPLACE ".*#define G4VERSION_NUMBER  ([0-9.]+).*" "\\1"
        Geant4_VERSION "${_Geant4_VERSION_CONTENTS}")
    
    if(NOT Geant4_FIND_QUIETLY)
        message(STATUS "Found Geant4 Version ${Geant4_VERSION}")
    endif()
  
    if(Geant4_FIND_VERSION)
        set(Geant4_VERSIONING_TESTS Geant4_VERSION_COMPATIBLE)

        if("${Geant4_VERSION}" VERSION_LESS "${Geant4_FIND_VERSION}")
            set(Geant4_VERSION_COMPATIBLE FALSE)
        else()
            set(Geant4_VERSION_COMPATIBLE TRUE)

            if(Geant4_FIND_VERSION_EXACT)
                if("${Geant4_VERSION}" VERSION_EQUAL "${Geant4_FIND_VERSION}")
                    set(Geant4_VERSION_EXACT TRUE)
                endif()
                    list(APPEND Geant4_VERSIONING_TESTS Geant4_VERSION_EXACT)
            endif()
        endif()
    else()
      set(Geant4_ANY_VERSION TRUE)
      set(Geant4_VERSIONING_TESTS Geant4_ANY_VERSION)
    endif()
endif()

#----------------------------------------------------------------------------
# Construct an error message for FPHSA
#
set(Geant4_DEFAULT_MSG "Could NOT find Geant4:\n")

if(NOT Geant4_INCLUDE_DIR)
    set(Geant4_DEFAULT_MSG "${Geant4_DEFAULT_MSG}Geant4 Header Path Not Found\n")
endif()

if(Geant4_LIBRARY)
    get_filename_component(Geant4_LIBRARY_DIR "${Geant4_LIBRARY}" PATH)
else()
    set(Geant4_DEFAULT_MSG "${Geant4_DEFAULT_MSG}Geant4 Library Not Found\n")
endif()

if(Geant4_FIND_VERSION)
    if(NOT Geant4_VERSION_COMPATIBLE)
        set(Geant4_DEFAULT_MSG "${Geant4_DEFAULT_MSG}Incompatible versions, ${Geant4_VERSION}(found) < ${Geant4_FIND_VERSION}(required)\n")
    endif()

    if(Geant4_FIND_VERSION_EXACT)
        if(NOT Geant4_VERSION_EXACT)
            set(Geant4_DEFAULT_MSG "${Geant4_DEFAULT_MSG}Non-exact versions, ${Geant4_VERSION}(found) != ${Geant4_FIND_VERSION}(required)\n")
        endif()
    endif()
endif()


#----------------------------------------------------------------------------
# Handle the QUIETLY and REQUIRED arguments, setting Geant4_FOUND to TRUE if
# all listed variables are TRUE
#
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Geant4 
    "${Geant4_DEFAULT_MSG}"
    Geant4_LIBRARY
    Geant4_INCLUDE_DIR
    ${Geant4_VERSIONING_TESTS}
    )
    
set(Geant4_FOUND GEANT_FOUND)  # The previous function sets only the uppercased name

#----------------------------------------------------------------------------
# If we found Geant4, set the needed non-cache variables
#
if(Geant4_FOUND)
    set(Geant4_LIBRARIES -L${Geant4_LIBRARY_DIR}     
      G4vis_management
      G4modeling
      G4interfaces
      G4persistency
      G4analysis
      G4error_propagation
      G4readout
      G4physicslists
      G4run
      G4event
      G4tracking
      G4parmodels
      G4processes
      G4digits_hits
      G4track
      G4particles
      G4geometry
      G4materials
      G4graphics_reps
      G4intercoms
      G4global )
    set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIR})
endif()

#----------------------------------------------------------------------------
# Mark cache variables that can be adjusted as advanced
#
mark_as_advanced(Geant4_INCLUDE_DIR Geant4_LIBRARY)

#----------------------------------------------------------------------------
# Components
if(Geant4_FOUND)
  find_library(Geant4_clhep_LIBRARY G4clhep PATHS ${Geant4_LIBRARY_DIR})
  if(Geant4_clhep_LIBRARY)
     set(Geant4_LIBRARIES ${Geant4_LIBRARIES} G4clhep)
     set(Geant4_clhep_FOUND TRUE)
  else()
     set(Geant4_clhep_FOUND FALSE)  
  endif()
endif()

