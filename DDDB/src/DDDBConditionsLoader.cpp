//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
// $Id$

// Framework include files
#include "DDDB/DDDBConditionsLoader.h"
#include "DDDB/DDDBReaderContext.h"
#include "DDDB/DDDBHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Operators.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsInterna.h"

// C/C++ include files
#include <set>

// Forward declartions
using namespace std;
using namespace DD4hep::DDDB;
using DD4hep::Geometry::LCDD;
using DD4hep::Geometry::DetElement;
using DD4hep::Conditions::IOV;
using DD4hep::Conditions::Condition;
using DD4hep::Conditions::ConditionsManager;
using DD4hep::Conditions::Interna::ConditionsManagerObject;
using DD4hep::Conditions::ConditionsDataLoader;
using DD4hep::Conditions::ConditionsListener;
using DD4hep::Conditions::RangeConditions;

namespace {
  void* create_loader(LCDD& lcdd, int argc, char** argv)   {
    const char* name = argc>0 ? argv[0] : "DDDBLoader";
    ConditionsManagerObject* m = (ConditionsManagerObject*)(argc>0 ? argv[1] : 0);
    return new DDDBConditionsLoader(lcdd,ConditionsManager(m),name);
  }
}
DECLARE_LCDD_CONSTRUCTOR(DD4hep_Conditions_dddb_Loader,create_loader)

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace DDDB  {
    /// Plugin entry point.
    long load_dddb_conditions_from_uri(LCDD& lcdd, int argc, char** argv);
    long dddb_conditions_2_dd4hep(LCDD& lcdd, int argc, char** argv);

    long load_dddb_from_uri(LCDD& lcdd, int argc, char** argv);
    long dddb_2_dd4hep(LCDD& lcdd, int argc, char** argv);
  } /* End namespace DDDB                    */
} /* End namespace DD4hep                    */

/// Standard constructor, initializes variables
DDDBConditionsLoader::DDDBConditionsLoader(LCDD& lcdd, ConditionsManager mgr, const string& nam) 
: Conditions::ConditionsDataLoader(lcdd, mgr, nam)
{
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(); // Ensures object existence!
  m_resolver = helper->xmlReader();
}

/// Default Destructor
DDDBConditionsLoader::~DDDBConditionsLoader() {
} 

/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t DDDBConditionsLoader::load_range(DetElement det,
                                         const string& cond,
                                         const IOV& req_validity,
                                         RangeConditions& conditions)   {
  return 0;
}

size_t DDDBConditionsLoader::load(DetElement det, 
                                   const string& cond, 
                                   const IOV& req_validity, 
                                   RangeConditions& conditions)  {
  return 0;
}

/// ConditionsListener overload: onRegister new condition
void DDDBConditionsLoader::onRegisterCondition(Condition cond, void* param)  {
  pair<RangeConditions*,IOV*>* arg = (pair<RangeConditions*,IOV*>*)param;
  RangeConditions* range = arg->first;
  Condition::Object* c = cond.ptr();
  RangeConditions::iterator i=std::find_if(range->begin(),range->end(),byName(cond));
  if ( i != range->end() ) {
    (*i) = cond;
    printout(DEBUG,"DDDB","++ Got  MATCH: %-40s [%08X] --> %s.",
	     (c->value+"/"+c->name).c_str(), c->hash, c->address.c_str());
    arg->second->iov_intersection(cond.iov());
    return;
  }
  printout(INFO,"DDDB","++ Got update: %-40s [%08X] --> %s.",
	   (c->value+"/"+c->name).c_str(), c->hash, c->address.c_str());
}

/// Update a range of conditions according to the required IOV
size_t DDDBConditionsLoader::update(const IOV& req_iov, 
				    RangeConditions& conditions,
				    IOV& conditions_validity)
{
  pair<RangeConditions*,IOV*> arg(&conditions,&conditions_validity);
  pair<ConditionsListener*,void*> call(this,&arg);
  map<string,Condition::Object*> urls;
  DDDBReaderContext  local;

  local.event_time  = req_iov.keyData.first;
  local.valid_since = 0;
  local.valid_until = 0;
  m_mgr->callOnRegister(call,true);

  XML::UriContextReader local_reader(m_resolver, &local);
  /// First collect all required URIs
  for(RangeConditions::const_iterator i=conditions.begin(); i!=conditions.end(); ++i)  {
    Condition::Object* c = (*i).ptr();
    size_t idx = c->address.find('#');
    string url = (idx == string::npos) ? c->address : c->address.substr(0,idx);
    printout(INFO,"DDDB","++ Need to update: %-40s [%08X] --> %s",
	     (c->value+"/"+c->name).c_str(), c->hash, url.c_str());
    urls.insert(make_pair(url,c));
  }
  /// Now load them. In the callbacks we can check if we got all required conditions
  for(map<string,Condition::Object*>::const_iterator j=urls.begin(); j!=urls.end(); ++j)  {
    const string& sys_id   = (*j).first;
    const string& obj_path = (*j).second->value;
    const void* argv_conddb[] = {&local_reader, sys_id.c_str(), obj_path.c_str(), 0};
    long result = load_dddb_conditions_from_uri(m_lcdd, 3, (char**)argv_conddb);
    if ( 0 == result )  {
      except("DDDB","++ Failed to load conditions from URI:%s",sys_id.c_str());
    }
    const void* argv_dddb[] = {"conditions_only", 0};
    result = dddb_conditions_2_dd4hep(m_lcdd, 1, (char**)argv_dddb);
    if ( 0 == result )  {
      except("DDDB","++ Failed to process conditions from URI:%s",sys_id.c_str());
    }
  }
  m_mgr->callOnRegister(call,false);
  
  return 0;
}
