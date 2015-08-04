# - Finds ROOT instalation
# This module sets up ROOT information 
# It defines:
# ROOT_FOUND          If the ROOT is found
# ROOT_ROOT           the root directory ($ROOTSYS)
# ROOT_INCLUDE_DIR    PATH to the include directory
# ROOT_LIBRARIES      Most common libraries
# ROOT_LIBRARY_DIR    PATH to the library directory 


find_program(ROOT_CONFIG_EXECUTABLE root-config
  HINTS ${ROOTSYS}/bin $ENV{ROOTSYS}/bin)

if(ROOT_CONFIG_EXECUTABLE)
  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix 
    OUTPUT_VARIABLE ROOTSYS 
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
    OUTPUT_VARIABLE ROOT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
    OUTPUT_VARIABLE ROOT_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE ROOT_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --evelibs
    OUTPUT_VARIABLE ROOT_EVE_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(ROOT_LIBRARY_DIR ${ROOTSYS}/lib)
  set(ROOT_VERSION     ${ROOT_VERSION})
  set(ROOT_ROOT        ${ROOTSYS})

  # Make variables changeble to the advanced user
  mark_as_advanced(ROOT_CONFIG_EXECUTABLE)
endif()

###set(ROOT_LIBRARIES    ${ROOT_LIBRARIES} -lGenVector)     

set(ROOT_EVE_LIBRARIES ${ROOT_EVE_LIBRARIES} )

# handle the QUIETLY and REQUIRED arguments and set ROOT_FOUND to TRUE if
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args(ROOT DEFAULT_MSG ROOTSYS ROOT_INCLUDE_DIR)
mark_as_advanced(ROOT_FOUND ROOT_INCLUDE_DIR)

include ( CMakeParseArguments)
find_program(ROOTCINT_EXECUTABLE rootcint PATHS ${ROOTSYS}/bin $ENV{ROOTSYS}/bin)

if(NOT ROOT_VERSION_STRING)
  file(STRINGS ${ROOT_INCLUDE_DIR}/RVersion.h _RVersion REGEX "define *ROOT_RELEASE ")
  string(REGEX MATCH "\"(([0-9]+)\\.([0-9]+)/([0-9]+)([a-z]*|-rc[0-9]+))\"" _RVersion ${_RVersion})
  set(ROOT_VERSION_STRING ${CMAKE_MATCH_1} CACHE INTERNAL "Version of ROOT")
  set(ROOT_VERSION_MAJOR  ${CMAKE_MATCH_2} CACHE INTERNAL "Major version of ROOT")
  set(ROOT_VERSION_MINOR  ${CMAKE_MATCH_3} CACHE INTERNAL "Minor version of ROOT")
  set(ROOT_VERSION_PATCH  ${CMAKE_MATCH_4} CACHE INTERNAL "Patch version of ROOT")
endif()
##message(WARNING "ROOT ${ROOT_VERSION_MAJOR} ${ROOT_VERSION_MINOR} ${ROOT_VERSION_PATCH}")
if ( ${ROOT_VERSION_MAJOR} LESS 6 )
  set ( ROOT_LIBRARIES ${ROOT_LIBRARIES} -lReflex )
endif()
if(ROOT_FIND_COMPONENTS)
  message( STATUS "ROOT[${ROOT_VERSION_STRING}]: Looking for Components: ${ROOT_FIND_COMPONENTS}" )
  foreach(comp ${ROOT_FIND_COMPONENTS})
    if ( "${comp}" STREQUAL "TEve" )
      set(ROOT_LIBRARIES ${ROOT_LIBRARIES} ${ROOT_EVE_LIBRARIES} FTGL )
    else()
      set(ROOT_LIBRARIES ${ROOT_LIBRARIES} -l${comp} )
    endif()
  endforeach()
endif()


#----------------------------------------------------------------------------
# function root_generate_dictionary( dictionary   
#                                    header1 header2 ... 
#                                    LINKDEF linkdef1 ... 
#                                    OPTIONS opt1...)
function(root_generate_dictionary dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "LINKDEF;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  set(includedirs) 
  foreach( d ${incdirs})    
   set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get LinkDef.h file------------------------------------
  set(linkdefs)
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(linkdefs ${linkdefs} ${f})
    else() 
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()
  #---call rootcint------------------------------------------
  message(STATUS "ROOT Dictionary output: ${dictionary}.cxx ${dictionary}.h")
  add_custom_command(OUTPUT ${dictionary}.cxx ${dictionary}.h
                     COMMAND echo ${ROOTCINT_EXECUTABLE} -cint -f  ${dictionary}.cxx 
                                          -c -p ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs}
                     COMMAND ${ROOTCINT_EXECUTABLE} -f  ${dictionary}.cxx 
                                          -c -p ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs}
                     DEPENDS ${headerfiles} ${linkdefs})
endfunction()

