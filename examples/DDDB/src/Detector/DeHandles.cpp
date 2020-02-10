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

// These folks are not really conditions. Condition::get<T>() is never called for these types.
#if 0

inline std::ostream& operator << (std::ostream& s, const DeVPStaticObject&)        { return s; }
inline std::ostream& operator << (std::ostream& s, const DeVPSensorStaticObject&)  { return s; }
inline std::ostream& operator << (std::ostream& s, const DeVPGenericStaticObject&) { return s; }
inline std::ostream& operator << (std::ostream& s, const DeVPObject&)              { return s; }
inline std::ostream& operator << (std::ostream& s, const DeVPSensorObject&)        { return s; }
inline std::ostream& operator << (std::ostream& s, const DeVPGenericObject&)       { return s; }

#include "Parsers/Parsers.h"
DD4HEP_DEFINE_PARSER_DUMMY(DeVPStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPSensorStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPGenericStaticObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPSensorObject)
DD4HEP_DEFINE_PARSER_DUMMY(DeVPGenericObject)

#endif
