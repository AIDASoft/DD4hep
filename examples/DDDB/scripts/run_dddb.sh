#!/bin/bash
#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
#
#  @author M.Frank
#
#==========================================================================
echo "+ ------------------------------------------------------------------+";
#echo "|                                                                   |";
echo "|   Starting DDDB plugin executor....                               |";
#echo "|                                                                   |";
echo "+ ------------------------------------------------------------------+";
#
# Check for arguments:
if test -z "${DDDB_DIR}"; then
    target=/tmp;
    if test -n "$USER"; then
        target=/tmp/$USER;
    fi;
    DDDB_DIR=${target}/DDDB;
fi;
loader="-loader DDDB_FileReader";
params="-params file:${DDDB_DIR}/Parameters.xml";
input="-input file:${DDDB_DIR}/DDDB/lhcb.xml";
config="";
exec="";
vis="";
debug="";
last_cmd="";
plugins="";
#
all_args="$*";
#
while [[ "$1" == -* ]]; do
    #echo "Arg:$1 $2 [$*]";
    a1=`echo $1 | tr A-Z a-z`;
    #echo "Arg: $1 -- ${last_cmd}";
    case ${a1} in
        -plugin)
            plugins="$*";
            while test -n "$1"; do 
                shift;
            done;
            ;;
        -end-plugin)
            plugins="$*";
            while test -n "$1"; do 
                shift;
            done;
            ;;
        -debug)
            debug="gdb --args";
            last_cmd="";
            ;;
	-attr)
            vis="${vis} -attr $2";
            last_cmd="";
            shift;
            ;;
	-visualize)
	    vis="${vis} -visualize";
            last_cmd="";
            ;;
	-noparams)
            params="";
            last_cmd="";
            ;;
	-params)
            params="-params $2";
            last_cmd="";
            shift;
            ;;
	-loader)
            loader="-loader $2";
            last_cmd="";
            shift;
            ;;                    
	-noinput)
            input="";
            last_cmd="";
            ;;
	-input)
            input="-input $2";
            last_cmd="";
            shift;
            ;;
	-config)
            config="${config} -config $2";
            last_cmd="config";
            shift;
            ;;
	-exec)
            exec="${exec} -exec $2";
            last_cmd="exec";
            shift;
            ;;
        -arg)
            if [ "${last_cmd}" = "exec" ]; then
                exec="${exec} $2";
            elif [ "${last_cmd}" = "config" ]; then
                config="${config} $2";
            fi;
            shift;
            ;;
	*)
            echo "Usage: $0 -arg [-arg]";
            exit 13;    # EACCES
	    ;;
    esac
    shift;
done;
#
#
if [ "$(uname)" == "Darwin" ]; then
  export DYLD_LIBRARY_PATH=${DD4HEP_LIBRARY_PATH}
fi
export DD4HEP_TRACE=ON;
echo "Command(1): ${debug} `which geoPluginRun` -destroy -plugin DDDB_Executor ${loader} ${params} ${input} ${config} ${exec} ${vis} ${plugins}"
echo "Command(2): ${debug} `which geoPluginRun` -destroy -plugin DDDB_Executor ${all_args} ${plugins}"

ARGS=`echo -plugin DDDB_Executor ${loader} ${params} ${input} ${config} ${exec} ${vis} ${plugins}`;
echo "Command: ${debug} `which geoPluginRun` -destroy $ARGS";
if test -z "${debug}";then
    exec `which geoPluginRun` -destroy ${ARGS};
else
    ${debug} `which geoPluginRun` -destroy ${ARGS};
fi;
