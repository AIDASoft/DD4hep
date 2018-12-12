//==========================================================================
//  AIDA Detector description implementation 
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

// Other dd4hep includes
#include "DD4hep/Printout.h"
#include "DD4hep/Operators.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/detail/ConditionsInterna.h"

// Forward declartions
using namespace std;
using namespace dd4hep;
using namespace dd4hep::cond;
using cond::ConditionsSlice;
using cond::ConditionsListener;
using cond::ConditionsDescriptor;
using cond::ConditionsManagerObject;
using cond::ConditionsLoadInfo;
using DDDB::DDDBConditionsLoader;

namespace {
  enum CMD {
    INSERT=1,
    REPLACE=2,
    NONE
  };
  typedef ConditionsDataLoader::RequiredItems Entries;
  typedef ConditionsDataLoader::LoadedItems   Loaded;

  /// Loader callback for Item loaders: load_single(...), load_range(...)
  /** 
   *  \author   M.Frank
   *  \version  1.0
   *  \ingroup  DD4HEP_CONDITIONS
   */
  struct ItemCollector : public ConditionsListener  {
  private:
    CMD                        cmd;
    Condition::key_type        key;
    RangeConditions&           rc;
    const IOV&                 req_iov;

  public:
    IOV                        iov;

  public:
    /// Initializing constructor
    ItemCollector(CMD c, Condition::key_type k, const IOV& i, RangeConditions& r)
      : cmd(c), key(k), rc(r), req_iov(i), iov(i.iovType)  {}
    /// ConditionsListener overload: onRegister new condition
    virtual void onRegisterCondition(Condition cond, void* param )  {
      Condition::Object* c = cond.ptr();
      if ( param == this && key == c->hash &&
           req_iov.iovType == c->iov->iovType &&
           IOV::key_is_contained(c->iov->keyData,req_iov.keyData) )
      {
        if ( cmd == REPLACE )  {
          auto i=std::find_if(rc.begin(),rc.end(),byName(cond));
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
    Loaded&    loaded;
    const IOV& req_iov;
  public:
    /// Resulting IOV
    IOV iov;
  public:
    /// Initializing constructor
    GroupCollector(const IOV& i, Loaded& l)
      : loaded(l), req_iov(i), iov(req_iov.iovType)
    {
      iov.reset().invert();
    }
    /// ConditionsListener overload: onRegister new condition
    virtual void onRegisterCondition(Condition cond, void* param)  {
      Condition::Object* c = cond.ptr();
      if ( c && param == this &&
           req_iov.iovType == c->iov->iovType &&
           IOV::key_is_contained(req_iov.keyData,c->iov->keyData) )
      {
        loaded.insert(make_pair(c->hash,cond));
        /// This is not entirely correct:
        /// There may be more conditions loaded than required
        /// and hence the interval of the pool bigger.
        /// But we do not want to make another map-lookup!
        iov.iov_intersection(c->iov->keyData);
      }
    }
  };
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace DDDB  {
    /// Plugin entry points.
    long load_dddb_conditions_from_uri(Detector& description, int argc, char** argv);
    long dddb_conditions_2_dd4hep(Detector& description, int argc, char** argv);

    long load_dddb_from_uri(Detector& description, int argc, char** argv);
    long dddb_2_dd4hep(Detector& description, int argc, char** argv);
  } /* End namespace DDDB                    */
} /* End namespace dd4hep                    */


/// Initializing constructor
DDDBConditionsLoader::KeyCollector::KeyCollector() : call(this,0)   {
}

/// ConditionsListener overload: onRegister new condition
void DDDBConditionsLoader::KeyCollector::onRegisterCondition(Condition cond, void*)  {
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
DDDBConditionsLoader::DDDBConditionsLoader(Detector& description, ConditionsManager mgr, const string& nam) 
  : ConditionsDataLoader(description, mgr, nam)
{
  DDDBHelper* helper = description.extension<DDDBHelper>(); // Ensures object existence!
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
void DDDBConditionsLoader::loadDocument(xml::UriContextReader& rdr, const Key& k)
{
  loadDocument(rdr, k.first, k.second);
}

/// Load single conditions document
void DDDBConditionsLoader::loadDocument(xml::UriContextReader& rdr, 
                                        const string& sys_id,
                                        const string& obj_id)
{
  const void* argv_conddb[] = {&rdr, sys_id.c_str(), obj_id.c_str(), 0};
  long result = load_dddb_conditions_from_uri(m_detector, 3, (char**)argv_conddb);
  if ( 0 == result )  {
    except("DDDB","++ Failed to load conditions from URI:%s",sys_id.c_str());
  }
  const void* argv_dddb[] = {"conditions_only", 0};
  result = dddb_conditions_2_dd4hep(m_detector, 1, (char**)argv_dddb);
  if ( 0 == result )  {
    except("DDDBLoader","++ Failed to process conditions from URI:%s",sys_id.c_str());
  }
}

/// Load  a condition set given a Detector Element and the conditions name according to their validity
size_t DDDBConditionsLoader::load_range(key_type key,
                                        const IOV& req_iov,
                                        RangeConditions& conditions)   {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t                      len = conditions.size();
    DDDBReaderContext           local;
    const Key&                  url_key = (*k).second;
    long                        start = req_iov.keyData.first;
    xml::UriContextReader       local_reader(m_resolver, &local);
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
                                         const IOV& req_iov,
                                         RangeConditions& conditions)  {
  KeyMap::const_iterator k = m_keys.keys.find(key);
  if ( k != m_keys.keys.end() )   {
    size_t                      len = conditions.size();
    DDDBReaderContext           local;
    ItemCollector               listener(INSERT, key, req_iov, conditions);
    xml::UriContextReader       local_reader(m_resolver, &local);

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
size_t DDDBConditionsLoader::load_many(const IOV&      req_iov,
                                       RequiredItems&  work,
                                       LoadedItems&    loaded,
                                       IOV&            conditions_validity)
{
  DDDBReaderContext  local;
  size_t len = loaded.size();
  map<std::string,std::string>   urls;

  local.event_time  = req_iov.keyData.first;
  local.valid_since = 0;
  local.valid_until = 0;

  xml::UriContextReader local_reader(m_resolver, &local);

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
    size_t loaded_len    = loaded.size();
    bool   print_results = isActivePrintLevel(DEBUG);
    m_mgr->callOnRegister(make_pair(&listener,&listener),true);
    listener.iov.reset().invert();
    for( const auto& url : urls )  {
      loadDocument(local_reader, url.first, url.second);
      if ( !print_results ) continue;
      printout(DEBUG,"DDDBLoader","++ Loaded %3ld conditions from %s.",loaded.size()-loaded_len,url.first.c_str());
      loaded_len = loaded.size();
    }
    if ( print_results )  {
      for( const auto& e : loaded )  {
        const Condition& cond = e.second;
        printout(INFO,"DDDBLoader","++ %16llX: %s -> %s",cond.key(),cond->value.c_str(),cond.name());
      }
    }
    conditions_validity = listener.iov;
  }
  catch( const exception& e )  {
    printout(ERROR,"DDDBLoader","+++ Load exception: %s",e.what());
    throw;
  }
  catch( ... )  {
    printout(ERROR,"DDDBLoader","+++ UNKNWON Load exception.");
    throw;    
  }
  m_mgr->callOnRegister(make_pair(&listener,&listener),false);
  return loaded.size()-len;
}
