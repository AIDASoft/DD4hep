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
    echo "remove_tabs.sh -opt [-opt]";
    echo "   -h                       Show this help.";
    echo "   --help                   Show this help.";
    exit EINVAL;
}


PARSED_ARGUMENTS=$(getopt -a -n remove_tabs -o hd: --longoptions help,directory: -- "[remove_tabs]" $*)
VALID_ARGUMENTS=$?
if [ "$VALID_ARGUMENTS" != "0" ]; then
    do_print "Invalid arguments to cmake step";
    usage;
fi;

eval set -- "$PARSED_ARGUMENTS";
while :
do
    case "${1}" in
        -d | --directory)   dirs="${2} ${dirs}"; echo ${2};         shift 2 ;;
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
#
exec_command()  {
    $* | gawk '{ print strftime("[%Y-%m-%d %H:%M:%S] | "), $0 }';
}
#
current=`pwd`;
line="==============================";
for dir in ${dirs}; do
    exec_command echo "${line}${line}${line}${line}${line}";
    cd ${dir};
    path=`pwd`;
    exec_command echo "Re moving tabs in directory: ${path}";
    headers=`grep "      " -r ${path} | tr ":" " "| awk '{ print $1}' | uniq -d | grep -e "${path}/.*\.h"`
    files=`grep "      " -r ${path} | tr ":" " "| awk '{ print $1}' | uniq -d | grep -e "${path}/.*\.cpp"`
    for file in ${headers} ${files}; do
        exec_command echo " --> Removing tabs from ${file}";
        sed -i 's/\t/        /g'  ${file};
    done;
    cd ${current};
done;
