#==========================================================================
#  $Id: $
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
dd4hep_package( DDDetectors USES DDRec )
#---Add Plugin Library-----------------------------------------------------
dd4hep_add_plugin ( DDDetectors SOURCES src/*.cpp )
#---Package installation procedure(s) -------------------------------------
dd4hep_install_dir ( compact DESTINATION DDDetectors )
