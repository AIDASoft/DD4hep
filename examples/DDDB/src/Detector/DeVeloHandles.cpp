//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================

// A bit of magic

// Framework include files
#include "Detector/DeVelo.h"
#include "DD4hep/detail/Handle.inl"

// This is some magic you do not really want to know about....
using namespace gaudi::detail;

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloSensorStaticObject,DeStaticObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloGenericStaticObject,DeStaticObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloStaticObject,DeVeloGenericStaticObject,DeStaticObject,ConditionObject);

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloSensorObject,DeIOVObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloGenericObject,DeIOVObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVeloObject,DeVeloGenericObject,DeIOVObject,ConditionObject);
