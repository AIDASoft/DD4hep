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
#include "DD4hep/Factories.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionPrinter.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;

using Conditions::AbstractMap;
using Conditions::Condition;

/// Initializing constructor
ConditionPrinter::ParamPrinter::ParamPrinter(ConditionPrinter* p)  : m_parent(p)  {
}

/// Callback to output conditions information
void ConditionPrinter::ParamPrinter::operator()(const AbstractMap::Params::value_type& obj)  const {
  const std::type_info& type = obj.second.typeInfo();
  ++m_parent->m_numParam;
  if ( type == typeid(string) )  {
    string value = obj.second.get<string>().c_str();
    size_t len = value.length();
    if ( len > 100 ) {
      value.erase(100);
      value += "...";
    }
    printout(m_parent->m_printLevel,"Condition","++ %s\t-> Param: %-16s %-8s -> %s",
             m_parent->m_prefix.c_str(),
             obj.first.c_str(), 
             obj.second.dataType().c_str(), 
             value.c_str());
  }
  else if ( type == typeid(AbstractMap) )  {
    const AbstractMap& d= obj.second.get<AbstractMap>();
    printout(m_parent->m_printLevel,"Condition","++ %s\t-> [%s] CL:%d %-8s -> %s",
             m_parent->m_prefix.c_str(),
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
    printout(m_parent->m_printLevel,"Condition","++ %s\t-> [%s] %-8s -> %s",
             m_parent->m_prefix.c_str(),
             obj.first.c_str(),
             obj.second.dataType().c_str(), 
             value.c_str());	
  }
}

/// Initializing constructor
ConditionPrinter::ConditionPrinter(const string& prefix, int flg, ParamPrinter* prt)
  : ConditionsProcessor(0), m_print(prt), m_prefix(prefix), m_flag(flg)
{
}

/// Default destructor
ConditionPrinter::~ConditionPrinter()   {
  printout(INFO,"Condition","++ %s +++++++++++++ Printout summary:", m_prefix.c_str());
  printout(INFO,"Condition","++ %s Number of conditions:       %8ld  [  dto. empty:%ld]",
           m_prefix.c_str(), m_numCondition, m_numEmptyCondition);
  printout(INFO,"Condition","++ %s Total Number of parameters: %8ld  [%7.3f Parameters/Condition]",
           m_prefix.c_str(), m_numParam, double(m_numParam)/std::max(double(m_numCondition),1e0));
}

/// Set printout level for prinouts
void ConditionPrinter::setPrintLevel(PrintLevel lvl)   {
  m_printLevel = lvl;
}

/// Callback to output conditions information
int ConditionPrinter::operator()(Condition cond)    {
  if ( cond.isValid() )   {
    printout(m_printLevel,"Condition","++ %s%s",m_prefix.c_str(),cond.str(m_flag).c_str());
    const AbstractMap& data = cond.get<AbstractMap>();
    printout(m_printLevel,"Condition","++ %s Path:%s Class:%d [%s]",
             m_prefix.c_str(),
             cond.name(),
             data.classID, 
             cond.data().dataType().c_str());
    ++m_numCondition;
    if ( !data.params.empty() )  {
      const string tmp = m_prefix;
      m_prefix.assign(tmp.length(),' ');
      if ( !m_print ) m_print = new ParamPrinter(this);
      for_each(data.params.begin(), data.params.end(),*m_print);
      m_prefix = tmp;
    }
    else  {
      ++m_numEmptyCondition;
    }
  }
  return 1;
}

/// Plugin function
static void* create_dddb_conditions_printer(Geometry::LCDD& /* lcdd */, int argc, char** argv)  {
  int        flags = 0;
  string     prefix = "";
  PrintLevel prtLevel = INFO;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-prefix",argv[i],6) )
      prefix = argv[++i];
    else if ( 0 == ::strncmp("-flags",argv[i],6) )
      flags = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-printlevel",argv[i],6) )
      prtLevel = DD4hep::printLevel(argv[++i]);
  }
  DDDB::ConditionPrinter* printer = flags
    ? new DDDB::ConditionPrinter(prefix,flags)
    : new DDDB::ConditionPrinter(prefix);
  printer->setPrintLevel(prtLevel);
  DetElement::Processor* proc = printer;
  return proc;
}
DECLARE_LCDD_CONSTRUCTOR(DDDB_ConditionsPrinter,create_dddb_conditions_printer)
