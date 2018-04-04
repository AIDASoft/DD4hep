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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DetService.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace gaudi;

using dd4hep::except;
using dd4hep::printout;
using dd4hep::PrintLevel;

typedef lock_guard<mutex> Lock;

/// Initializing constructor
DetService::DetService(ConditionsManager m) : m_manager(m)
{
  declareProperty("AgeLimit", m_ageLimit=3);
}

/// Initialize the service
int DetService::initialize()   {
  return 1;
}

/// Finalize the service
int DetService::finalize()   {
  return 1;
}

/// Accessor: get refernece to the conditions manager
DetService::ConditionsManager DetService::manager() const   {
  return m_manager;
}

/// Accessor: Access IOV Type from conditions manager
const DetService::IOVType* DetService::iovType(const std::string& identifier)  const  {
  return m_manager.iovType(identifier);
}

/// Open content creation context and copy items from registered object
DetService::Content DetService::getContent(const string& nam)   {
  Lock lock(m_contentLock);
  for(const auto& c : m_activeContents)   {
    if ( c.first == nam )
      return c.second;
  }
  for(const auto& c : m_openContents)   {
    if ( c.first == nam )
      return c.second;
  }
  return Content();
}

/// Open content creation context and copy items from registered object
DetService::Content DetService::openContent(const string& nam)   {
  Content ptr = getContent(nam);
  if ( !ptr )   {
    Lock lock(m_contentLock);
    ptr.reset(new dd4hep::cond::ConditionsContent());
    m_openContents.insert(m_openContents.end(),make_pair(nam,ptr));
    return ptr;
  }
  except("DetService",
         "openContent FAILED. ConditionsContent object with name %s exists already.",
         nam.c_str());
  return Content();
}

/// Add a condition address to the content. Condition is identified by it's global key
bool DetService::_addContent(Content& content,
                             Condition::key_type key,
                             const std::string& address)    {
  if ( content )   {
    for(auto& c : m_openContents)    {
      if ( content == c.second )   {
        if ( c.second->insertKey(key, address) )   {
          return true;
        }
      }
    }
  }
  return false;
}

/// Add a condition address to the content. Condition is identified by it's global key
void DetService::addContent(Content& content,
                            Condition::key_type key,
                            const std::string& address)    {
  /// Fast return if all fine:
  if ( _addContent(content, key, address) )
    return;
  /// Handle errors:
  if ( content )   {
    for(auto& c : m_openContents)    {
      if ( content == c.second )   {
        except("DetService","Attempt to insert item with duplicate key: %016lX", key);
      }
    }
    except("DetService","This content object is not availible for manipulation. Key:%016X", key);
  }
  except("DetService","This content object is INVALID and cannot be manipulated. Key:%016X", key);
}

/// Add a condition address to the content
void DetService::addContent(Content& content,
                            DetElement det,
                            const string& item,
                            const string& address)   {
  dd4hep::ConditionKey key(det.key(), item);
  if ( _addContent(content, key.hash, address) )
    return;
  if ( content )   {
    for(auto& c : m_openContents)    {
      if ( content == c.second )   {
        except("DetService","Attempt to insert item with duplicate key:%s -> %s",
               det.path().c_str(), item.c_str());
      }
    }
    except("DetService","This content object is not availible for manipulation. %s -> %s",
           det.path().c_str(), item.c_str());
  }
  except("DetService","This content object is INVALID and cannot be manipulated. %s -> %s",
         det.path().c_str(), item.c_str());
}

/// Add a condition functor for a derived condition to the content
void DetService::addContent(Content& content, Dependency* dep)
{
  if ( content && dep )   {
    for(auto& c : m_openContents)  {
      if ( content == c.second && c.second->insertDependency(dep) )   {
        return;
      }
      except("DetService","Attempt to insert dependency with duplicate key.");
    }
    except("DetService","This content object is not availible for manipulation.");
  }
  except("DetService","This content object is INVALID and cannot be manipulated.");
}

/// Open content creation context
void DetService::closeContent(Content& content)   {
  if ( content )   {
    for(auto c=m_openContents.begin(); c!=m_openContents.end(); ++c)  {
      if ( content == (*c).second )   {
        Lock lock(m_contentLock);
        m_activeContents.insert(m_activeContents.end(),*c);
        m_openContents.erase(c);
        return;
      }
    }
    except("DetService","This content object is not availible for manipulation.");
  }
  except("DetService","This content object is INVALID and cannot be manipulated.");
}

