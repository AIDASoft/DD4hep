#!/bin/bash
 
# General variables
export LCGRELEASES=/cvmfs/sft.cern.ch/lcg/releases/LCG_84

#--------------------------------------------------------------------------------
#     GCC 4.9.3
#--------------------------------------------------------------------------------

source $LCGRELEASES/gcc/4.9.3/x86_64-centos7/setup.sh

#--------------------------------------------------------------------------------
#     CMake 3.4.1
#--------------------------------------------------------------------------------

export CMAKE_HOME=$LCGRELEASES/CMake/3.4.1/x86_64-centos7-gcc49-opt
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     Python 2.7.10
#--------------------------------------------------------------------------------

export PYTHONDIR=$LCGRELEASES/Python/2.7.10/x86_64-centos7-gcc49-opt
export PATH=$PYTHONDIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH

#--------------------------------------------------------------------------------
#     pytools 1.9
#--------------------------------------------------------------------------------

export PYTOOLSDIR=$LCGRELEASES/1.9_python2.7/x86_64-centos7-gcc49-opt
export PYTHONPATH=$PYTOOLSDIR/lib/python2.7/site-packages:$PYTHONPATH
export PATH=$PYTOOLSDIR/bin:$PATH

#--------------------------------------------------------------------------------
#     CLHEP
#--------------------------------------------------------------------------------

export CLHEP=$LCGRELEASES/clhep/2.3.1.1/x86_64-centos7-gcc49-opt
export CLHEP_BASE_DIR="$CLHEP"
export CLHEP_INCLUDE_DIR="$CLHEP/include"
export PATH="$CLHEP_BASE_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=$LCGRELEASES/ROOT/6.06.02/x86_64-centos7-gcc49-opt
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------

export LCIO="/afs/cern.ch/eng/clic/software/lcio/v02-08/x86_64-centos7-gcc49-opt"
export PYTHONPATH="$LCIO/src/python:$LCIO/examples/python:$PYTHONPATH"
export PATH="$LCIO/tools:$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Geant4	
#--------------------------------------------------------------------------------

export G4INSTALL=$LCGRELEASES/../Geant4/10.02-7c0e3/x86_64-centos7-gcc49-opt
export G4ENV_INIT="$G4INSTALL/bin/geant4.sh"
export G4SYSTEM="Linux-g++"

#--------------------------------------------------------------------------------
#     QT
#--------------------------------------------------------------------------------

export QTDIR=$LCGRELEASES/qt/4.8.4/x86_64-centos7-gcc49-opt
export QMAKESPEC="$QTDIR/mkspecs/linux-g++"
export PATH="$QTDIR/bin:$PATH"
export LD_LIBRARY_PATH="$QTDIR/lib:$LD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=$LCGRELEASES/Boost/1.59.0_python2.7/x86_64-centos7-gcc49-opt

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=$LCGRELEASES/XercesC/3.1.1p1/x86_64-centos7-gcc49-opt
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"
