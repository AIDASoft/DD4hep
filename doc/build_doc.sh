#!/bin/bash
SW=/home/frankm/SW;
TARGET=${SW}/DD4hep_head_dbg.root_v5.34.10;
mkdir build_doc;
cd build_doc;
#
#
cmake -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=${TARGET}/DD4hep \
    -DCMAKE_MODULE_PATH=:${TARGET}/DD4hep \
    -DDD4hep_INCLUDE_DIRS=${TARGET}/DD4hep/include \
    -DDD4HEP_USE_XERCESC=ON \
    -DXERCESC_ROOT_DIR=${SW}/xercesc \
    -DDD4HEP_USE_BOOST=ON \
    -DDD4HEP_USE_LCIO=ON \
    -DLCIO_DIR=${SW}/lcio/v02-03-03 \
    -DGeant4_ROOT=${SW}/g4_9.6.p02_dbg \
    -DDD4HEP_USE_GEANT4=ON \
    -DDD4HEP_USE_PYROOT=OFF \
    -DBUILD_TESTING=ON \
    -DINSTALL_DOC=True \
    ..
#
#
#
make -j 5;
make -j 5 doc;
cd latex;
make -j 5;
cd ..;
echo "cd build_doc;scp -r ../doc/* ./html ./latex/refman.pdf frankm@lxplus:WWW/DD4hep/"
#
#
