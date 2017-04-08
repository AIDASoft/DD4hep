#!/bin/bash

export COMPILER_TYPE="gcc"

source /Package/.dd4hep-ci.d/init_x86_64.sh

cd /Package
mkdir build
cd build
cmake -D DD4HEP_USE_GEANT4=ON -D DD4HEP_USE_BOOST=ON -DBoost_NO_BOOST_CMAKE=ON -D DD4HEP_USE_LCIO=ON -D BUILD_TESTING=ON  -D Geant4_DIR=$G4INSTALL/lib64/Geant4-10.2.2 -D DD4HEP_USE_CXX11=ON -DCMAKE_BUILD_TYPE=Release -DROOT_DIR=$ROOTSYS -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always" .. && \
export PATH=/cov-analysis-linux64/bin:$PATH && \
cov-build --dir cov-int make -j2 && \
tar czvf myproject.tgz cov-int
