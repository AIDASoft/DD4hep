//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/ConditionsPrinter.h"
//#include "DD4hep/objects/ConditionsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Initializing constructor
ConditionsPrinter::ConditionsPrinter(const std::string& pref, int flg)
  : Condition::Processor(), name("Condition"), prefix(pref), printLevel(INFO), m_flag(flg)
{
}

/// Callback to output conditions information
int ConditionsPrinter::operator()(Condition cond)    {
  if ( cond.isValid() )   {
    std::string repr = cond.str(m_flag);
    if ( repr.length() > 100 )
      printout(this->printLevel,name,
               "++ %s%s", prefix.c_str(), repr.c_str());
    else
      printout(this->printLevel,name,"++ %s%s [%p]",
               prefix.c_str(), repr.c_str(), cond.ptr());
    const OpaqueData& data = cond.data();
    std::string values = data.str();
    if ( values.length() > 132 ) values = values.substr(0,130)+"...";
    std::string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(this->printLevel,name,"++ %s \tPath:%s Key:%16llX Type:%s",
             new_prefix.c_str(), cond.name(), cond.key(), data.dataType().c_str());
    printout(this->printLevel,name,"++ %s \tData:%s", new_prefix.c_str(), values.c_str());
  }
  return 1;
}

