#!/bin/bash

#
# Simple script to run DD4hep tests
# - sources thisdd4hep.sh first and then
#   calls the command (given as first argument)
#   with all following arguments
#

# ---- initialize DD4hep for this shell
. @CMAKE_INSTALL_PREFIX@/bin/thisdd4hep.sh


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

echo "---running test :  '" ${command} ${theargs} "'"
${command} ${theargs}
