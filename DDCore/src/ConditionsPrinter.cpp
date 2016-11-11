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

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Initializing constructor
ConditionsPrinter::ConditionsPrinter(const std::string& pref, int flg)
  : name("Condition"), prefix(pref), m_flag(flg)
{
}

/// Callback to output conditions information
int ConditionsPrinter::operator()(Condition cond)    {
  if ( cond.isValid() )   {
    printout(INFO,name,"++ %s%s",prefix.c_str(),cond.str(m_flag).c_str());
    const OpaqueData& data = cond.data();
    std::string values = data.str();
    if ( values.length() > 132 ) values = values.substr(0,130)+"...";
    std::string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(INFO,name,"++ %s \tPath:%s",
             new_prefix.c_str(),
             cond.name().c_str(),
             data.dataType().c_str());
    printout(INFO,name,"++ %s \tData:%s",
             new_prefix.c_str(),
             values.c_str());
  }
  return 1;
}
