#!/bin/bash
ARCH_OS=$1;
GCC_VSN=$2;
export LCG_external_area=/afs/cern.ch/sw/lcg/external;
if test -z "$GCC_VSN";
    then
    GCC_VSN=gcc46;
    echo "Using default gcc version:${GCC_VSN}";
    fi;
if test -z "$1";
    then
    echo "You must supply the architecture as first argument:";
    echo "$0 [os-version:slc5,slc6]  [gcc-version:gcc43,gcc46]";
else  
    svn co https://svnsrv.desy.de/basic/aidasoft/DD4hep/trunk DD4hep
    mkdir ${ARCH_OS};
    cd    ${ARCH_OS};
    export ARCH=x86_64-${ARCH_OS};
    if test $GCC_VSN = "46";
	then 
	. ${LCG_external_area}/gcc/4.6/${ARCH}/setup.sh /afs/cern.ch/sw/lcg/external;
	fi;
    if test $GCC_VSN = "43";
	then
	. ${LCG_external_area}/gcc/4.3/${ARCH}/setup.sh /afs/cern.ch/sw/lcg/external;
	fi;
    export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.04/${ARCH}-${GCC_VSN}-dbg/root;
    ls -laF ${ROOTSYS}/bin
    export PATH=${LCG_external_area}/CMake/2.8.6/${ARCH}-${GCC_VSN}-opt/bin:${ROOTSYS}/bin:$PATH;
    echo "CMAKE: `which cmake`   --  ${LCG_external_area}/CMake/2.8.6/${ARCH}-${GCC_VSN}-opt/bin"
    echo $PATH
    cmake -DCMAKE_BUILD_TYPE=Debug -DDD4HEP_WITH_GEANT4=OFF -DDD4HEP_USE_PYROOT=OFF ../DD4hep;
    . thisdd4hep.sh;
    make -j 5;
    echo "Now run this command to test the installation:";
    echo "geoDisplay file:../DD4hep/DDExamples/ILDExDet/compact/CLIC_ILD_CDR.xml";
    ./bin/geoDisplay file:../DD4hep/DDExamples/ILDExDet/compact/CLIC_ILD_CDR.xml
    
fi;
