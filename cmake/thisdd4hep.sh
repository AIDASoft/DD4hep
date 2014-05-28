
# Source this script to set up the DD4Hep installation that this script is part of.
#
# This script if for the csh like shells, see thisdd4hep.csh for csh like shells.
#
# Author: Pere Mato. F. Gaede


#echo " ### thisdd4hep.sh:   initialize the environment for DD4hep ! " 

if [ "x${BASH_ARGV[0]}" = "x" ]; then
    if [ ! -f bin/thisdd4hep.sh ]; then
        echo ERROR: must "cd where/dd4hep/is" before calling ". bin/thisdd4hep.sh" for this version of bash!
        return 1
    fi
else
    # get param to "."
    THIS=$(dirname ${BASH_ARGV[0]})
fi


#----------- source the ROOT environment first
source @ROOT_ROOT@/bin/thisroot.sh

if [ @DD4HEP_WITH_GEANT4@ ]
then
    export G4INSTALL=@Geant4_ROOT@
    export G4ENV_INIT=@Geant4_ROOT@/bin/geant4.sh

# ---------- initialze geant4 environment
    test -r ${G4ENV_INIT} && { cd $(dirname ${G4ENV_INIT}) ; . ./$(basename ${G4ENV_INIT}) ; cd $OLDPWD ; }


#---- if geant4 was built with external CLHEP we have to extend the dynamic search path
    if [ @GEANT4_WITH_CLHEP@ ] ; then
	if [ @USE_DYLD@ ]
	then
	    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:@CLHEP_LIBRARY_PATH@
	else
	    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:@CLHEP_LIBRARY_PATH@
	fi
    fi
fi

#----PATH-----------------------------------------------------------------
if [ -z "${PATH}" ]; then
  PATH=@CMAKE_INSTALL_PREFIX@/bin; export PATH       # Linux, ELF HP-UX
else
  PATH=@CMAKE_INSTALL_PREFIX@/bin:$PATH; export PATH
fi


#----(DY)LD_LIBRARY_PATH---------------------------------------------------------------
if [ @USE_DYLD@ ]
then
    if [ -z "${DYLD_LIBRARY_PATH}" ]; then
	DYLD_LIBRARY_PATH=@CMAKE_INSTALL_PREFIX@/lib:@XERCESC_ROOT_DIR@/lib; export DYLD_LIBRARY_PATH   # Mac OS X
    else
	DYLD_LIBRARY_PATH=@CMAKE_INSTALL_PREFIX@/lib:@XERCESC_ROOT_DIR@/lib:$DYLD_LIBRARY_PATH; export DYLD_LIBRARY_PATH
    fi
else
    if [ -z "${LD_LIBRARY_PATH}" ]; then
	LD_LIBRARY_PATH=@CMAKE_INSTALL_PREFIX@/lib:@XERCESC_ROOT_DIR@/lib:@Geant4_DIR@/..; export LD_LIBRARY_PATH       # Linux, ELF HP-UX
    else
	LD_LIBRARY_PATH=@CMAKE_INSTALL_PREFIX@/lib:@XERCESC_ROOT_DIR@/lib:@Geant4_DIR@/..:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
    fi
fi

#----PYTHONPATH---------------------------------------------------------------
if [ -z "${PYTHONPATH}" ]; then
  PYTHONPATH=@CMAKE_INSTALL_PREFIX@/python/DDG4/python:@CMAKE_SOURCE_DIR@/DDCore/python; export PYTHONPATH
else
  PYTHONPATH=@CMAKE_INSTALL_PREFIX@/python/DDG4/python:@CMAKE_SOURCE_DIR@/DDCore/python:$PYTHONPATH; export PYTHONPATH
fi

#----DD4hep installation directory--------------------------------------------
DD4hepINSTALL=@CMAKE_INSTALL_PREFIX@; export DD4hepINSTALL;
