#==========================================================================
#
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
#--------------------------------------------------------------------------
#  DDDetectors
#
#  Author:  M.Frank
#  Date:    13/11/2014
#
#--------------------------------------------------------------------------

new_dd4hep_add_plugin(DDDetectors 
  SOURCES src/*.cpp
  LINK_LIBRARIES DDRec
  INCLUDE_DIRS include
)

INSTALL(TARGETS DDDetectors DESTINATION lib)
INSTALL(DIRECTORY compact DESTINATION DDDetectors)
