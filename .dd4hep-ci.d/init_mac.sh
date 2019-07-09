#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    if [ $(sw_vers -productVersion | awk -F '.' '{print $1 "." $2}') == "10.14" ]; then
        OS=mac1014
        COMPILER_TYPE=clang
        COMPILER_VERSION=clang100
    else
        echo "Bootstrap only works on macOS Mojave (10.14)"
        exit 1
    fi
else
    echo "This script is only meant for Mac"
    exit 1
fi

# Determine is you have CVMFS installed
if [ ! -d "/cvmfs" ]; then
    echo "No CVMFS detected, please install it."
    exit 1
fi

if [ ! -d "/cvmfs/clicdp.cern.ch" ]; then
    echo "No clicdp CVMFS repository detected, please add it."
    exit 1
fi

# Choose build type
if [ -z ${BUILD_TYPE} ]; then
    BUILD_TYPE=opt
fi


# General variables
CLICREPO=/cvmfs/clicdp.cern.ch
BUILD_FLAVOUR=x86_64-${OS}-${COMPILER_VERSION}-${BUILD_TYPE}

#--------------------------------------------------------------------------------
#     CMake
#--------------------------------------------------------------------------------

export CMAKE_HOME=${CLICREPO}/software/CMake/3.14.3/${BUILD_FLAVOUR}
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python
#--------------------------------------------------------------------------------

export PYTHONDIR=${CLICREPO}/software/Python/2.7.16/${BUILD_FLAVOUR}
export PATH=${PYTHONDIR}/bin:$PATH
export DYLD_LIBRARY_PATH="${PYTHONDIR}/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=${CLICREPO}/software/ROOT/6.18.00/${BUILD_FLAVOUR}
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export DYLD_LIBRARY_PATH="$ROOTSYS/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=${CLICREPO}/software/Xerces-C/3.1.3/${BUILD_FLAVOUR}
export PATH="$XercesC_HOME/bin:$PATH"
export DYLD_LIBRARY_PATH="$XercesC_HOME/lib:$DYLD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Geant4
#--------------------------------------------------------------------------------

export G4INSTALL=${CLICREPO}/software/Geant4/10.05.p01/${BUILD_FLAVOUR}
export G4LIB=$G4INSTALL/lib/Geant4-10.5.1/
export G4ENV_INIT="${G4INSTALL}/bin/geant4.sh"
export G4SYSTEM="Linux-g++"


#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=${CLICREPO}/software/Boost/1.70.0/${BUILD_FLAVOUR}
export DYLD_LIBRARY_PATH="${BOOST_ROOT}/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Ninja
#--------------------------------------------------------------------------------

export Ninja_HOME=${CLICREPO}/software/Ninja/1.9.0/${BUILD_FLAVOUR}
export PATH="$Ninja_HOME:$PATH"

#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------

export LCIO=${CLICREPO}/software/LCIO/2.12.1/${BUILD_FLAVOUR}/
export CMAKE_PREFIX_PATH="$LCIO:$CMAKE_PREFIX_PATH"
