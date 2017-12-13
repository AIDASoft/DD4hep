#!/bin/bash

#Determine which OS you are using
if [ "$(uname)" == "Darwin" ]; then
    if [ $(sw_vers -productVersion | awk -F '.' '{print $1 "." $2}') == "10.13" ]; then
        OS=mac1013
        COMPILER_TYPE=llvm
        COMPILER_VERSION=llvm90
    else
        echo "Bootstrap only works on macOS High Sierra (10.13)"
    fi
elif [ "$(uname)" == "Linux" ]; then
    if [ "$( cat /etc/*-release | grep Scientific )" ]; then
        OS=slc6
    elif [ "$( cat /etc/*-release | grep CentOS )" ]; then
        OS=centos7
    fi
else
    echo "UNKNOWN OS"
    exit 1
fi

#Determine is you have CVMFS installed
if [ ! -d "/cvmfs" ]; then
    echo "No CVMFS detected, please install it."
    exit 1
fi

if [ ! -d "/cvmfs/clicdp.cern.ch" ]; then
    echo "No clicdp CVMFS repository detected, please add it."
    exit 1
fi


#Determine which compiler to use
if [ -z ${COMPILER_TYPE} ]; then
    COMPILER_TYPE="gcc"
fi
if [ ${COMPILER_TYPE} == "gcc" ]; then
    COMPILER_VERSION="gcc7"
fi
if [[ ${COMPILER_TYPE} == "llvm" && ${OS} != "mac1013" ]]; then
    COMPILER_VERSION="llvm5"
fi


#Choose build type
if [ -z ${BUILD_TYPE} ]; then
    BUILD_TYPE=opt
fi


# General variables
CLICREPO=/cvmfs/clicdp.cern.ch
BUILD_FLAVOUR=x86_64-${OS}-${COMPILER_VERSION}-${BUILD_TYPE}

#--------------------------------------------------------------------------------
#     Compiler
#--------------------------------------------------------------------------------

if [[ ${COMPILER_TYPE} == "gcc" && ${OS} != "mac1013" ]]; then
    source ${CLICREPO}/compilers/gcc/7.2.0/x86_64-${OS}/setup.sh
fi
if [[ ${COMPILER_TYPE} == "llvm" && ${OS} != "mac1013" ]]; then
    source ${CLICREPO}/compilers/llvm/5.0.0/x86_64-${OS}/setup.sh
fi

#--------------------------------------------------------------------------------
#     CMake
#--------------------------------------------------------------------------------

export CMAKE_HOME=${CLICREPO}/software/CMake/3.9.5/${BUILD_FLAVOUR}
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python
#--------------------------------------------------------------------------------

export PYTHONDIR=${CLICREPO}/software/Python/2.7.14/${BUILD_FLAVOUR}
export PATH=${PYTHONDIR}/bin:$PATH
export LD_LIBRARY_PATH=${PYTHONDIR}/lib:${LD_LIBRARY_PATH}

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=${CLICREPO}/software/ROOT/6.12.04/${BUILD_FLAVOUR}
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=${CLICREPO}/software/Xerces-C/3.2.0/${BUILD_FLAVOUR}
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Geant4
#--------------------------------------------------------------------------------
#Determine which Geant4 version to use
if [ -z ${GEANT4_VERSION} ]; then
GEANT4_VERSION="10.03.p03"
fi

export G4INSTALL=${CLICREPO}/software/Geant4/${GEANT4_VERSION}/${BUILD_FLAVOUR}
export G4ENV_INIT="${G4INSTALL}/bin/geant4.sh"
export G4SYSTEM="Linux-g++"


#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------
export LCIO=${CLICREPO}/software/LCIO/2.11.0/${BUILD_FLAVOUR}
export PYTHONPATH=${LCIO}/python
export PATH=${LCIO}/bin:$PATH
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=${CLICREPO}/software/Boost/1.65.1/${BUILD_FLAVOUR}
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Ninja
#--------------------------------------------------------------------------------

export Ninja_HOME=${CLICREPO}/software/Ninja/1.8.2/${BUILD_FLAVOUR}
export PATH="$Ninja_HOME:$PATH"

#--------------------------------------------------------------------------------
#     Doxygen
#--------------------------------------------------------------------------------

export Doxygen_HOME=${CLICREPO}/software/Doxygen/1.8.13/${BUILD_FLAVOUR}
export PATH="$Doxygen_HOME/bin:$PATH"
