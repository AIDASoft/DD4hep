##==========================================================================
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
# DDCMS is a detector description convention developed by the CMS experiment.
#
#=================================================================================

dd4hep_package(    DDCMS
  USES             [DD4hep REQUIRED COMPONENTS DDCore]
                   [CLHEP REQUIRED]
		   [ROOT REQUIRED COMPONENTS Geom GenVector]
  INCLUDE_DIRS     include
  INSTALL_INCLUDES include/DDCMS)

#
# We only create only library for DDCMS. The whole package is a single component
# library. A priory there is no need to seperate the implementation from the
# plugins....
#
# If need arises, this can be changed easily.
#
#---DDCMS plugin library -------------------------------------------------------
dd4hep_add_plugin ( DDCMS SOURCES src/*.cpp src/plugins/*.cpp
  USES  [DD4hep REQUIRED COMPONENTS DDCore DDAlign DDCond]
        [ROOT REQUIRED COMPONENTS Geom GenVector])
