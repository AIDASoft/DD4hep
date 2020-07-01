#!/bin/bash

source /DD4hep/.dd4hep-ci.d/init_x86_64.sh

cd /DD4hep
mkdir build
cd build
unset CPATH  # causes implicit includes to be added in cmake, preventing setting some system libraries to -isystem
cmake -GNinja -DDD4HEP_USE_GEANT4=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DDD4HEP_USE_LCIO=ON \
    -DDD4HEP_USE_TBB=${DD4HEP_USE_TBB:-OFF} \
    -DBUILD_TESTING=ON \
    -DDD4HEP_DEBUG_CMAKE=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always -Werror"  \
    -DBUILD_SHARED_LIBS=${SHARED} \
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
cmake -GNinja -DBoost_NO_BOOST_CMAKE=ON -DCMAKE_CXX_STANDARD=${STANDARD} .. && \
ninja && \
ninja install && \
ctest --output-on-failure -j2
