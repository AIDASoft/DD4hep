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
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"
#include "DD4hep/PluginTester.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

using Conditions::Condition;
using Conditions::AbstractMap;
using Conditions::ConditionsMap;
using Conditions::ConditionsSlice;

/// Initializing constructor
ConditionPrinter::ParamPrinter::ParamPrinter(ConditionPrinter* p)  : m_parent(p)  {
}

/// Callback to output conditions information
void ConditionPrinter::ParamPrinter::operator()(const AbstractMap::Params::value_type& obj)  const {
  const std::type_info& type = obj.second.typeInfo();
  ++m_parent->numParam;
  if ( type == typeid(string) )  {
    string value = obj.second.get<string>().c_str();
    size_t len = value.length();
    if ( len > m_parent->lineLength ) {
      value.erase(m_parent->lineLength);
      value += "...";
    }
    printout(m_parent->printLevel,m_parent->name,"++ %s\t-> Param: %-16s %-8s -> %s",
             prefix.c_str(),
             obj.first.c_str(), 
             obj.second.dataType().c_str(), 
             value.c_str());
  }
  else if ( type == typeid(AbstractMap) )  {
    const AbstractMap& d= obj.second.get<AbstractMap>();
    printout(m_parent->printLevel,m_parent->name,"++ %s\t-> [%s] CL:%d %-8s -> %s",
             prefix.c_str(),
             obj.first.c_str(), d.classID,
             obj.second.dataType().c_str(), 
             obj.second.str().c_str());	
  }
  else {
    string value = obj.second.str();
    size_t len = value.length();
    if ( len > m_parent->lineLength ) {
      value.erase(m_parent->lineLength);
      value += "...";
    }
    printout(m_parent->printLevel,m_parent->name,"++ %s\t-> [%s] %-8s -> %s",
             prefix.c_str(),
             obj.first.c_str(),
             obj.second.dataType().c_str(), 
             value.c_str());	
  }
}

/// Initializing constructor
ConditionPrinter::ConditionPrinter(ConditionsMap* m, const string& p, int f, ParamPrinter* prt)
  : ConditionsPrinter(m,p,f), m_print(prt)
{
  m_print = new ParamPrinter(this);
  m_print->printLevel = printLevel;
}

/// Default destructor
ConditionPrinter::~ConditionPrinter()   {
  printout(INFO,name,"++ %s +++++++++++++ Printout summary:", prefix.c_str());
  printout(INFO,name,"++ %s Number of conditions:       %8ld  [  dto. empty:%ld]",
           prefix.c_str(), numCondition, numEmptyCondition);
  printout(INFO,name,"++ %s Total Number of parameters: %8ld  [%7.3f Parameters/Condition]",
           prefix.c_str(), numParam, double(numParam)/std::max(double(numCondition),1e0));
  deletePtr(m_print);
}

/// Callback to output conditions information
int ConditionPrinter::operator()(Condition cond)  const  {
  if ( cond.isValid() )   {
    printout(printLevel,name,"++ %s%s",prefix.c_str(),cond.str(m_flag).c_str());
    const AbstractMap& data = cond.get<AbstractMap>();
    printout(printLevel,name,"++ %s Path:%s Class:%d [%s]",
             prefix.c_str(),
             cond.name(),
             data.classID, 
             cond.data().dataType().c_str());
    ++numCondition;
    if ( !data.params.empty() )  {
      m_print->printLevel = printLevel;
      m_print->prefix.assign(prefix.length(),' ');
      for_each(data.params.begin(), data.params.end(),*m_print);
    }
    else  {
      ++numEmptyCondition;
    }
  }
  return 1;
}
