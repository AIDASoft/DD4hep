#!/bin/bash
#
# Simple script to run DD4hep tests
# - sources this${PackageName}.sh first and then
#   calls the command (given as first argument)
#   with all following arguments
#
if [ ! ${DD4hep_DIR} ]; then
    export DD4hep_DIR=@DD4hep_DIR@;
fi;
#
source ${DD4hep_DIR}/bin/thisdd4hep.sh;
dd4hep_parse_this ${BASH_ARGV[0]} @PackageName@;
#
#----- initialize environment for this package - including DD4hep 
source ${THIS}/bin/this@PackageName@.sh
#
#----- parse command line - first argument is the 
#      test to run
command=$1
theargs=""
shift
for i in "$@" ; do
    if [ $i != ${command} ] ; then 
	theargs="${theargs} $i"
    fi
done

#echo " #### LD_LIBRARY_PATH = :  ${LD_LIBRARY_PATH}" 

echo " ### running test :  '${command} ${theargs}'"
exec ${command} ${theargs}
