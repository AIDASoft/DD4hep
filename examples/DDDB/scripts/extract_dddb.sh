#!/bin/bash
# $Id$
#==========================================================================
#  AIDA Detector description implementation 
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
echo "|   Starting DDDB extraction from tar file                          |";
echo "+ ------------------------------------------------------------------+";
#
if test -n "$USER"; then
    target=/tmp/$USER;
else
    target=/tmp;
fi;
source=${DD4hepINSTALL}/examples/DDDB/DDDB.tar.gz;
clean="NO";
#
# Check for arguments:
while [[ "$1" == -* ]]; do
    #echo "Arg:$1 $2 [$*]";
    a1=`echo $1 | tr A-Z a-z`;
    case ${a1} in
	-to)
	    target=$2;
	    shift
	    ;;
	-target)
	    target=$2;
	    shift
	    ;;
        -dddb)
	    source=$2;
	    shift
	    ;;
        -clean)
            clean="YES";
            ;;
        -input)
	    source=$2;
	    shift
	    ;;
	*)
            echo "Usage: $0 -arg [-arg]";
            echo "  -target <directory>  Installation target directory. Default: $target";
            echo "  -input  <tar-file>   Input data file. Default: $source";
            exit 13;    # EACCES
	    ;;
    esac
    shift;
done;
#
# Seems like cmake install is not really deterministic and installs files either with or without
# data source directory name....if input is not present try to add /data/.
#
if test ! -f ${source}; then
    base=`basename ${source}`;
    dir=`dirname ${source}`;
    source=${dir}/data/${base};
fi;
#
# Now do the installation
#
if test -d ${target}/DDDB; then
    if test "${clean}" = "YES";then
        rm -rf ${target}/DDDB;
        echo "DDDB Database successfully removed ${target}";
        exit 0;
    fi;
    echo "DDDB database is already extracted to ${target}. Nothing to do.";
elif test ! -f ${source}; then
    echo "DDDB database tar file ${source} is not present. [Installation FAILED]";
    #echo "Directory: /builds/CLICdp/DD4hep/examples/DDDB";
    #ls -laF /builds/CLICdp/DD4hep/examples/DDDB;
    #echo "Directory /builds/CLICdp/DD4hep/examples/DDDB/data";
    #ls -laF /builds/CLICdp/DD4hep/examples/DDDB/data;
    exit 2;  # ENOENT
else
    mkdir -p ${target}/DDDB;
    if test ! -d ${target}/DDDB; then
        echo "DDDB database target directory ${target} cannot be created [Installation FAILED]";
        exit 2;  # ENOENT
    fi;
    cd ${target};
    echo "${target} : tar -xf ${source}";
    `which tar` -xf ${source};
    #/bin/ls -laF `dirname $source`;
    #/bin/ls -laF $target;
fi;
if test -f ${target}/DDDB/DDDB/lhcb.xml; then
    echo "DDDB Database successfully installed.";
else
    echo "DDDB Database installation FAILED";
    exit 2;  # ENOENT
fi;
exit 0;
