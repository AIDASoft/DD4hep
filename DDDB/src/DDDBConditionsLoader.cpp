//==========================================================================
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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework include files
#include "DDDB/DDDBConditionsLoader.h"
#include "DDDB/DDDBReaderContext.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Operators.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsManagerObject.h"

// C/C++ include files
#include <set>

// Forward declartions
using namespace std;
using namespace DD4hep::DDDB;
using DD4hep::Geometry::LCDD;
using DD4hep::Conditions::Condition;
using DD4hep::Conditions::AbstractMap;
using DD4hep::Conditions::RangeConditions;
using DD4hep::Conditions::ConditionsSlice;
typedef DD4hep::Conditions::RangeConditions RC;

namespace {
  enum CMD {
    INSERT=1,
    REPLACE=2,
    NONE
  };
  typedef DD4hep::Conditions::ConditionsDataLoader::RequiredItems Entries;
  typedef DD4hep::Conditions::ConditionsDataLoader::LoadedItems   Loaded;
  struct CallArgs  {
    CMD                        cmd;
    Condition::key_type        key;
    Condition::iov_type&       iov;
    RC*                        rc;
    Entries*                   work;
    Loaded*                    loaded;
    CallArgs(CMD c, Condition::key_type k, Condition::iov_type& i, RC& r)
      : cmd(c), key(k), iov(i), rc(&r), work(0), loaded(0) {}
    CallArgs(CMD c, Condition::key_type k, Condition::iov_type& i, Entries& w, Loaded& l)
      : cmd(c), key(k), iov(i), rc(0), work(&w), loaded(&l) {}
  };

