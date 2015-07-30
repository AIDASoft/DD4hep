dir_name=`pwd`;
INSTALL_G4=/home/frankm/SW/g4_10.01.p02_dbg/lib/Geant4-10.1.2;
INSTALL_LCIO=/home/frankm/SW/lcio/v02-04-03;
INSTALL_XERCESC=/home/frankm/SW/xercesc;
export ROOTSYS=/home/frankm/SW/root_v5.34.25_dbg;
. ${ROOTSYS}/bin/thisroot.sh;
#
#
make_opt()
{
    if test "$1" = "ON"; then
	echo $*;
    else
	echo ${1};
    fi;
}
LINE="==================================================================================================="
for DOGEANT4 in OFF ON; do
    for DOXERCESC in OFF ON; do
	for DOGEAR in OFF; do
	    for DOLCIO in OFF ON; do
		folder=build_Xer${DOXERCESC}_Geant${DOGEANT4}_Gear${DOGEAR}_Lcio${DOLCIO}
		mkdir ${dir_name}/$folder
		cd ${dir_name}/$folder
		echo "${LINE}";
		echo "${LINE}";
		echo "=============================== ${folder} ";
		echo "${LINE}";
		echo "${LINE}";
                #-DDD4HEP_DEBUG_CMAKE=ON \
		CMD="cd ${dir_name}/$folder ; cmake -D DD4HEP_USE_XERCESC=${DOXERCESC} \
		    -DDD4HEP_USE_GEANT4=`make_opt ${DOGEANT4} -DGeant4_DIR=${INSTALL_G4}`\
		    -DDD4HEP_USE_LCIO=`make_opt ${DOLCIO} -DLCIO_DIR=${INSTALL_LCIO}` \
		    -DBoost_NO_BOOST_CMAKE=ON \
		    -DDD4HEP_USE_XERCESC=`make_opt ${DOXERCESC} -DXERCESC_ROOT_DIR=${INSTALL_XERCESC}` \
		    ../../../DD4hep.trunk/checkout;";
                echo ${CMD};
                eval ${CMD};
		if [ $? -ne  0 ]; then
		    echo "DANGER WILL ROBINSON DANGER!"
		    echo "${LINE}";
		    echo "=============================== ${folder} ";
		    echo "${LINE}";
                    echo "++++ Failed CMAKE command:"
                    echo ${CMD};
		    exit 1
		fi
		make -j5
		if [ $? -ne  0 ]; then
		    echo "DANGER WILL ROBINSON DANGER!"
		    echo "${LINE}";
		    echo "=============================== ${folder} ";
		    echo "${LINE}";
                    echo "++++ Failed BUILD:"
                    echo ${CMD};
		    exit 1
		fi
		cd ..
	    done
	done
    done
done
