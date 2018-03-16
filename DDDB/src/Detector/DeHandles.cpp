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
#include "Detector/DeVP.h"
#include "DD4hep/detail/Handle.inl"

// This is some magic you do not really want to know about....

using namespace gaudi::detail;

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeStaticObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPSensorStaticObject,DeStaticObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPGenericStaticObject,DeStaticObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPStaticObject,DeVPGenericStaticObject,DeStaticObject,ConditionObject);

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeIOVObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPSensorObject,DeIOVObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPGenericObject,DeIOVObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DeVPObject,DeVPGenericObject,DeIOVObject,ConditionObject);


#include "Parsers/Parsers.h"
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPStaticObject)
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPSensorStaticObject)
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPGenericStaticObject)
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPObject)
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPSensorObject)
DD4HEP_DEFINE_OSTREAM_DUMMY(DeVPGenericObject)

DD4HEP_DEFINE_PARSER_DUMMY(DeVPStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPSensorStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPGenericStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPSensorObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPGenericObject)

#include "DD4hep/detail/BasicGrammar_inl.h"
#include "DD4hep/detail/ConditionsInterna.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPStaticObject,eval_none<DeVPStaticObject>)
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPSensorStaticObject,eval_none<DeVPSensorStaticObject>)
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPGenericStaticObject,eval_none<DeVPGenericStaticObject>)
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPObject,eval_none<DeVPObject>)
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPSensorObject,eval_none<DeVPSensorObject>)
DD4HEP_DEFINE_PARSER_GRAMMAR(DeVPGenericObject,eval_none<DeVPGenericObject>)

DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPStaticObject)
DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPSensorStaticObject)
DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPGenericStaticObject)
DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPObject)
DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPSensorObject)
DD4HEP_DEFINE_CONDITIONS_TYPE(DeVPGenericObject)
