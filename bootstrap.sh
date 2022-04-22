#!/bin/bash

########################################################################################################################
#
# Assuming you have CVMFS and run SLC6, CERN Centos7 or macOS Sierra you can bootstrap dd4hep by executing this script
#
########################################################################################################################


source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc11-opt/setup.sh

STRING=$(env LC_CTYPE=C tr -dc "a-zA-Z0-9-_\$\?" < /dev/urandom | head -c 4)

mkdir build_${STRING}
cd build_${STRING}
if [ "$(uname)" == "Darwin" ]; then
    cmake -GNinja -D DD4HEP_USE_GEANT4=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release -DROOT_DIR=$ROOTSYS ..
else
    cmake -GNinja -D DD4HEP_USE_GEANT4=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release -DROOT_DIR=$ROOTSYS ..
fi
ninja
ninja install
source ../bin/thisdd4hep.sh
ctest --output-on-failure -j4
cd ../examples/
mkdir build_${STRING}
cd build_${STRING}
cmake -GNinja ..
ninja
ninja install
ctest --output-on-failure
