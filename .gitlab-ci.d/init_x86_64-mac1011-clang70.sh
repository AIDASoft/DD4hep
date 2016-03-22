#!/bin/bash
 
# General variables
export MACRELEASES=/Users/gitlab-runner/CI

#--------------------------------------------------------------------------------
#     CLHEP
#--------------------------------------------------------------------------------

export CLHEP=$MACRELEASES/clhep-2.3.1.0
export CLHEP_BASE_DIR="$CLHEP"
export CLHEP_INCLUDE_DIR="$CLHEP/include"
export PATH="$CLHEP_BASE_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Geant4    
#--------------------------------------------------------------------------------

export G4INSTALL=$MACRELEASES/geant4.10.02
export G4ENV_INIT="$G4INSTALL/bin/geant4.sh"
export G4SYSTEM="Linux-g++"

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=$MACRELEASES/root-6.06.02
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="$ROOTSYS/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------

export LCIO=$MACRELEASES/lcio
export PYTHONPATH="$LCIO/src/python:$LCIO/examples/python:$PYTHONPATH"
export PATH="$LCIO/tools:$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="$LCIO/lib:$DYLD_LIBRARY_PATH"

#--------------------------------------------------------------------------------
#     Boost
#--------------------------------------------------------------------------------

export BOOST_ROOT=$MACRELEASES/boost_1.60

#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------

export XercesC_HOME=$MACRELEASES/xerces-c-3.1.3
export PATH="$XercesC_HOME/bin:$PATH"
export LD_LIBRARY_PATH="$XercesC_HOME/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="$XercesC_HOME/lib:$DYLD_LIBRARY_PATH"
