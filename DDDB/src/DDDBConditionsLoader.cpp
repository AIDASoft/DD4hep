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
#include "DDDB/DDDBHelper.h"

// Other DD4hep includes
#include "DD4hep/Printout.h"
#include "DD4hep/Operators.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DDCond/ConditionsManagerObject.h"

// Forward declartions
using namespace std;
using namespace DD4hep;
using Geometry::LCDD;
using Conditions::Condition;
using Conditions::RangeConditions;
using Conditions::ConditionsSlice;
using Conditions::ConditionsListener;
using Conditions::ConditionsDescriptor;
using Conditions::ConditionsManagerObject;
using Conditions::ConditionsLoadInfo;
using DDDB::DDDBConditionsLoader;

namespace {
  enum CMD {
    INSERT=1,
    REPLACE=2,
    NONE
  };
  typedef Conditions::ConditionsDataLoader::RequiredItems Entries;
  typedef Conditions::ConditionsDataLoader::LoadedItems   Loaded;

  /// Loader callback for Item loaders: load_single(...), load_range(...)
  /** 
   *  \author   M.Frank
   *  \version  1.0
   *  \ingroup  DD4HEP_CONDITIONS
   */
  struct ItemCollector : public ConditionsListener  {
  private:
    typedef Conditions::RangeConditions RC;
    CMD                        cmd;
    Condition::key_type        key;
    RC&                        rc;
    const Condition::iov_type& req_iov;

  public:
    Condition::iov_type        iov;

  public:
    /// Initializing constructor
    ItemCollector(CMD c, Condition::key_type k, const Condition::iov_type& i, RC& r)
      : cmd(c), key(k), rc(r), req_iov(i), iov(i.iovType)  {}
    /// ConditionsListener overload: onRegister new condition
    virtual void onRegisterCondition(Condition cond, void* param )  {
      Condition::Object* c = cond.ptr();
      if ( param == this && key == c->hash &&
           req_iov.iovType == c->iov->iovType &&
           IOV::key_is_contained(c->iov->keyData,req_iov.keyData) )
      {
        if ( cmd == REPLACE )  {
          RC::iterator i=std::find_if(rc.begin(),rc.end(),byName(cond));
          if ( i != rc.end() ) {
            (*i) = cond;
            printout(DEBUG,"DDDB","++ Got  MATCH: %-40s [%16llX] --> %s.",
                     c->name.c_str(), c->hash, c->address.c_str());
            iov.iov_intersection(*c->iov);
            return;
          }
          printout(INFO,"DDDB","++ Got update: %-40s [%16llX] --> %s.",
                   c->name.c_str(), c->hash, c->address.c_str());
        }
        else if (  cmd == INSERT )  {
          iov.iov_intersection(*c->iov);
          rc.insert(rc.end(),cond);
        }
      }
    }
  };
  
  /// Loader callback for Group loader: load_many(...)
  /** 
   *  \author   M.Frank
   *  \version  1.0
   *  \ingroup  DD4HEP_CONDITIONS
   */
  struct GroupCollector : public ConditionsListener  {
  private:
    Loaded&                    loaded;
    const Condition::iov_type& req_iov;

  public:
    Condition::iov_type        iov;

  public:
    /// Initializing constructor
    GroupCollector(const Condition::iov_type& i, Loaded& l)
      : loaded(l), req_iov(i), iov(req_iov.iovType)  {}
    /// ConditionsListener overload: onRegister new condition
    virtual void onRegisterCondition(Condition cond, void* param)  {
      Condition::Object* c = cond.ptr();
      if ( c && param == this &&
           req_iov.iovType == c->iov->iovType &&
           IOV::key_is_contained(c->iov->keyData,req_iov.keyData) )
      {
        loaded.insert(make_pair(c->hash,cond));
      }
    }
  };
}

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace DDDB  {
    /// Plugin entry points.
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
  if ( (0==(c->flags&Condition::DERIVED)) && !c->address.empty() )   {
    KeyMap::const_iterator k = keys.find(c->hash);
    if ( k == keys.end() )   {
      const string& addr = c->address;
      size_t idx     = addr.find('@');
      string doc_nam = addr.substr(0,idx);
      string doc_url = addr.substr(idx+1);
      if ( (idx=doc_url.find('#')) != string::npos )
        doc_url = doc_url.substr(0,idx);
      keys.insert(make_pair(c->hash,make_pair(doc_url,doc_nam)));
    }
  }
}

