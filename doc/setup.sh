#
#  
#  ++++ Prerequisites/compiler setup
#  
export CMTCONFIG=x86_64-slc5-gcc43-dbg
. `which SetupProject.sh` Gaudi
#
. ./build/thisdd4hep.sh
#
#  ++++ Additional setup for ROOT, CLHEP, GEANT4 and XERCES
#  
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.00/${CMTCONFIG}/root;
export PATH=/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc5-gcc43-opt/bin:${ROOTSYS}/bin:$PATH
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH};
export G4SYS=/afs/cern.ch/sw/lcg/external/geant4/9.5.p01/x86_64-slc5-gcc43
export G4SYS=${HOME}/cmtuser/geant4/g4.9.5
export CLHEPSYS=/afs/cern.ch/sw/lcg/external/clhep/2.1.2.3/x86_64-slc5-gcc43-opt
export LD_LIBRARY_PATH=${G4SYS}/lib64:${LD_LIBRARY_PATH};
export PATH=${G4SYS}/bin:$PATH
export XERCESCSYS=/afs/cern.ch/sw/lcg/external/XercesC/3.1.1p1/x86_64-slc5-gcc43-opt
export G4LEDATA=${G4SYS}/share/Geant4-9.5.1/data/G4EMLOW6.23
export G4LEVELGAMMADATA=${G4SYS}/share/Geant4-9.5.1/data/PhotonEvaporation2.2
#
# QT for Geant4:
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/cern.ch/sw/lcg/external/qt/4.7.4/x86_64-slc5-gcc43-opt/lib
#
#
#
cd build
