#!/bin/bash

########################################################################################################################
#
# Assuming you have CVMFS and run SLC6, CERN Centos7 or macOS Sierra you can bootstrap DD4hep by executing this script
#
########################################################################################################################


source ./.dd4hep-ci.d/init_x86_64.sh

STRING=$(env LC_CTYPE=C tr -dc "a-zA-Z0-9-_\$\?" < /dev/urandom | head -c 4)

mkdir build_${STRING}
cd build_${STRING}
if [ "$(uname)" == "Darwin" ]; then
    cmake -GNinja -D DD4HEP_USE_GEANT4=ON -D DD4HEP_USE_BOOST=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON -D Geant4_DIR=$G4INSTALL/lib/Geant4-10.2.2 -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release ..
else
    cmake -GNinja -D DD4HEP_USE_GEANT4=ON -D DD4HEP_USE_BOOST=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON -D Geant4_DIR=$G4INSTALL/lib64/Geant4-10.2.2 -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release ..
fi
ninja
ninja install
../bin/thisdd4hep.sh
ctest --output-on-failure -j4
cd ../examples/
mkdir build_${STRING}
cd build
cmake -GNinja ..
ninja
ninja install
ctest --output-on-failure