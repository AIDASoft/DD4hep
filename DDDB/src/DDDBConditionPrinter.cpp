// $Id$
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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/Printout.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

using Conditions::AbstractMap;
using Conditions::Condition;

/// Initializing constructor
ConditionPrinter::ParamPrinter::ParamPrinter(string& p)  : m_prefix(p)  {
}

/// Callback to output conditions information
void ConditionPrinter::ParamPrinter::operator()(const AbstractMap::Params::value_type& obj)  const {
  const std::type_info& type = obj.second.typeInfo();
  if ( type == typeid(string) )  {
    string value = obj.second.get<string>().c_str();
    size_t len = value.length();
    if ( len > 100 ) {
      value.erase(100);
      value += "...";
    }
    printout(INFO,"Condition","++ %s\t-> Param: %-16s %-8s -> %s",
             m_prefix.c_str(),
             obj.first.c_str(), 
             obj.second.dataType().c_str(), 
             value.c_str());	
  }
  else if ( type == typeid(AbstractMap) )  {
    const AbstractMap& d= obj.second.get<AbstractMap>();
    printout(INFO,"Condition","++ %s\t-> [%s] CL:%d %-8s -> %s",
             m_prefix.c_str(),
             obj.first.c_str(), d.classID,
             obj.second.dataType().c_str(), 
             obj.second.str().c_str());	
  }
  else {
    string value = obj.second.str();
    size_t len = value.length();
    if ( len > 100 ) {
      value.erase(100);
      value += "...";
    }
    printout(INFO,"Condition","++ %s\t-> [%s] %-8s -> %s",
             m_prefix.c_str(),
             obj.first.c_str(),
             obj.second.dataType().c_str(), 
             value.c_str());	
  }
}

/// Initializing constructor
ConditionPrinter::ConditionPrinter(const string& prefix, int flg, ParamPrinter* prt)
  : m_prefix(prefix), m_print(prt), m_flag(flg)
{
}

/// Callback to output conditions information
int ConditionPrinter::operator()(Condition cond)    {
  if ( cond.isValid() )   {
    printout(INFO,"Condition","++ %s%s",m_prefix.c_str(),cond.str(m_flag).c_str());
    const AbstractMap& data = cond.get<AbstractMap>();
    string new_prefix = m_prefix;
    new_prefix.assign(m_prefix.length(),' ');
    printout(INFO,"Condition","++ %s Path:%s Class:%d [%s]",
             m_prefix.c_str(),
             cond.name().c_str(),
             data.classID, 
             cond.data().dataType().c_str());
    if ( !data.params.empty() )  {
      if ( m_print )  {
        const string& tmp = m_print->prefix();
        m_print->setPrefix(new_prefix);
        for_each(data.params.begin(), data.params.end(),*m_print);
        m_print->setPrefix(tmp);
        return 1;
      }
      for_each(data.params.begin(), data.params.end(),ParamPrinter(new_prefix));
    }
  }
  return 1;
}
