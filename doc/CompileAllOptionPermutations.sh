dir_name=`pwd`;
SW=/home/frankm/SW;

INSTALL_G4=${SW}/g4_10.01.p02_dbg/lib/Geant4-10.1.2;
INSTALL_LCIO=${SW}/lcio/v02-04-03;
INSTALL_XERCESC=${SW}/xercesc;
CHECKOUT=${dir_name}/../../DD4hep.trunk/checkout;
export ROOTSYS=${SW}/root_v5.34.25_dbg;
export ROOTSYS=${SW}/root_v6.04.00_dbg;
. ${ROOTSYS}/bin/thisroot.sh;
#cat ${ROOTSYS}/bin/thisroot.sh;
#
LINE="==================================================================================================="
#
make_output()
{
    echo "${LINE}";
    echo "${LINE}";
    echo "=============================== `pwd` ";
    echo "${LINE}";
    echo "${LINE}";
    if test -n "${1}"; then 
        echo " +++++ ${1}";
    fi;
    if test -n "${2}"; then 
        echo " +++++ ${2}";
    fi;
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
    echo ${fCMD};
    eval ${CMD};
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed CMAKE command:"
        echo ${CMD};
	exit 1
    fi
    make install VERBOSE=1 -j 5;
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed BUILD:"
        echo ${CMD};
	exit 1
    fi;
    make test;
    if [ $? -ne  0 ]; then
        make_output "DANGER WILL ROBINSON DANGER!" "++++ Failed TESTS:"
        echo ${CMD};
	#exit 1
    fi;
}

build_all()
{
    for DOGEANT4 in OFF ON; do
	for DOXERCESC in OFF ON; do
	    for DOGEAR in OFF; do
		for DOLCIO in OFF ON; do
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
		    CMD="cd ${dir_name}/$folder ; cmake ${OPTS} ${CHECKOUT};";
                    make_build;

		    unset ROOTSYS;
                    DD4hep_DIR=${WORK_DIR}/DD4hep;

                    OPTS_ex="`make_opt ${DOGEANT4} -DDD4HEP_USE_GEANT4 -DGeant4_DIR=${INSTALL_G4}`\
		    `make_opt ${DOLCIO}     -DDD4HEP_USE_LCIO    -DLCIO_DIR=${INSTALL_LCIO}` \
		    `make_opt ${DOXERCESC}  -DDD4HEP_USE_XERCESC -DXERCESC_ROOT_DIR=${INSTALL_XERCESC}` \
                    -DROOTSYS=${ROOTSYS}";
		    source ${DD4hep_DIR}/bin/thisdd4hep.sh;
   		    CMD="cd ${WORK_DIR}/EX; cmake ${OPTS} -DDD4hep_DIR=${DD4hep_DIR} ${CHECKOUT}/examples;";
                    make_build;
                    #
		    cd ../..;
		done;
	    done;
	done;
    done;
}

build_all;
