#!/bin/bash

if [[ "${VIEW}" =~ "dev" ]]; then
    source /cvmfs/sft-nightlies.cern.ch/lcg/views/${VIEW}/setup.sh
else
    source /cvmfs/sft.cern.ch/lcg/views/${VIEW}/setup.sh
fi

cd /Package
mkdir build
cd build
unset CPATH  # causes implicit includes to be added in cmake, preventing setting some system libraries to -isystem
cmake -GNinja \
      -DDD4HEP_USE_GEANT4=ON \
      -DBoost_NO_BOOST_CMAKE=ON \
      -DDD4HEP_USE_LCIO=ON \
      -DDD4HEP_USE_TBB=${TBB} \
      -DDD4HEP_USE_HEPMC3=${HEPMC3:-OFF} \
      -DBUILD_TESTING=ON \
      -DDD4HEP_DEBUG_CMAKE=ON \
      -DBUILD_SHARED_LIBS=${SHARED} \
      -DDD4HEP_USE_XERCESC=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always"  \
      -DCMAKE_CXX_STANDARD=${STANDARD} .. && \
      ninja

BUILD_RESULT=$?

if [[ "${SHARED}" == "OFF" ]]; then
  echo "Not building SHARED libraries, ending tests here"
  exit ${BUILD_RESULT}
fi

test ${BUILD_RESULT} -eq 0 && \
ninja install && \
. ../bin/thisdd4hep.sh && \
ctest --output-on-failure -j4 && \
cd ../examples/ && \
mkdir build && \
cd build && \
cmake -GNinja \
      -DBoost_NO_BOOST_CMAKE=ON \
      -DDD4HEP_USE_XERCESC=ON \
      -DCMAKE_CXX_STANDARD=${STANDARD} .. && \
ninja && \
ninja install && \
ctest --output-on-failure -j2


