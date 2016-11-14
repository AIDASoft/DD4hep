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
#include "DD4hep/DetConditions.h"
#include "DD4hep/ConditionsPrinter.h"

using std::string;
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

/// Container callback for object processing
int ConditionsPrinter::operator()(Container container, UserPool* pool)   {
  if ( pool )  {
    for(const auto& k : container.keys() )  {
      Condition c = container.get(k.first,*pool);
      string cn = c.name().substr(c.name().find('#')+1);
      Condition::key_type key = ConditionKey::hashCode(cn);
      printout(INFO,name,"++ %s %s %s [%08X] -> %s [%08X]",
               prefix.c_str(), "Condition:", cn.c_str(), key==k.first ? key : k.first,
               c.name().c_str(), k.second.first);
      (*this)(c);
    }
    return 1;
  }
  except(name,"Cannot dump conditions container without user-pool.");
  return 0;
}

/// Callback to output conditions information of an entire DetElement
int ConditionsPrinter::operator()(DetElement de, UserPool* pool, bool recurse)    {
  if ( de.isValid() )  {
    if ( pool )  {
      DetConditions conds(de);
      (*this)(conds.conditions(), pool);
      if ( recurse )  {
        for (const auto& c : de.children() )
          (*this)(c.second, pool, recurse);
      }
      return 1;
    }
    except(name,"Cannot dump conditions from %s without pool",de.name());
  }
  except(name,"Cannot dump conditions of an invalid detector element");
  return 0;
}
