#!/bin/bash
# Source this script to set up the DD4Hep installation that this script is part of.
#
# This script if for the csh like shells, see thisdd4hep.csh for csh like shells.
#
# Author: Pere Mato. F. Gaede, M.Frank
#-------------------------------------------------------------------------------
#
#echo " ### thisdd4hep.sh:   initialize the environment for DD4hep ! " 
#
dd4hep_parse_this()   {
    package=${2};
    if [ "x${1}" = "x" ]; then
	if [ ! -f bin/this${package}.sh ]; then
            echo ERROR: must "cd where/${package}/is" before calling ". bin/this${package}.sh" for this version of bash!;
            return 1;
	fi
	THIS="${PWD}";
    else
	# get param to "."
	THIS=$(dirname $(dirname ${1}));
	#if [ ! -f ${THIS}/bin/this${package}.sh ]; then
	#    THIS=$(dirname ${package});
	#fi;
    fi;
    THIS=$(cd ${THIS} > /dev/null; pwd);
}

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
    ## echo "dd4hep_add_path: ${path_name}=${path_value}";
}

dd4hep_add_library_path()    {
    path_prefix=${1};
    if [ @USE_DYLD@ ];
    then
        if [ ${DYLD_LIBRARY_PATH} ]; then
	    export DYLD_LIBRARY_PATH=${path_prefix}:$DYLD_LIBRARY_PATH;
        else
            export DYLD_LIBRARY_PATH=${path_prefix};
        fi;
    else
        if [ ${LD_LIBRARY_PATH} ]; then
	    export LD_LIBRARY_PATH=${path_prefix}:$LD_LIBRARY_PATH;
        else
	    export LD_LIBRARY_PATH=${path_prefix};
        fi;
    fi;
}
#
dd4hep_parse_this ${BASH_ARGV[0]} DD4hep;
#
# These 3 are the main configuration variables: ROOT, Geant4 and XercesC
# --> LCIO & Co. are handled elsewhere!
export ROOTSYS=@ROOT_ROOT@;
export Geant4_DIR=@Geant4_DIR@;
export XERECESCINSTALL=@XERCESC_ROOT_DIR@;
#
#----DD4hep installation directory--------------------------------------------
export DD4hepINSTALL=${THIS};
#
#----------- source the ROOT environment first ------------------------------
ROOTENV_INIT=${ROOTSYS}/bin/thisroot.sh;
test -r ${ROOTENV_INIT} && { cd $(dirname ${ROOTENV_INIT}); . ./$(basename ${ROOTENV_INIT}) ; cd $OLDPWD ; }
#
#----Geant4 LIBRARY_PATH------------------------------------------------------
if [ ${Geant4_DIR} ]; then
    G4LIB_DIR=`dirname ${Geant4_DIR}`;
    export G4INSTALL=`dirname ${G4LIB_DIR}`;
    export G4ENV_INIT=${G4INSTALL}/bin/geant4.sh
    # ---------- initialze geant4 environment
    test -r ${G4ENV_INIT} && { cd $(dirname ${G4ENV_INIT}) ; . ./$(basename ${G4ENV_INIT}) ; cd $OLDPWD ; }
    #---- if geant4 was built with external CLHEP we have to extend the dynamic search path
    if [ @GEANT4_USE_CLHEP@ ] ; then
	dd4hep_add_library_path @CLHEP_LIBRARY_PATH@;
    fi;
    dd4hep_add_library_path ${G4LIB_DIR};
    unset G4ENV_INIT;
    unset G4LIB_DIR;
fi;
#
#----XercesC LIBRARY_PATH-----------------------------------------------------
if [ ${XERCESCINSTALL} ]; then
    #dd4hep_add_path    PATH ${XERCESCINSTALL}/bin;
    dd4hep_add_library_path ${XERCESCINSTALL}/lib;
fi;
#
#----PATH---------------------------------------------------------------------
dd4hep_add_path PATH       ${THIS}/bin;
#----LIBRARY_PATH-------------------------------------------------------------
dd4hep_add_library_path    ${THIS}/lib;
#----PYTHONPATH---------------------------------------------------------------
dd4hep_add_path PYTHONPATH ${THIS}/python;
#
unset ROOTENV_INIT;
unset THIS;
#-----------------------------------------------------------------------------
