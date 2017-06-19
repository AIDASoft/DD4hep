#!/bin/bash
ARCH_OS=$1;
GCC_VSN=$2;
export LCG_external_area=/afs/cern.ch/sw/lcg/external;
if test -z "$GCC_VSN";
then
    export GCC_VSN=gcc46;
    echo "Using default gcc version:${GCC_VSN}";
fi;
if test -z "${ARCH_OS}";
then
    export ARCH_OS=slc6;
    echo "Using default architecture version:${ARCH_OS}";
fi;

if test -n "${ARCH_OS}";
then 
    if test -n "${GCC_VSN}";
    then
	svn co https://svnsrv.desy.de/basic/aidasoft/DD4hep/trunk DD4hep
        export BUILD_DIR=`pwd`/${ARCH_OS}-${GCC_VSN};
	export ARCH=x86_64-${ARCH_OS};
	if test $GCC_VSN = "gcc46";
	then 
	    . ${LCG_external_area}/gcc/4.6/${ARCH}/setup.sh /afs/cern.ch/sw/lcg/external;
	fi;
	if test $GCC_VSN = "gcc43";
	then
	    . ${LCG_external_area}/gcc/4.3/${ARCH}/setup.sh /afs/cern.ch/sw/lcg/external;
	fi;
	export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.04/${ARCH}-${GCC_VSN}-dbg/root;
	export LD_LIBRARY_PATH=PATH=${ROOTSYS}/bin:$LD_LIBRARY_PATH;
	export PATH=${ROOTSYS}/bin:$PATH;

	echo "CMAKE: `which cmake`   --  ${LCG_external_area}/CMake/2.8.6/${ARCH}-${GCC_VSN}-opt/bin";
	#echo "PATH=${PATH}";
	echo "`pwd`";
	mkdir ${BUILD_DIR};
	cd    ${BUILD_DIR};
	cmake -DCMAKE_BUILD_TYPE=Debug -DDD4HEP_USE_GEANT4=OFF -DDD4HEP_USE_PYROOT=OFF ${BUILD_DIR}/DD4hep;
	make -j 5;
	. thisdd4hep.sh;
	export LD_LIBRARY_PATH=${BUILD_DIR}/lib:${BUILD_DIR}/../DD4hep/examples/CLICSiD/lib:$LD_LIBRARY_PATH;
	./bin/geoDisplay file:../DD4hep/examples/CLICSiD/compact/compact_polycones.xml    
	echo "";
	echo "Did the simple test detector show up properly?";
	echo "";
    else
	echo "You must supply the architecture as first argument:";
	echo "$0 [os-version:slc5,slc6]  [gcc-version:gcc43,gcc46]";
    fi;
else
    echo "You must supply the architecture as first argument:";
    echo "$0 [os-version:slc5,slc6]  [gcc-version:gcc43,gcc46]";
fi;
