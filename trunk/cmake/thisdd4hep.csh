# Source this script to set up the DD4Hep installation that this script is part of.
#
# This script if for the csh like shells, see thisdd4hep.sh for bash like shells.
#
# Author: Pere Mato

set ARGS=($_)
set THIS="`dirname ${ARGS[2]}`"


#----PATH----------------------------------------------------------------------------
if ($?PATH) then
   setenv PATH @EXECUTABLE_OUTPUT_PATH@:$PATH      # Linux, ELF HP-UX
else
   setenv PATH @EXECUTABLE_OUTPUT_PATH@
endif

#----LD_LIBRARY_PATH-----------------------------------------------------------------
if ($?LD_LIBRARY_PATH) then
   setenv LD_LIBRARY_PATH @LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib:$LD_LIBRARY_PATH      # Linux, ELF HP-UX
else
   setenv LD_LIBRARY_PATH @LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib
endif

#----DYLD_LIBRARY_PATH---------------------------------------------------------------
if ($?DYLD_LIBRARY_PATH) then
   setenv DYLD_LIBRARY_PATH @LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib:$DYLD_LIBRARY_PATH  # Mac OS X
else
   setenv DYLD_LIBRARY_PATH @LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib
endif

#----PYTHONPATH---------------------------------------------------------------
if ($?PYTHONPATH) then
   setenv PYTHONPATH @CMAKE_SOURCE_DIR@/DDCore/python:$PYTHONPATH
else
   setenv PYTHONPATH @CMAKE_SOURCE_DIR@/DDCore/python
endif
