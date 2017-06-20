//==========================================================================
//  AIDA Detector description implementation 
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
#include "DDParsers/ToStream.h"
#include "DD4hep/Printout.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionsPrinter.h"
#include "DD4hep/ConditionsProcessor.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;

namespace {
  /// C++ version: replace all occurrences of a string
  string str_replace(const std::string& str, const std::string& pattern, const std::string& replacement) {
    string res = str;
    for(size_t id=res.find(pattern); id != std::string::npos; id = res.find(pattern) )
      res.replace(id,pattern.length(),replacement);
    return res;
  }
}


/// DDDB Conditions data dumper helper to output parameter maps.
/**
 *   \author  M.Frank
 *   \version 1.0
 *   \date    31/03/2016
 *   \ingroup DD4HEP_DDDB
 */
class ConditionsPrinter::ParamPrinter {
protected:
  /// Parent object
  ConditionsPrinter* m_parent = 0;
public:
  std::string        prefix;
  PrintLevel         printLevel;
public:
  /// Copy constructor
  ParamPrinter(const ParamPrinter& copy) = default;
  /// Initializing constructor
  ParamPrinter(ConditionsPrinter* p);
  /// Default destructor
  virtual ~ParamPrinter() = default;
  /// Assignment operator
  ParamPrinter& operator=(const ParamPrinter& copy) = default;
  /// Callback to output conditions information
  virtual void operator()(const cond::AbstractMap::Params::value_type& obj)  const;
};


/// Initializing constructor
ConditionsPrinter::ParamPrinter::ParamPrinter(ConditionsPrinter* p)  : m_parent(p)  {
}

/// Callback to output conditions information
void ConditionsPrinter::ParamPrinter::operator()(const AbstractMap::Params::value_type& obj)  const {
  const type_info& type = obj.second.typeInfo();
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
ConditionsPrinter::ConditionsPrinter(ConditionsMap* m, const string& pref, int flg)
  : mapping(m), m_flag(flg), name("Condition"), prefix(pref)
{
  m_print = new ParamPrinter(this);
  m_print->printLevel = printLevel;
}

/// Default destructor
ConditionsPrinter::~ConditionsPrinter()   {
  printout(INFO,name,"++ %s +++++++++++++ Printout summary:", prefix.c_str());
  printout(INFO,name,"++ %s Number of conditions:       %8ld  [  dto. empty:%ld]",
           prefix.c_str(), numCondition, numEmptyCondition);
  printout(INFO,name,"++ %s Total Number of parameters: %8ld  [%7.3f Parameters/Condition]",
           prefix.c_str(), numParam, double(numParam)/std::max(double(numCondition),1e0));
  detail::deletePtr(m_print);
}

/// Actual print method
int ConditionsPrinter::operator()(Condition cond)   const   {
  m_print->printLevel = printLevel;
  if ( cond.isValid() )   {
    string repr = cond.str(m_flag);
    Condition::Object* ptr    = cond.ptr();
    const type_info&   type   = cond.typeInfo();
    const OpaqueData&  opaque = cond.data();

    if ( repr.length() > lineLength )
      repr = repr.substr(0,lineLength)+"...";
    printout(this->printLevel,name, "++ %s%s", prefix.c_str(), repr.c_str());

    string new_prefix = prefix;
    new_prefix.assign(prefix.length(),' ');
    printout(this->printLevel,name,"++ %s \tPath:%s Key:%16llX Type:%s",
             new_prefix.c_str(), cond.name(), cond.key(), opaque.dataType().c_str());
    //string values = opaque.str();
    //if ( values.length() > lineLength ) values = values.substr(0,130)+"...";
    //printout(this->printLevel,name,"++ %s \tData:%s", new_prefix.c_str(), values.c_str());
    if ( type == typeid(AbstractMap) )  {
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
    else if ( type == typeid(Delta) )  {
      string piv;
      stringstream str_tr, str_rot, str_piv;
      const Delta& D = cond.get<Delta>();
      if ( D.hasTranslation() ) Utils::toStream(D.translation, str_tr);
      if ( D.hasRotation()    ) Utils::toStream(D.rotation, str_rot);
      if ( D.hasPivot()       ) {
        Utils::toStream(D.pivot, str_piv);
        piv = str_replace(str_piv.str(),"\n","");
        piv =  "( "+str_replace(piv,"  "," , ")+" )";
      }
      printout(printLevel,name,"++ %s \t[%p] Typ:%s",
               prefix.c_str(), cond.ptr(),
               typeName(typeid(*ptr)).c_str());
      printout(printLevel,name,"++ %s \tData(%11s-%8s-%5s): [%s, %s, %s]",
               prefix.c_str(), 
               D.hasTranslation() ? "Translation" : "",
               D.hasRotation() ? "Rotation" : "",
               D.hasPivot() ? "Pivot" : "",
               str_replace(str_tr.str(),"\n","").c_str(),
               str_replace(str_rot.str(),"\n","").c_str(),
               piv.c_str()
               );
    }
    else if ( type == typeid(AlignmentData) )  {
      string piv;
      stringstream str_tr, str_rot, str_piv;
      const Delta& D = cond.get<AlignmentData>().delta;
      if ( D.hasTranslation() ) Utils::toStream(D.translation, str_tr);
      if ( D.hasRotation()    ) Utils::toStream(D.rotation, str_rot);
      if ( D.hasPivot()       ) {
        Utils::toStream(D.pivot, str_piv);
        piv = str_replace(str_piv.str(),"\n","");
        piv =  "( "+str_replace(piv,"  "," , ")+" )";
      }
      
      printout(printLevel,name,"++ %s \t[%p] Typ:%s",
               prefix.c_str(), cond.ptr(),
               typeName(typeid(*ptr)).c_str());
      printout(printLevel,name,"++ %s \tData(%11s-%8s-%5s): [%s, %s, %s]",
               prefix.c_str(), 
               D.hasTranslation() ? "Translation" : "",
               D.hasRotation() ? "Rotation" : "",
               D.hasPivot() ? "Pivot" : "",
               str_replace(str_tr.str(),"\n","").c_str(),
               str_replace(str_rot.str(),"\n","").c_str(),
               piv.c_str()
               );
    }
    else if ( type == typeid(string) )  {
      string value = cond.get<string>().c_str();
      size_t len = value.length();
      if ( len > lineLength ) {
        value = value.substr(0,lineLength);
        value += "...";
        value = str_replace(value,"\n","");
      }
      printout(printLevel,name,"++ %s \tString [%s]: %s",
               prefix.c_str(),
               cond.data().dataType().c_str(), 
               value.c_str());
    }
    else {
      string value = cond.str();
      size_t len = value.length();
      if ( len > lineLength ) {
        value = value.substr(0,lineLength);
        value += "...";
        value = str_replace(value,"\n","");
      }
      printout(printLevel,name,"++ %s \t[%s]: %s",
               prefix.c_str(),
               cond.data().dataType().c_str(), 
               value.c_str());	
    }
    return 1;
  }
  return 0;
}

/// Processing callback to print conditions
int ConditionsPrinter::operator()(DetElement de, int level)   const {
  if ( mapping )   {
    vector<Condition> conditions;
    conditionsCollector(*mapping,conditions)(de,level);
    printout(this->printLevel, name, "++ %s %-3ld Conditions for DE %s",
             prefix.c_str(), conditions.size(), de.path().c_str()); 
    for( auto cond : conditions )
      (*this)(cond);
    return int(conditions.size());
  }
  except(name,"Failed to dump conditions for DetElement:%s [No slice availible]",
         de.path().c_str());
  return 0;
}