  void* create_loader(LCDD& lcdd, int argc, char** argv)   {
    using DD4hep::Conditions::ConditionsManager;
    using DD4hep::Conditions::ConditionsManagerObject;
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


/// Initializing constructor
DDDBConditionsLoader::KeyCollector::KeyCollector() : call(this,0)   {
}

/// ConditionsListener overload: onRegister new condition
void DDDBConditionsLoader::KeyCollector::onRegisterCondition(Conditions::Condition cond, void*)  {
  Condition::Object* c = cond.ptr();
  // Register address key
  if ( !c->address.empty() )   {
    KeyMap::const_iterator k = keys.find(c->hash);
    if ( k != keys.end() )   {
      size_t idx = c->address.find('#');
      string url = (idx == string::npos) ? c->address : c->address.substr(0,idx);
      keys.insert(make_pair(c->hash,make_pair(url,c->name)));
    }
  }
}

/// Standard constructor, initializes variables
DDDBConditionsLoader::DDDBConditionsLoader(LCDD& lcdd, ConditionsManager mgr, const string& nam) 
  : Conditions::ConditionsDataLoader(lcdd, mgr, nam)
{
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(); // Ensures object existence!
  m_mgr->callOnRegister(m_keys.call,true);
  m_resolver = helper->xmlReader();
}

/// Default Destructor
DDDBConditionsLoader::~DDDBConditionsLoader() {
  m_mgr->callOnRegister(m_keys.call,false);
} 

/// Load single conditions document
void DDDBConditionsLoader::loadDocument(XML::UriContextReader& rdr, const Key& k)
{
  loadDocument(rdr, k.first, k.second);
}

/// Load single conditions document
void DDDBConditionsLoader::loadDocument(XML::UriContextReader& rdr, 
                                        const string& sys_id,
                                        const string& obj_id)
{
  const void* argv_conddb[] = {&rdr, sys_id.c_str(), obj_id.c_str(), 0};
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

/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t DDDBConditionsLoader::load_range(key_type key,
                                        const iov_type& req_validity,
                                        RangeConditions& conditions)   {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t len = conditions.size();
    DDDBReaderContext   local;
    const Key&          url_key = (*k).second;
    Condition::iov_type iov(req_validity.iovType);
    long                start = req_validity.keyData.first;
    CallArgs            arg(INSERT, key, iov, conditions);
    XML::UriContextReader local_reader(m_resolver, &local);
    pair<ConditionsListener*,void*> call(this,&arg);

    m_mgr->callOnRegister(call,true);  
    while (start < req_validity.keyData.second )   {
      local.event_time  = start;
      local.valid_since = 0;
      local.valid_until = 0;
      iov.reset().invert();
      loadDocument(local_reader, url_key);
      start = iov.keyData.second+1;
    }
    m_mgr->callOnRegister(call,false);
    return conditions.size() - len;
  }
  return 0;
}

size_t DDDBConditionsLoader::load_single(key_type key,
                                         const iov_type& req_validity,
                                         RangeConditions& conditions)  {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t len = conditions.size();
    DDDBReaderContext   local;
    Condition::iov_type iov(req_validity.iovType);
    CallArgs arg(INSERT, key, iov, conditions);
    pair<ConditionsListener*,void*> call(this,&arg);
    XML::UriContextReader local_reader(m_resolver, &local);

    iov.reset().invert();
    local.event_time  = req_validity.keyData.first;
    local.valid_since = 0;
    local.valid_until = 0;
    m_mgr->callOnRegister(call,true);  
    loadDocument(local_reader, (*k).second);
    m_mgr->callOnRegister(call,false);
    return conditions.size() - len;
  }
  return 0;
}

#if 0
/// Update a range of conditions according to the required IOV
size_t DDDBConditionsLoader::update(const iov_type& req_validity,
                                    RangeConditions& conditions,
                                    iov_type& conditions_validity)
{
  CallArgs arg(REPLACE, 0, conditions_validity, conditions);
  pair<ConditionsListener*,void*> call(this,&arg);
  map<string,Condition::Object*> urls;
  DDDBReaderContext  local;

  local.event_time  = req_validity.keyData.first;
  local.valid_since = 0;
  local.valid_until = 0;
  m_mgr->callOnRegister(call,true);

  XML::UriContextReader local_reader(m_resolver, &local);
  /// First collect all required URIs
  for(RC::const_iterator i=conditions.begin(); i!=conditions.end(); ++i)  {
    Condition::Object* c = (*i).ptr();
    size_t idx = c->address.find('#');
    string url = (idx == string::npos) ? c->address : c->address.substr(0,idx);
#if 0
    printout(INFO,"DDDB","++ Need to update: %-40s [%16llX] --> %s",
             c->name.c_str(), c->hash, url.c_str());
#endif
    urls.insert(make_pair(url,c));
  }
  /// Now load them. In the callbacks we can check if we got all required conditions
  for(map<string,Condition::Object*>::const_iterator j=urls.begin(); j!=urls.end(); ++j)  {
    Condition            cond = (*j).second;
    const AbstractMap&   data = cond.get<AbstractMap>();
    const Document*       doc = data.option<Document>();
    loadDocument(local_reader, doc->id, doc->name);
  }
  m_mgr->callOnRegister(call,false);
  return 0;
}
#endif

/// Optimized update using conditions slice data
size_t DDDBConditionsLoader::load_many(const iov_type& req_validity,
                                       RequiredItems&  work,
                                       LoadedItems&    loaded,
                                       iov_type&       conditions_validity)
{
  CallArgs arg(REPLACE, 0, conditions_validity, work, loaded);
  pair<ConditionsListener*,void*> call(this,&arg);
  DDDBReaderContext  local;
  map<std::string,std::string>   urls;

  local.event_time  = req_validity.keyData.first;
  local.valid_since = 0;
  local.valid_until = 0;
  m_mgr->callOnRegister(call,true);

  XML::UriContextReader local_reader(m_resolver, &local);

  /// First collect all required URIs
  for(const auto& i : work )  {
    ConditionsSlice::Entry* e = i.second;
    if ( e->dependency )  {
      printout(INFO,"DDDB","++ CANNOT update derived: %-40s [%16llX]",
               e->dependency->target.name.c_str(), e->dependency->target.hash);
      continue;
    }
    std::string* address = e->loadinfo->data<std::string>();
    if ( !address )  {
      printout(INFO,"DDDB","++ CANNOT update condition: %-40s [%16llX]",
               e->key.name.c_str(), e->key.hash);
      continue;
    }
    size_t idx     = address->find('@');
    string doc_nam = address->substr(0,idx);
    string doc_url = address->substr(idx+1);
    if ( (idx=doc_url.find('#')) != string::npos )
      doc_url = doc_url.substr(0,idx);
    //printout(DEBUG,"DDDB","++ Need to update: %-40s [%16llX] --> %s",
    //         e->key.name.c_str(), e->key.hash, doc_url.c_str());
    urls.insert(make_pair(doc_url,doc_nam));
  }
  /// Now load them. In the callbacks we can check if we got all required conditions
  for(const auto& url : urls )  {
    loadDocument(local_reader, url.first, url.second);
    // loadDocument(local_reader, doc->id, doc->name);
  }
  m_mgr->callOnRegister(call,false);
  //except("ConditionsLoader","+++ update: Invalid call!");
  return 0;
}


/// ConditionsListener overload: onRegister new condition
void DDDBConditionsLoader::onRegisterCondition(Condition cond, void* param)  {
  // See if we got what we asked for:
  if ( param )   {
    CallArgs* arg = (CallArgs*)param;
    Condition::Object* c = cond.ptr();
    if ( arg->rc )  {
      RC& r = *arg->rc;
      if ( arg->cmd == REPLACE )  {
        RC::iterator i=std::find_if(r.begin(),r.end(),byName(cond));
        if ( i != r.end() ) {
          (*i) = cond;
          printout(DEBUG,"DDDB","++ Got  MATCH: %-40s [%16llX] --> %s.",
                   c->name.c_str(), c->hash, c->address.c_str());
          arg->iov.iov_intersection(cond.iov());
          return;
        }
        printout(INFO,"DDDB","++ Got update: %-40s [%16llX] --> %s.",
                 c->name.c_str(), c->hash, c->address.c_str());
      }
      else if ( arg->cmd == INSERT && arg->key == c->hash )   {
        arg->iov.iov_intersection(cond.iov());
        r.insert(r.end(),cond);
      }
    }
    else  {
      key_type     key    = cond.key();
      LoadedItems& loaded = *arg->loaded;
      loaded.insert(make_pair(key,cond));
    }
  }
}
