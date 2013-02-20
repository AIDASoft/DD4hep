
# Source this script to set up the DD4Hep installation that this script is part of.
#
# This script if for the csh like shells, see thisdd4hep.csh for csh like shells.
#
# Author: Pere Mato


if [ "x${BASH_ARGV[0]}" = "x" ]; then
    if [ ! -f bin/thisdd4hep.sh ]; then
        echo ERROR: must "cd where/dd4hep/is" before calling ". bin/thisdd4hep.sh" for this version of bash!
        return 1
    fi
else
    # get param to "."
    THIS=$(dirname ${BASH_ARGV[0]})
fi

#----PATH-----------------------------------------------------------------
if [ -z "${PATH}" ]; then
  PATH=@EXECUTABLE_OUTPUT_PATH@; export PATH       # Linux, ELF HP-UX
else
  PATH=@EXECUTABLE_OUTPUT_PATH@:$PATH; export PATH
fi

#----LD_LIBRARY_PATH-----------------------------------------------------------------
if [ -z "${LD_LIBRARY_PATH}" ]; then
  LD_LIBRARY_PATH=@LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib; export LD_LIBRARY_PATH       # Linux, ELF HP-UX
else
  LD_LIBRARY_PATH=@LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

#----DYLD_LIBRARY_PATH---------------------------------------------------------------
if [ -z "${DYLD_LIBRARY_PATH}" ]; then
  DYLD_LIBRARY_PATH=@LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib; export DYLD_LIBRARY_PATH   # Mac OS X
else
  DYLD_LIBRARY_PATH=@LIBRARY_OUTPUT_PATH@:@XERCESC_ROOT_DIR@/lib:$DYLD_LIBRARY_PATH; export DYLD_LIBRARY_PATH
fi

#----PYTHONPATH---------------------------------------------------------------
if [ -z "${PYTHONPATH}" ]; then
  PYTHONPATH=@CMAKE_SOURCE_DIR@/DDCore/python; export PYTHONPATH
else
  PYTHONPATH=@CMAKE_SOURCE_DIR@/DDCore/python:$PYTHONPATH; export PYTHONPATH
fi

