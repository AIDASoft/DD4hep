#!/bin/bash

export LCGEXTERNAL=/afs/cern.ch/sw/lcg/external
export LCGRELEASES=/afs/cern.ch/sw/lcg/app/releases

#--------------------------------------------------------------------------------
# CMAKE
#--------------------------------------------------------------------------------
export CMAKE_HOME=$LCGEXTERNAL/CMake/2.8.8/x86_64-slc6-gcc46-opt
export PATH=${CMAKE_HOME}/bin:$PATH


#--------------------------------------------------------------------------------
#     GCC
#--------------------------------------------------------------------------------

gcc_config_version=4.8.1
mpfr_config_version=3.1.2
gmp_config_version=5.1.1
LCGPLAT=x86_64-slc6-gcc48-opt
LCG_lib_name=lib64
LCG_arch=x86_64

if [ -z $1 ] 
then LCG_contdir=/afs/cern.ch/sw/lcg/contrib
else LCG_contdir=$1 
fi
LCG_gcc_home=${LCG_contdir}/gcc/${gcc_config_version}/${LCGPLAT}
LCG_mpfr_home=${LCG_contdir}/mpfr/${mpfr_config_version}/${LCGPLAT}
LCG_gmp_home=${LCG_contdir}/gmp/${gmp_config_version}/${LCGPLAT}

export PATH=${LCG_gcc_home}/bin:${PATH}
export COMPILER_PATH=${LCG_gcc_home}/lib/gcc/${LCG_arch}-unknown-linux-gnu/${gcc_config_version}

if [ ${LD_LIBRARY_PATH} ] 
then
export LD_LIBRARY_PATH=${LCG_gcc_home}/${LCG_lib_name}:${LCG_mpfr_home}/lib:${LCG_gmp_home}/lib:${LD_LIBRARY_PATH}
else
export LD_LIBRARY_PATH=${LCG_gcc_home}/${LCG_lib_name}:${LCG_mpfr_home}/lib:${LCG_gmp_home}/lib
fi

### ROOT
source $LCGRELEASES/ROOT/5.34.10/x86_64-slc6-gcc48-dbg/root/bin/thisroot.sh


#--------------------------------------------------------------------------------
#     Geant4
#--------------------------------------------------------------------------------
source /afs/cern.ch/sw/lcg/external/geant4/9.6.p01/x86_64-slc6-gcc46-opt/bin/geant4.sh
#--------------------------------------------------------------------------------
#     XercesC
#--------------------------------------------------------------------------------
export XERCESCSYS=/afs/cern.ch/sw/lcg/external/XercesC/3.1.1p2/x86_64-slc6-gcc46-opt
export LD_LIBRARY_PATH=${XERCESCSYS}/lib:${LD_LIBRARY_PATH}

# #--------------------------------------------------------------------------------
# #     CLHEP
# #--------------------------------------------------------------------------------
# export CLHEP="/afs/cern.ch/eng/clic/software/x86_64-slc6-gcc48/CLHEP/2.1.3.1"
# export CLHEP_BASE_DIR="$CLHEP"
# export CLHEP_INCLUDE_DIR="$CLHEP/include"
# export PATH="$CLHEP_BASE_DIR/bin:$PATH"
# export LD_LIBRARY_PATH="$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#     LCIO
#--------------------------------------------------------------------------------
export LCIO="/afs/cern.ch/eng/clic/software/x86_64-slc6-gcc48/LCIO/v02-04-01"
export LCIO_INCLUDE_DIR="$LCIO/include"
export PATH="$LCIO/bin:$PATH"
export LD_LIBRARY_PATH="$LCIO/lib:$LD_LIBRARY_PATH"


#--------------------------------------------------------------------------------
#        BOOST
#--------------------------------------------------------------------------------
export BOOST_ROOT=/afs/cern.ch/sw/lcg/external/Boost/1.53.0_python2.7/x86_64-slc6-gcc48-opt




