#!/bin/bash

export GCC_VERSION="4.8.5"
export BUILD_TYPE="opt"

source /DD4hep/.travis-ci.d/init_x86_64.sh


cd /DD4hep
mkdir build
cd build
cmake -GNinja -D DD4HEP_USE_GEANT4=ON -D DD4HEP_USE_BOOST=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON  -D Geant4_DIR=$G4INSTALL/lib64/Geant4-10.2.2 -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release ..
ninja
ninja install
. ../bin/thisdd4hep.sh
ctest --output-on-failure -j4
cd ../examples/
mkdir build
cd build
cmake -GNinja -DBoost_NO_BOOST_CMAKE=ON ..
ninja
ninja install
ctest --output-on-failure
