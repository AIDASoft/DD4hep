#!/bin/bash

#Determine which OS you are using
if [ "$(uname)" == "Linux" ]; then
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

if [ ! -d "/cvmfs/sft.cern.ch" ]; then
    echo "No sft CVMFS repository detected, please add it."
    exit 1
fi


#Determine which compiler to use
if [ -z ${COMPILER_TYPE} ]; then
    COMPILER_TYPE="gcc"
fi
if [ ${COMPILER_TYPE} == "gcc" ]; then
    COMPILER_VERSION="gcc8"
fi
if [[ ${COMPILER_TYPE} == "clang" ]]; then
    COMPILER_VERSION="clang8"
fi


#Choose build type
if [ -z ${BUILD_TYPE} ]; then
    BUILD_TYPE=opt
fi


# General variables
if [ -z ${LCG_RELEASE} ]; then
    LCG_RELEASE="LCG_96"
fi

LCGREPO=/cvmfs/sft.cern.ch/lcg/releases/${LCG_RELEASE}
BUILD_FLAVOUR=x86_64-${OS}-${COMPILER_VERSION}-${BUILD_TYPE}

export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/views/${LCG_RELEASE}/${BUILD_FLAVOUR}/lib64:/cvmfs/sft.cern.ch/lcg/views/${LCG_RELEASE}/${BUILD_FLAVOUR}/lib:$LD_LIBRARY_PATH

#--------------------------------------------------------------------------------
#     Compiler
#--------------------------------------------------------------------------------

if [[ ${COMPILER_TYPE} == "gcc" ]]; then
    source /cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-${OS}/setup.sh
fi
if [[ ${COMPILER_TYPE} == "clang" ]]; then
    source /cvmfs/sft.cern.ch/lcg/releases/clang/8.0.0-ed577/x86_64-${OS}/setup.sh
fi

#--------------------------------------------------------------------------------
#     CMake
#--------------------------------------------------------------------------------

export CMAKE_HOME=${LCGREPO}/CMake/3.14.3/${BUILD_FLAVOUR}
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python
#--------------------------------------------------------------------------------
if [[ $LCG_RELEASE =~ "python3" ]]; then
    export PYTHONDIR=${LCGREPO}/Python/3.6.5/${BUILD_FLAVOUR}
else
    export PYTHONDIR=${LCGREPO}/Python/2.7.16/${BUILD_FLAVOUR}
fi
export PATH=${PYTHONDIR}/bin:$PATH
export LD_LIBRARY_PATH=${PYTHONDIR}/lib:${LD_LIBRARY_PATH}

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=${LCGREPO}/ROOT/6.18.00/${BUILD_FLAVOUR}
export ROOT_ROOT=$ROOTSYS
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_ROOT=${LCGREPO}/XercesC/3.1.3/${BUILD_FLAVOUR}
export PATH="$XercesC_ROOT/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_ROOT/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Geant4
#--------------------------------------------------------------------------------

export Geant4_ROOT=${LCGREPO}/Geant4/10.05.p01/${BUILD_FLAVOUR}
export G4LIB=$Geant4_ROOT/lib/Geant4-10.5.1/
export G4ENV_INIT="${Geant4_ROOT}/bin/geant4.sh"
export G4SYSTEM="Linux-g++"


#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------
export LCIO=/cvmfs/clicdp.cern.ch/software/LCIO/2.12.1/x86_64-${OS}-${COMPILER_VERSION}-LCG_96
export PYTHONPATH=${LCIO}/python
export PATH=${LCIO}/bin:$PATH
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=${LCGREPO}/Boost/1.70.0/${BUILD_FLAVOUR}
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Ninja
#--------------------------------------------------------------------------------

export Ninja_HOME=${LCGREPO}/ninja/1.9.0/${BUILD_FLAVOUR}
export PATH="$Ninja_HOME/bin:$PATH"

#--------------------------------------------------------------------------------
#     Doxygen
#--------------------------------------------------------------------------------

export Doxygen_HOME=${LCGREPO}/doxygen/1.8.15/${BUILD_FLAVOUR}
export PATH="$Doxygen_HOME/bin:$PATH"