/// Remove content from cache container
bool DetService::_remove(CachedContents& cache, Content& content)   {
  for(auto c=cache.begin(); c!=cache.end(); ++c)  {
    if ( content == (*c).second )   {
      cache.erase(c);
      return true;
    }
  }
  return false;
}

/// Close content creation context
bool DetService::removeContent(Content& cont)   {
  Lock lock(m_contentLock);
  if ( cont )   {
    bool ret = _remove(m_activeContents,cont)  ? true : _remove(m_openContents,cont);
    if ( ret ) cont.reset();
    return ret;
  }
  return false;
}

/// Age all slices and check for slices which have expired.
void DetService::_age()  {
  CachedSlices tmp = m_cache;
  m_cache.clear();
  m_cache.reserve(tmp.size());
  for( size_t i=0; i<tmp.size(); ++i ) {
    if ( ++tmp[i]->age > m_ageLimit && tmp[i]->slice.unique() ) continue;
    m_cache.push_back(tmp[i]);
    tmp[i] = 0;
  }
  for(auto* cc : tmp)   {
    if ( cc ) delete cc;
  }
}

/// Find an existing cached slice
DetService::Slice DetService::_find(Content& content,
                                    const IOVType* typ,
                                    EventStamp stamp)
{
  // When we are here we know that all arguments make sense.
  // No need here for any further checks
  dd4hep::IOV req_iov(typ,stamp);
  // First scan the cache: maybe we do not have to do anything.....
  for(CacheEntry* c : m_cache)    {
    if ( c->iov.iovType != typ ) continue;
    if ( content != c->slice->content ) continue;
    if ( dd4hep::IOV::key_is_contained(req_iov.keyData,c->iov.keyData) )  {
      c->age = 0;
      _age();
      c->age = 0;
      return c->slice;
    }
  }
  return  Slice();
}

/// Project a newly created conditions slice.
DetService::Slice DetService::_create(Content& content,
                                      Context *ctx,
                                      const IOVType* typ,
                                      EventStamp stamp)
{
  dd4hep::IOV req_iov(typ, stamp);
  unique_ptr<CacheEntry> e(new CacheEntry());
  e->slice.reset(new dd4hep::cond::ConditionsSlice(m_manager,content));
  m_manager.prepare(req_iov, *e->slice, ctx);
  m_cache.push_back(e.release());
  return m_cache.back()->slice;
}

/// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
DetService::Slice DetService::_project(Content& content,
                                       Context *ctx,
                                       const IOVType* typ,
                                       EventStamp stamp)   {
  Slice ptr;
  if ( content )  {
    Lock lock(m_sliceLock);
    if ( !(ptr=_find(content, typ, stamp)) )   {
      return _create(content, ctx, typ, stamp);
    }
    except("DetService","Invalid conditions content give. Cannot create slice.");
  }
  return ptr;
}

/// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
DetService::Slice DetService::project(Content& content,
                                      Context *ctx,
                                      const IOVType* typ,
                                      EventStamp stamp)   {
  /// Check if the givem IOV type is correct
  auto used_types = m_manager.iovTypesUsed();
  for(const auto* i : used_types)  {
    if ( i == typ )   {
      return _project(content, ctx, i, stamp);
    }
  }
  dd4hep::except("DetService",
                 "Request to project a conditions slice for an unknown IOV type.");
  return Slice();
}

/// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
DetService::Slice DetService::project(Content& content,
                                      Context *ctx,
                                      const string& typ,
                                      EventStamp stamp)   {
  /// Check if the givem IOV type is correct
  const IOVType* iovType = m_manager.iovType(typ);
  if ( iovType )  {
    return _project(content, ctx, iovType, stamp);
  }
  dd4hep::except("DetService",
                 "Request to project a conditions slice for an unknown IOV type: %s",
                 typ.c_str());
  return Slice();
}

/// Project a new conditions slice. If a free cached slice is availible, it shall be re-used
DetService::Slice DetService::project(const string& content,
                                      Context *ctx,
                                      const string& typ,
                                      EventStamp stamp)   {
  Content cont(getContent(content));
  if ( cont )   {
    return project(cont, ctx, typ, stamp);
  }
  dd4hep::except("DetService",
                 "Request to project a conditions slice for unknown content: %s",
                 content.c_str());
  return Slice();
}

/// Invoke slice cleanup
void DetService::cleanup(const Cleanup &cleaner)   {
  cleaner(m_manager);
}
