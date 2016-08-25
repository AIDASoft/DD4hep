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
dd4hep_package(    DDDB
  USES             DDCore DDCond
                   [BOOST REQUIRED COMPONENTS filesystem]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDDB)

#---DDDB plugin library -------------------------------------------------------
dd4hep_add_plugin ( DDDB SOURCES src/*.cpp )
