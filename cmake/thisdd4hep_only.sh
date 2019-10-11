#!/bin/bash
# Source this script to set up the DD4Hep installation that this script is part of.
# This version of the script has been modified to set up environment variables for
# only DD4hep.
#
# This script if for the csh like shells, see thisdd4hep.csh for csh like shells.
#
# Author: Pere Mato. F. Gaede, M.Frank
#-------------------------------------------------------------------------------
#
#echo " ### thisdd4hep_only.sh:   initialize the environment for DD4hep ! " 
#
#-----------------------------------------------------------------------------
dd4hep_parse_this()   {
    package=${2};
    if [ "x${1}" = "x" ]; then
	if [ ! -f bin/this${package}_only.sh ]; then
            echo ERROR: must "cd where/${package}/is" before calling ". bin/this${package}_only.sh" for this version of bash!;
            return 1;
	fi
	THIS="${PWD}";
    else
	THIS=$(dirname $(dirname ${1}));
    fi;
    THIS=$(cd ${THIS} > /dev/null; pwd);
}
#-----------------------------------------------------------------------------
dd4hep_add_path()   {
    path_name=${1};
    path_prefix=${2};
    eval path_value=\$$path_name;
    if [ ${path_value} ]; then
	path_value=${path_prefix}:${path_value};
    else
	path_value=${path_prefix};
    fi; 
    eval export ${path_name}=${path_value};
}
#-----------------------------------------------------------------------------
dd4hep_add_library_path()    {
    path_prefix=${1};
    if [ @USE_DYLD@ ];
    then
        if [ ${DYLD_LIBRARY_PATH} ]; then
            export DYLD_LIBRARY_PATH=${path_prefix}:$DYLD_LIBRARY_PATH;
            export DD4HEP_LIBRARY_PATH=${path_prefix}:$DD4HEP_LIBRARY_PATH;
        else
            export DYLD_LIBRARY_PATH=${path_prefix};
            export DD4HEP_LIBRARY_PATH=${path_prefix};
        fi;
    else
        if [ ${LD_LIBRARY_PATH} ]; then
	    export LD_LIBRARY_PATH=${path_prefix}:$LD_LIBRARY_PATH;
        else
	    export LD_LIBRARY_PATH=${path_prefix};
        fi;
    fi;
}
#-----------------------------------------------------------------------------
#
dd4hep_parse_this ${BASH_ARGV[0]} DD4hep;
#
#----DD4hep installation directory--------------------------------------------
export DD4hepINSTALL=${THIS};
export DD4hep_DIR=${THIS};
export DD4hep_ROOT=${THIS};
#
#----PATH---------------------------------------------------------------------
dd4hep_add_path PATH       ${THIS}/bin;
#----LIBRARY_PATH-------------------------------------------------------------
dd4hep_add_library_path    ${THIS}/lib;
#----PYTHONPATH---------------------------------------------------------------
dd4hep_add_path PYTHONPATH ${THIS}/@DD4HEP_PYTHON_INSTALL_DIR@;
#----ROOT_INCLUDE_PATH--------------------------------------------------------
dd4hep_add_path ROOT_INCLUDE_PATH ${THIS}/include;
#-----------------------------------------------------------------------------
if [ @USE_DYLD@ ];
then
    export DD4HEP_LIBRARY_PATH=${DYLD_LIBRARY_PATH};
else
    export DD4HEP_LIBRARY_PATH=${LD_LIBRARY_PATH};
fi;
#-----------------------------------------------------------------------------
#
unset THIS;
#-----------------------------------------------------------------------------
