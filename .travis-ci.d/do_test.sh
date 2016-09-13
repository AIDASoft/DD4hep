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
    GCC_VERSION=4.8.5
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

export PYTHONDIR=${CLICREPO}/software/Python/2.7.12/${BUILD_FLAVOUR}
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
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=${CLICREPO}/software/Xerces-C/3.1.3/${BUILD_FLAVOUR}
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     Geant4	
#--------------------------------------------------------------------------------

export G4INSTALL=${CLICREPO}/software/Geant4/10.02.p02/${BUILD_FLAVOUR}
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

#--------------------------------------------------------------------------------
#     Ninja
#--------------------------------------------------------------------------------

export Ninja_HOME=${CLICREPO}/software/Ninja/1.7.1/${BUILD_FLAVOUR}
export PATH="$Ninja_HOME:$PATH"


#--------------------------------------------------------------------------------
#     BUILD
#--------------------------------------------------------------------------------

export GCC_VERSION="4.8.5"
export BUILD_TYPE="opt"
cd /DD4hep
mkdir build
cd build
cmake -GNinja -D DD4HEP_USE_GEANT4=ON -D DD4HEP_USE_BOOST=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON  -D Geant4_DIR=$G4INSTALL/lib64/Geant4-10.2.2 -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Debug ..
ninja
ninja install
. ../bin/thisdd4hep.sh
ctest --output-on-failure -j4
cd ../examples/
mkdir build
cd build
cmake -GNinja ..
ninja
ninja install
ctest --output-on-failure
