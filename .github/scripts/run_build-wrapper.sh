#!/bin/bash
source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh

curl --create-dirs -sSLo $HOME/.sonar/build-wrapper-linux-x86.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
unzip -o $HOME/.sonar/build-wrapper-linux-x86.zip -d $HOME/.sonar/
export PATH=$HOME/.sonar/build-wrapper-linux-x86:$PATH

cd /home/runner/work/DD4hep/DD4hep
mkdir build
cd build
unset CPATH  # causes implicit includes to be added in cmake, preventing setting some system libraries to -isystem
cmake -GNinja \
      -DDD4HEP_USE_GEANT4=ON \
      -DBoost_NO_BOOST_CMAKE=ON \
      -DDD4HEP_USE_LCIO=ON \
      -DDD4HEP_USE_TBB=ON \
      -DBUILD_TESTING=ON \
      -DDD4HEP_DEBUG_CMAKE=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always"  \
      -DCMAKE_CXX_STANDARD=17 .. && \
cd .. && \
build-wrapper-linux-x86-64 --out-dir bw-output cmake --build build/

BUILD_RESULT=$?
exit ${BUILD_RESULT}
