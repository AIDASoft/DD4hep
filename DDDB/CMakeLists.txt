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
# DDDB is a detector description convention developed by the LHCb experiment.
# For further information concerning the DTD, please see:
# http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
# The usage of Xerces-C is mandatory due to the entity resolution mechanism
#
#=================================================================================

dd4hep_package(    DDDB
  USES             DDCore DDAlign DDCond
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDDB)

#
# We only create only library for DDDB. The whole package is a single component
# library. A priory there is no need to seperate the implementation from the
# plugins....
#
# If need arises, this can be changed easily.
#
#---DDDB plugin library -------------------------------------------------------
dd4hep_add_plugin ( DDDB SOURCES src/*.cpp src/plugins/*.cpp
  USES  DDCore DDAlign DDCond )
