#!/bin/bash
#=================================================================================
#
#  AIDA Detector description implementation 
#---------------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#=================================================================================

usage()  {
    echo "run_test_in_loop.sh -opt [-opt]";
    echo "   -n         <name>        Supply test name";
    echo "   --name     <name>        Supply test name";
    echo "   -l         <number>      Number of test loops";
    echo "   --loops    <number>      Number of test loops";
    echo "   -j         <number>      Parallelism flag";
    echo "   --parallel <number>      Parallelism flag";
    echo "   -g         <dir>         Non default Geant4 data directory";
    echo "   --g4-data  <dir>         Non default Geant4 data directory";
    echo "   -h                       Show this help.";
    echo "   --help                   Show this help.";
    exit EINVAL;
}

PARSED_ARGUMENTS=$(getopt -a -n run_test_in_loop -o hg:Nl:R:j: --longoptions help,g4-data:,show-only,loops:,test-regex:,parallel: -- "[run_test_in_loop]" $*)
VALID_ARGUMENTS=$?
if [ "$VALID_ARGUMENTS" != "0" ]; then
    do_print "Invalid arguments to cmake step";
    usage;
fi;

noop=;
test_name=;
num_loop=1
parallel=10;
g4_data_dir=;
eval set -- "$PARSED_ARGUMENTS";
while :
do
    case "${1}" in
	-g | --g4-data)       g4_data_dir=${2};           shift 2 ;;
	-l | --loops)         num_loop=${2};              shift 2 ;;
	-R | --tests-regex)   test_name="-R ${2}";        shift 2 ;;
	-j | --parallel)      parallel=${2};              shift 2 ;;
	-N | --show-only)     noop="-N";                  shift ;;
	-h | --help)    usage;                            shift ;;
	# -- means the end of the arguments; drop this, and break out of the while loop
	--) shift;  break;;
	# If invalid options were passed, then getopt should have reported an error,
	# which we checked as VALID_ARGUMENTS when getopt was called...
	*) do_print " Unexpected option: $1 - this should not happen."
            usage; shift; break;;
    esac;
done;
#
if test -z "${num_loop}"; then
    usage;
fi;
#
if test -z "${g4_data_dir}"; then
    g4_data_dir=/scratch/online/frankm/SW/Geant4Data;
fi;
#
#
#
if test -n "${g4_data_dir}"; then
    export GEANT4_DATA_DIR=${g4_data_dir};
    export G4ABLADATA=${GEANT4_DATA_DIR}/G4ABLA3.3;
    export G4CHANNELINGDATA=${GEANT4_DATA_DIR}/G4CHANNELING2.0;
    export G4LEDATA=${GEANT4_DATA_DIR}/G4EMLOW8.8;
    export G4ENSDFSTATEDATA=${GEANT4_DATA_DIR}/G4ENSDFSTATE3.0;
    export G4INCLDATA=${GEANT4_DATA_DIR}/G4INCL1.3;
    export G4NEUTRONHPDATA=${GEANT4_DATA_DIR}/G4NDL4.7.1;
    export G4PARTICLEXSDATA=${GEANT4_DATA_DIR}/G4PARTICLEXS4.2;
    export G4PIIDATA=${GEANT4_DATA_DIR}/G4PII1.3;
    export G4SAIDXSDATA=${GEANT4_DATA_DIR}/G4SAIDDATA2.0;
    export G4LEVELGAMMADATA=${GEANT4_DATA_DIR}/PhotonEvaporation6.1.2;
    export G4RADIOACTIVEDATA=${GEANT4_DATA_DIR}/RadioactiveDecay6.1.2;
    export G4REALSURFACEDATA=${GEANT4_DATA_DIR}/RealSurface2.2;
fi;
#
exec_command()  {
    $* | gawk '{ print strftime("[%Y-%m-%d %H:%M:%S] | "), $0 }';
}
#
line="==============================";
for i in $(seq 1 ${num_loop}); do
    exec_command echo "${line}${line}${line}${line}${line}";
    exec_command echo "Executing test "${test_name}" in loop number ${i}";
    exec_command ctest -j ${parallel} ${noop} ${test_name};
    if test "$?" = "0"; then
	exec_command echo "=====> Test ${i} SUCCEEDED: exit code: $? ";
    else
	exec_command echo "=====> Test ${i}  FAILED: exit code: $?";
	exec_command echo "${line}${line}${line}${line}${line}";
	exit $?;
    fi;
done;
exec_command echo "${line}${line}${line}${line}${line}";
