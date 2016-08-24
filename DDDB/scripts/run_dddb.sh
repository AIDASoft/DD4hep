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
echo "|                                                                   |";
echo "|   Starting DDDB plugin executor....                               |";
echo "|                                                                   |";
echo "+ ------------------------------------------------------------------+";
#
ARGS=`echo -plugin DDDB_Executor \
    -loader DDDB_FileReader \
    -config DD4hep_ConditionsManagerInstaller \
    -params file:../DDDB/Parameters.xml \
    -input file:../DDDB/DDDB/lhcb.xml \
    -exec DDDB_DetectorConditionDump \
    -visualize -attr file:../DDDB/Visattrs.xml`;
echo "Command: `which geoPluginRun` $ARGS";
`which geoPluginRun` ${ARGS};
