#!/bin/bash
# $Id$
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
#
#
while [[ "$1" == -* ]]; do
    #echo "Arg:$1 $2 [$*]";
    a1=`echo $1 | tr A-Z a-z`;
    case ${a1} in
        -debug)
            debug="gdb --args";
            ;;
	-attr)
            vis="${vis} -attr $2";
            shift;
            ;;
	-visualize)
	    vis="${vis} -visualize";
            ;;
	-params)
            params="-params $2";
            shift;
            ;;
	-loader)
            loader="-loader $2";
            shift;
            ;;                    
	-input)
            input="-input $2";
            shift;
            ;;
	-config)
            config="${config} -config $2";
            shift;
            ;;
	-exec)
            exec="${exec} -exec $2";
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
export DD4HEP_TRACE=ON;
ARGS=`echo -plugin DDDB_Executor ${loader} ${params} ${input} ${config} ${exec} ${vis}`;
echo "Command: ${debug} `which geoPluginRun` -destroy $ARGS";
if test -z "${debug}";then
    exec `which geoPluginRun` -destroy ${ARGS};
else
    ${debug} `which geoPluginRun` -destroy ${ARGS};
fi;
