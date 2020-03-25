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
#-----------------------------------------------------------------------------
dd4hep_parse_this()   {
    SOURCE=${1}
    if [ "x${SOURCE}" = "x" ]; then
        if [ -f bin/thisdd4hep.sh ]; then
            THIS="$PWD"; export THIS
        elif [ -f ./thisdd4hep.sh ]; then
            THIS=$(cd ..  > /dev/null; pwd); export THIS
        else
            echo ERROR: must "cd where/dd4hep/is" before calling ". bin/thisdd4hep.sh" for this version of bash!
            THIS=; export THIS
            return 1
        fi
    else
        # get param to "."
        thisroot=$(dirname ${SOURCE})
        THIS=$(cd ${thisroot}/.. > /dev/null;pwd); export THIS
    fi
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
    ## echo "DD4hep_add_path: ${path_name}=${path_value}";
}
#-----------------------------------------------------------------------------
dd4hep_add_library_path()    {
    path_prefix=${1};
    if [ @APPLE@ ];
    then
        if [ ${DYLD_LIBRARY_PATH} ]; then
            export DYLD_LIBRARY_PATH=${path_prefix}:$DYLD_LIBRARY_PATH;
            export LD_LIBRARY_PATH=${path_prefix}:$LD_LIBRARY_PATH;
            export DD4HEP_LIBRARY_PATH=${path_prefix}:$DD4HEP_LIBRARY_PATH;
        else
            export DYLD_LIBRARY_PATH=${path_prefix};
            export LD_LIBRARY_PATH=${path_prefix};
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
SOURCE=${BASH_ARGV[0]}
if [ "x$SOURCE" = "x" ]; then
    SOURCE=${(%):-%N} # for zsh
fi

dd4hep_parse_this $SOURCE;
#
# These 3 are the main configuration variables: ROOT, Geant4 and XercesC
# --> LCIO & Co. are handled elsewhere!

if [ -z $ROOTSYS ]; then
    export ROOTSYS=`dirname @ROOT_DIR@`
fi;
export Geant4_DIR=@Geant4_DIR@;
export XERCESCINSTALL=@XERCESC_ROOT_DIR@;
#
#----DD4hep installation directory--------------------------------------------
export DD4hepINSTALL=${THIS};
export DD4hep_DIR=${THIS};
export DD4hep_ROOT=${THIS};
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
    export CLHEP_DIR=@CLHEP_INCLUDE_DIR@/../;
    export CLHEP_ROOT_DIR=@CLHEP_INCLUDE_DIR@/../;
    export CLHEP_INCLUDE_DIR=@CLHEP_INCLUDE_DIR@;
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
dd4hep_add_library_path @BOOST_DIR@
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
unset ROOTENV_INIT;
unset THIS;
#-----------------------------------------------------------------------------
