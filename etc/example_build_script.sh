#!/bin/bash
#
# ====  User configuration =========================================
# Set here the proper installation directories of the pre-requisites
# ==================================================================
SW=~/SW
MY_ROOT=${SW}/root_v5.34.25_dbg
MY_GEANT4=${SW}/g4_10.01.p02_dbg
MY_LCIO=${SW}/lcio/v02-04-03
MY_XERCES=${SW}/xerces-c-3.1.1
MY_DD4HEP=`pwd`/DD4hep_tmp2
# ==================================================================
# From here on all should be box-standard
# ==================================================================
. ${MY_ROOT}/bin/thisroot.sh
. ${MY_GEANT4}/bin/geant4.sh          # Remove line if no DDG4

mkdir -p ${MY_DD4HEP}/build
mkdir -p ${MY_DD4HEP}/install
cd ${MY_DD4HEP}
svn co https://svnsrv.desy.de/basic/aidasoft/DD4hep/trunk

cd    build
cmake -DCMAKE_BUILD_TYPE=Debug                                 \
      -DCMAKE_INSTALL_PREFIX=${MY_DD4HEP}/install              \
      -DCMAKE_MODULE_PATH=${MY_DD4HEP}/install                 \
      -DDD4HEP_USE_XERCESC=ON -DXERCESC_ROOT_DIR=${MY_XERCES}  \
      -DDD4HEP_USE_BOOST=ON -DDD4HEP_USE_GEANT4=ON -DGeant4_ROOT=${MY_GEANT4}        \
      -DDD4HEP_USE_LCIO=ON -DLCIO_DIR=${MY_LCIO}               \
      ../trunk

# Build and install
make -j 6 install

# Move to installation and setup runtime environment
cd ${MY_DD4HEP}/install
. bin/thisdd4hep.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${MY_LCIO}/lib

# Run Example...
python examples/DDG4/examples/CLICSidSimu.py
