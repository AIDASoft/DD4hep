dir_name=`pwd`;
SW=/home/frankm/SW;

INSTALL_LCIO=${SW}/lcio/v02-04-03;
INSTALL_XERCESC=${SW}/xercesc;

INSTALL_G4=${SW}/g4_10.01.p02_dbg/lib/Geant4-10.1.2;
CHECKOUT=${dir_name}/../../DD4hep.trunk/checkout;
GEANT_VERSION=10.01.p02;
#ROOT_VERSION=5.34.25

GEANT_VERSION=10.02.p02;
GEANT_VERSION=10.03.p02;
INSTALL_G4=${SW}/g4_${GEANT_VERSION}_dbg/lib/Geant4-10.3.2;
#ROOT_VERSION=6.06.06;
ROOT_VERSION=6.08.00;
ROOT_VERSION=6.10.04;
ROOT_VERSION=6.12.06;
source ${INSTALL_G4}/../../bin/geant4.sh;
num_threads=2
#
# Root version < 6.08 : 
#export CXX="`which g++-5` -D_GLIBCXX_USE_CXX11_ABI=0";
#export CC="`which gcc-5` -D_GLIBCXX_USE_CXX11_ABI=0";
#
export CXX="`which g++-5`";
export CC="`which gcc-5`";

# ==============================================================================
# Parse arguments
# ==============================================================================
parse_command_line_args()
{
    MULTITHREADED="";
    ARG_ERROR="";
    PLATFORMS="dbg opt";
    while [[ "$1" == -* ]]; do
        #echo "Arg:$1 $2";
        a1=`echo $1 | tr A-Z a-z`;
	case ${a1} in

	    -root)
		ROOT_VERSION=$2;
		shift
		;;

	    -geant4)
		GEANT_VERSION=$2;
		shift
		;;

	    -build)
		PLATFORMS="$2";
		shift
		;;

            -mt)
                MULTITHREADED="YES";
                ;;

            -st)
                MULTITHREADED="";
                ;;

	    -threads)
		num_threads=$2;
		shift
		;;

	    -j)
		num_threads=$2;
		shift
		;;

	    *)
		ARG_ERROR=$1;
		;;
	esac
	shift
    done
}
#cat ${ROOTSYS}/bin/thisroot.sh;
#
LINE="===================================================================================================";
BARS="=====  ";
#
make_output()
{
    echo "${LINE}";
    echo "${BARS}";
    echo "=====>>     `pwd` ";
    if test -n "${1}"; then 
        echo " +++++ ${1}";
    fi;
    if test -n "${2}"; then 
        echo " +++++ ${2}";
    fi;
    echo "${BARS}";
    echo "${LINE}";
}

make_opt()
{
    if test "$1" = "ON"; then
        arg="${2}=${1}";shift;shift;
	echo ${arg} $*;
        #else
        #	echo ${1};
    fi;
}

make_build()
{
    echo ${CMD};
    eval ${CMD};
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed CMAKE command:"
        echo ${CMD};
	exit 1
    fi
    make install VERBOSE=1 -j ${num_threads};
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed BUILD!"
        echo ${CMD};
	exit 1
    fi;
    make test;
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed TESTS!"
        echo ${CMD};
	exit 1
    fi;
    echo "${LINE}";
    echo "${BARS}";
    echo "=======>>  Successfully build configuration `pwd` ";
    echo "${BARS}";
    echo "${LINE}";
}

build_all()
{
    export Geant4_DIR=${SW}/g4_${GEANT_VERSION}_dbg;
    DEF_MODES="ON OFF";
    G4_MODES="${DEF_MODES}";
    XERCES_MODES="${DEF_MODES}";
    LCIO_MODES="${DEF_MODES}";
    XERCES_MODES="OFF";
    G4_MODES="ON";
    LCIO_MODES="OFF";
    BUILD_TYPE=;
    BUILD_TYPE="-DCMAKE_BUILD_TYPE=Debug";
    for DOGEANT4 in ${G4_MODES}; do
	for DOXERCESC in ${XERCES_MODES}; do
	    for DOGEAR in OFF; do
		for DOLCIO in ${LCIO_MODES}; do
		    folder=build_Xer${DOXERCESC}_Geant${DOGEANT4}_Gear${DOGEAR}_Lcio${DOLCIO}
                    WORK_DIR=${dir_name}/${folder};
		    mkdir -p ${WORK_DIR}/EX;
		    cd ${WORK_DIR};
                    make_output;
                    #-DDD4HEP_DEBUG_CMAKE=ON \
		    unset DD4hep_DIR;
                    OPTS="`make_opt ${DOGEANT4} -DDD4HEP_USE_GEANT4 -DGeant4_DIR=${INSTALL_G4}`\
		    `make_opt ${DOLCIO}     -DDD4HEP_USE_LCIO -DLCIO_DIR=${INSTALL_LCIO}` \
		    `make_opt ${DOXERCESC}  -DDD4HEP_USE_XERCESC -DXERCESC_ROOT_DIR=${INSTALL_XERCESC}` \
                    -DROOTSYS=${ROOTSYS} -DCMAKE_INSTALL_PREFIX=${WORK_DIR}/DD4hep";
		    CMD="cd ${dir_name}/$folder ; cmake ${BUILD_TYPE} ${OPTS} ${CHECKOUT};";
                    make_build;

		    unset ROOTSYS;
                    DD4hep_DIR=${WORK_DIR}/DD4hep;

                    OPTS_ex="`make_opt ${DOGEANT4} -DDD4HEP_USE_GEANT4 -DGeant4_DIR=${INSTALL_G4}`\
		    `make_opt ${DOLCIO}     -DDD4HEP_USE_LCIO    -DLCIO_DIR=${INSTALL_LCIO}` \
		    `make_opt ${DOXERCESC}  -DDD4HEP_USE_XERCESC -DXERCESC_ROOT_DIR=${INSTALL_XERCESC}` \
                    -DROOTSYS=${ROOTSYS}";
		    source ${DD4hep_DIR}/bin/thisdd4hep.sh;
   		    CMD="cd ${WORK_DIR}/EX; cmake ${BUILD_TYPE} ${OPTS} -DDD4hep_DIR=${DD4hep_DIR} ${CHECKOUT}/examples;";
                    make_build;
                    #
		    cd ../..;
		done;
	    done;
	done;
    done;
}


parse_command_line_args $*;
if [ "$ARG_ERROR" != "" ]; then
    echo "Unknown option: ERROR CONDITION";
    exit 0;
elif [ "$ROOT_VERSION" == "" ] || [ "$PLATFORMS" == "" ]; then
    echo "ERROR CONDITION ROOT_VERSION=${ROOT_VERSION}  PLATFORMS=${PLATFORMS}";
    exit 0;
fi;
export INSTALL_G4;
export ROOTSYS=${SW}/root_v${ROOT_VERSION}_dbg;
. ${ROOTSYS}/bin/thisroot.sh;
#
#
build_all;
#
#
#