/// Standard constructor, initializes variables
DDDBConditionsLoader::DDDBConditionsLoader(LCDD& lcdd, ConditionsManager mgr, const string& nam) 
  : Conditions::ConditionsDataLoader(lcdd, mgr, nam)
{
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(); // Ensures object existence!
  // It is UGLY to listen on conditions appearing to know from which file they origin
  // but we do not have a better way as of now....
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
    except("DDDBLoader","++ Failed to process conditions from URI:%s",sys_id.c_str());
  }
}

/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t DDDBConditionsLoader::load_range(key_type key,
                                        const iov_type& req_iov,
                                        RangeConditions& conditions)   {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t                      len = conditions.size();
    DDDBReaderContext           local;
    const Key&                  url_key = (*k).second;
    long                        start = req_iov.keyData.first;
    XML::UriContextReader       local_reader(m_resolver, &local);
    ItemCollector               listener(INSERT, key, req_iov, conditions);

    m_mgr->callOnRegister(make_pair(&listener,&listener), true);  
    while (start < req_iov.keyData.second )   {
      local.event_time  = start;
      local.valid_since = 0;
      local.valid_until = 0;
      listener.iov.reset().invert();
      loadDocument(local_reader, url_key);
      start = listener.iov.keyData.second+1;
    }
    m_mgr->callOnRegister(make_pair(&listener,&listener), false);
    return conditions.size() - len;
  }
  return 0;
}

/// Access single conditions from the persistent medium
size_t DDDBConditionsLoader::load_single(key_type key,
                                         const iov_type& req_iov,
                                         RangeConditions& conditions)  {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t                      len = conditions.size();
    DDDBReaderContext           local;
    ItemCollector               listener(INSERT, key, req_iov, conditions);
    XML::UriContextReader       local_reader(m_resolver, &local);

    local.valid_since = 0;
    local.valid_until = 0;
    local.event_time  = req_iov.keyData.first;
    m_mgr->callOnRegister(make_pair(&listener,&listener), true);  
    loadDocument(local_reader, (*k).second);
    m_mgr->callOnRegister(make_pair(&listener,&listener), false);
    return conditions.size() - len;
  }
  return 0;
}

/// Optimized update using conditions slice data
size_t DDDBConditionsLoader::load_many(const iov_type& req_iov,
                                       RequiredItems&  work,
                                       LoadedItems&    loaded,
                                       iov_type&       conditions_validity)
{
  DDDBReaderContext  local;
  size_t len = loaded.size();
  map<std::string,std::string>   urls;

  local.event_time  = req_iov.keyData.first;
  local.valid_since = 0;
  local.valid_until = 0;

  XML::UriContextReader local_reader(m_resolver, &local);

  // First collect all required URIs which need loading.
  // Since one file contains many conditions, we have
  // to create a unique set
  for(const auto& i : work )  {
    ConditionsLoadInfo* e = i.second;
    std::string* addr = e->data<std::string>();
    if ( !addr )  {
      printout(INFO,"DDDBLoader","++ CANNOT update condition: [%16llX] [No load info]]",i.first);
      continue;
    }
    size_t idx     = addr->find('@');
    string doc_nam = addr->substr(0,idx);
    string doc_url = addr->substr(idx+1);
    if ( (idx=doc_url.find('#')) != string::npos )
      doc_url = doc_url.substr(0,idx);
    urls.insert(make_pair(doc_url,doc_nam));
  }

  // Now load all necessary URIs.
  // In the callbacks we can check if we got all required conditions
  GroupCollector listener(req_iov, loaded);
  try  {
    m_mgr->callOnRegister(make_pair(&listener,&listener),true);
    listener.iov.reset().invert();
    for(const auto& url : urls )  {
      loadDocument(local_reader, url.first, url.second);
    }
    conditions_validity = listener.iov;
  }
  catch(const exception& e)  {
    printout(ERROR,"DDDBLoader","+++ Load exception: %s",e.what());
    throw;
  }
  catch(...)  {
    printout(ERROR,"DDDBLoader","+++ UNKNWON Load exception.");
    throw;    
  }
  m_mgr->callOnRegister(make_pair(&listener,&listener),false);
  return loaded.size()-len;
}
