#!/bin/bash
 
# General variables
export LCGRELEASES=/cvmfs/sft.cern.ch/lcg/releases/

#--------------------------------------------------------------------------------
#     GCC 4.8.4
#--------------------------------------------------------------------------------

source $LCGRELEASES/gcc/4.8.4/x86_64-cc7/setup.sh

#--------------------------------------------------------------------------------
#     CMake 3.4.1
#--------------------------------------------------------------------------------

export CMAKE_HOME=$LCGRELEASES/CMake/3.4.1-811c2/x86_64-centos7-gcc49-opt
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python 2.7.9
#--------------------------------------------------------------------------------

export PYTHONDIR=$LCGRELEASES/Python/2.7.9.p1-af54f/x86_64-centos7-gcc48-opt
export PATH=$PYTHONDIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH

#--------------------------------------------------------------------------------
#     pytools 1.9
#--------------------------------------------------------------------------------

export PYTOOLSDIR=$LCGRELEASES/pytools/1.9_python2.7-f55a5/x86_64-cc7-gcc48-opt
export PYTHONPATH=$PYTOOLSDIR/lib/python2.7/site-packages:$PYTHONPATH
export PATH=$PYTOOLSDIR/bin:$PATH

#--------------------------------------------------------------------------------
#     CLHEP
#--------------------------------------------------------------------------------

export CLHEP=$LCGRELEASES/clhep/2.3.1.1-6ba0c/x86_64-centos7-gcc48-opt
export CLHEP_BASE_DIR="$CLHEP"
export CLHEP_INCLUDE_DIR="$CLHEP/include"
export PATH="$CLHEP_BASE_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=$LCGRELEASES/ROOT/6.04.06-145b2/x86_64-cc7-gcc48-opt
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------

export LCIO="/afs/cern.ch/eng/clic/software/lcio/v02-08/x86_64-centos7-gcc48-opt"
export PYTHONPATH="$LCIO/src/python:$LCIO/examples/python:$PYTHONPATH"
export PATH="$LCIO/tools:$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Geant4	
#--------------------------------------------------------------------------------

export G4INSTALL=$LCGRELEASES/Geant4/10.01.p02-81fbd/x86_64-cc7-gcc48-opt/
export G4ENV_INIT="$G4INSTALL/bin/geant4.sh"
export G4SYSTEM="Linux-g++"

#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=$LCGRELEASES/Boost/1.59.0_python2.7-71952/x86_64-cc7-gcc48-opt/

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=$LCGRELEASES/XercesC/3.1.1p1-8ccd5/x86_64-cc7-gcc48-opt/
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"







