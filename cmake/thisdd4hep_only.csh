# Source this script to set up the DD4Hep installation that this script is part of.
# This version of the script has been modified to set up environment variables for
# only DD4hep.
#
# This script if for the csh like shells, see thisdd4hep_only.sh for bash like shells.
#
# Author: Pere Mato

set ARGS=($_)
set THIS="`dirname ${ARGS[2]}`"


#----PATH----------------------------------------------------------------------------
if ($?PATH) then
   setenv PATH @CMAKE_INSTALL_PREFIX@/bin:$PATH      # Linux, ELF HP-UX
else
   setenv PATH @CMAKE_INSTALL_PREFIX@/bin
endif

#----LD_LIBRARY_PATH-----------------------------------------------------------------
if ($?LD_LIBRARY_PATH) then
   setenv LD_LIBRARY_PATH @CMAKE_INSTALL_PREFIX@/lib:$LD_LIBRARY_PATH      # Linux, ELF HP-UX
else
   setenv LD_LIBRARY_PATH @CMAKE_INSTALL_PREFIX@/lib
endif

#----DYLD_LIBRARY_PATH---------------------------------------------------------------
if ($?DYLD_LIBRARY_PATH) then
   setenv DYLD_LIBRARY_PATH @CMAKE_INSTALL_PREFIX@/lib:$DYLD_LIBRARY_PATH  # Mac OS X
else
   setenv DYLD_LIBRARY_PATH @CMAKE_INSTALL_PREFIX@/lib
endif

#----PYTHONPATH---------------------------------------------------------------
if ($?PYTHONPATH) then
   setenv PYTHONPATH @CMAKE_SOURCE_DIR@/DDCore/python:$PYTHONPATH
else
   setenv PYTHONPATH @CMAKE_SOURCE_DIR@/DDCore/python
endif
