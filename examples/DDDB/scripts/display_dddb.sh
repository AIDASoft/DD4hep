#!/bin/bash
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
if test -z "${DDDB_DIR}"; then
    target=/tmp;
    if test -n "$USER"; then
        target=/tmp/$USER;
    fi;
    DDDB_DIR=${target}/DDDB;
fi;
export DDDB_DIR;
. ${DD4hepINSTALL}/bin/run_test_DDDB.sh \
  ${DD4hepINSTALL}/bin/run_dddb.sh \
  -config DD4hep_ConditionsManagerInstaller \
  -visualize \
  -attr file:${DDDB_DIR}/Visattrs.xml $*
