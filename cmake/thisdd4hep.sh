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
    package=${2};
    if [ "x${SOURCE}" = "x" ]; then
        if [ -f bin/this${package}.sh ]; then
            THIS="$PWD"; export THIS
        elif [ -f ./this${package}.sh ]; then
            THIS=$(cd ..  > /dev/null; pwd); export THIS
        else
            echo ERROR: must "cd where/${package}/is" before calling ". bin/this${package}.sh" for this version of bash!
            THIS=; export THIS
            return 1
        fi
    else
        # get param to "."
        thisroot=$(dirname ${SOURCE})
        THIS=$(cd ${thisroot}/.. > /dev/null;pwd); export THIS
    fi
    unset SOURCE package thisroot
}
#-----------------------------------------------------------------------------
dd4hep_add_path()   {
    path_name=${1}
    path_prefix=${2}
    eval path_value=\$$path_name
    if [ "${path_value}" ]; then
        # Prevent duplicates
        if ! echo ${path_value} | tr : '\n' | grep -q "^${path_prefix}$"; then
            path_value="${path_prefix}:${path_value}"
        fi
    else
	path_value="${path_prefix}"
    fi; 
    eval export ${path_name}='${path_value}'
    unset path_name path_prefix path_value
}
#-----------------------------------------------------------------------------
dd4hep_add_library_path()    {
    p=${1};
    if [ @APPLE@ ]; then
        # Do not prepend system library locations on macOS. Stuff will break.
        [[ "$p" = "/usr/lib" || "$p" = "/usr/local/lib" ]] && unset p && return
        dd4hep_add_path DYLD_LIBRARY_PATH     "$p"
        dd4hep_add_path DD4HEP_LIBRARY_PATH   "$p"
    else
        dd4hep_add_path LD_LIBRARY_PATH       "$p"
    fi
    unset p
}
#-----------------------------------------------------------------------------
#
SOURCE=${BASH_ARGV[0]}
if [ "x$SOURCE" = "x" ]; then
    SOURCE=${(%):-%N} # for zsh
fi

dd4hep_parse_this $SOURCE  dd4hep;
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
    unset G4ENV_INIT;
    unset G4LIB_DIR;
fi;
#
#----XercesC LIBRARY_PATH-----------------------------------------------------
if [ ${XERCESCINSTALL} ]; then
    dd4hep_add_library_path ${XERCESCINSTALL}/lib;
fi;
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
unset SOURCE;
#-----------------------------------------------------------------------------
