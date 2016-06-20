#!/bin/bash


if [ "$( cat /etc/*-release | grep Scientific )" ]; then
    OS=slc6
elif [ "$( cat /etc/*-release | grep CentOS )" ]; then
    OS=centos7
else
    echo "UNKNOWN OS"
    exit 1
fi


if [ -z ${GCC_VERSION} ]; then
    GCC_VERSION=4.9.3
fi
  
if [ -z ${BUILD_TYPE} ]; then
    BUILD_TYPE=opt
fi

GCC_VER=`echo ${GCC_VERSION} | sed -e 's/\.//g' | cut -c 1-2`

# General variables
CLICREPO=/cvmfs/clicdp.cern.ch
BUILD_FLAVOUR=x86_64-${OS}-gcc${GCC_VER}-${BUILD_TYPE}
#--------------------------------------------------------------------------------
#     GCC
#--------------------------------------------------------------------------------

source ${CLICREPO}/compilers/gcc/${GCC_VERSION}/x86_64-${OS}/setup.sh

#--------------------------------------------------------------------------------
#     CMake
#--------------------------------------------------------------------------------

export CMAKE_HOME=${CLICREPO}/software/CMake/3.5.2/${BUILD_FLAVOUR}
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python
#--------------------------------------------------------------------------------

export PYTHONDIR=${CLICREPO}/software/Python/2.7.11/${BUILD_FLAVOUR}
export PATH=$PYTHONDIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=${CLICREPO}/software/ROOT/6.06.04/${BUILD_FLAVOUR}
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     CLHEP
#--------------------------------------------------------------------------------

export CLHEP=${CLICREPO}/software/CLHEP/2.2.0.4/${BUILD_FLAVOUR}
export CLHEP_BASE_DIR="$CLHEP"
export CLHEP_INCLUDE_DIR="$CLHEP/include"
export PATH="$CLHEP_BASE_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=${CLICREPO}/software/Xerces-C/3.1.3/${BUILD_FLAVOUR}
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Geant4	
#--------------------------------------------------------------------------------

export G4INSTALL=${CLICREPO}/software/Geant4/10.01.p03/${BUILD_FLAVOUR}
export G4ENV_INIT="$G4INSTALL/bin/geant4.sh"
export G4SYSTEM="Linux-g++"


#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------
export LCIO=${CLICREPO}/software/LCIO/2.7.1/${BUILD_FLAVOUR}
# export PYTHONPATH="$LCIO/src/python:$LCIO/examples/python:$PYTHONPATH"
export PATH="$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=${CLICREPO}/software/Boost/1.61.0/${BUILD_FLAVOUR}
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:$LD_LIBRARY_PATH"



