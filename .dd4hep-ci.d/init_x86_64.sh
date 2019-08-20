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
if [ -z ${COMPILER_VERSION} ]; then
    if [ ${COMPILER_TYPE} == "gcc" ]; then
        COMPILER_VERSION="gcc8"
    fi
    if [[ ${COMPILER_TYPE} == "clang" ]]; then
        COMPILER_VERSION="clang8"
    fi
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

echo /cvmfs/sft.cern.ch/lcg/views/${LCG_RELEASE}/${BUILD_FLAVOUR}/setup.sh
source /cvmfs/sft.cern.ch/lcg/views/${LCG_RELEASE}/${BUILD_FLAVOUR}/setup.sh


#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------
export LCIO=/cvmfs/clicdp.cern.ch/software/LCIO/2.12.1/x86_64-${OS}-${COMPILER_VERSION}-LCG_96
export LCIO_ROOT=${LCIO}
export PYTHONPATH=${LCIO}/python:$PYTHONPATH
export PATH=${LCIO}/bin:$PATH
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"
