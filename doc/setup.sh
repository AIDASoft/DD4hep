#!/bin/bash
#  Prerequisites:
export ARCH=x86_64-slc5;
export CMTCONFIG=${ARCH}-gcc46-dbg;
#
. /afs/cern.ch/sw/lcg/external/gcc/4.6.2/${ARCH}/setup.sh /afs/cern.ch/sw/lcg/external;
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.04/${CMTCONFIG}/root;
export PATH=/afs/cern.ch/sw/lcg/external/CMake/2.8.6/${ARCH}-gcc46-opt/bin:${ROOTSYS}/bin:$PATH;
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH};
export XERCESCSYS=/afs/cern.ch/sw/lcg/external/XercesC/3.1.1p1/${ARCH}-gcc46-opt;
export G4SYS=/afs/cern.ch/sw/lcg/external/geant4/9.5.p01/x86_64-slc5-gcc43
export G4SYS=${HOME}/cmtuser/geant4/g4.9.5
export CLHEPSYS=/afs/cern.ch/sw/lcg/external/clhep/2.1.2.3/${CMTOPT}
export LD_LIBRARY_PATH=${G4SYS}/lib64:${LD_LIBRARY_PATH};
export PATH=${G4SYS}/bin:$PATH
export G4LEDATA=${G4SYS}/share/Geant4-9.5.1/data/G4EMLOW6.23
export G4LEVELGAMMADATA=${G4SYS}/share/Geant4-9.5.1/data/PhotonEvaporation2.2
#
# QT for Geant4:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/cern.ch/sw/lcg/external/qt/4.7.4/${CMTOPT}/lib
#
. ./build/thisdd4hep.sh
#
cd build
