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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDAlign/GlobalAlignmentCache.h"
#include "DDAlign/GlobalAlignmentOperators.h"
#include "DD4hep/objects/DetectorInterna.h"

// ROOT include files
#include "TGeoManager.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;
using namespace DD4hep::Alignments::DDAlign_standard_operations;
typedef GlobalAlignmentStack::StackEntry Entry;

DetElement _detector(DetElement child)   {
  if ( child.isValid() )   {
    DetElement p(child.parent());
    if ( p.isValid() && !p.parent().isValid() )
      return child;
    else if ( !p.isValid() )  // World detector element...
      return child;
    return _detector(p);
  }
  throw runtime_error("DD4hep: DetElement cannot determine detector parent [Invalid handle]");
}

/// Default constructor
GlobalAlignmentCache::GlobalAlignmentCache(LCDD& lcdd, const string& sdPath, bool top)
  : m_lcdd(lcdd), m_sdPath(sdPath), m_sdPathLen(sdPath.length()), m_refCount(1), m_top(top)
{
}

/// Default destructor
GlobalAlignmentCache::~GlobalAlignmentCache()   {
  int nentries = (int)m_cache.size();
  int nsect = (int)m_detectors.size();
  releaseObjects(m_detectors);
  m_cache.clear();
  printout(INFO,"GlobalAlignmentCache",
           "Destroy cache for subdetector %s [%d section(s), %d entrie(s)]",
           m_sdPath.c_str(),nsect,nentries);
}

/// Add reference count
int GlobalAlignmentCache::addRef()   {
  return ++m_refCount;
}

/// Release object. If reference count goes to NULL, automatic deletion is triggered.
int GlobalAlignmentCache::release()   {
  int value = --m_refCount;
  if ( value == 0 )  {
    delete this;
  }
  return value;
}

/// Create and install a new instance tree
GlobalAlignmentCache* GlobalAlignmentCache::install(LCDD& lcdd)   {
  GlobalAlignmentCache* cache = lcdd.extension<GlobalAlignmentCache>(false);
  if ( !cache )  {
    lcdd.addExtension<GlobalAlignmentCache>(new GlobalAlignmentCache(lcdd,"world",true));
  }
  return cache;
}

/// Unregister and delete a tree instance
void GlobalAlignmentCache::uninstall(LCDD& lcdd)   {
  if ( lcdd.extension<GlobalAlignmentCache>(false) )  {
    lcdd.removeExtension<GlobalAlignmentCache>(true);
  }
}

/// Add a new entry to the cache. The key is the placement path
bool GlobalAlignmentCache::insert(GlobalAlignment alignment)  {
  TGeoPhysicalNode* pn = alignment.ptr();
  unsigned int index = hash32(pn->GetName()+m_sdPathLen);
  Cache::const_iterator i = m_cache.find(index);
  printout(ALWAYS,"GlobalAlignmentCache","Section: %s adding entry: %s",
           name().c_str(),alignment->GetName());
  if ( i == m_cache.end() )   {
    m_cache[index] = pn;
    return true;
  }
  return false;
}

/// Retrieve the cache section corresponding to the path of an entry.
GlobalAlignmentCache* GlobalAlignmentCache::section(const string& path_name) const   {
  size_t idx, idq;
  if ( path_name[0] != '/' )   {
    return section(m_lcdd.world().placementPath()+'/'+path_name);
  }
  else if ( (idx=path_name.find('/',1)) == string::npos )  {
    return (m_sdPath == path_name.c_str()+1) ? (GlobalAlignmentCache*)this : 0;
  }
  else if ( m_detectors.empty() )  {
    return 0;
  }
  if ( (idq=path_name.find('/',idx+1)) != string::npos ) --idq;
  string path = path_name.substr(idx+1,idq-idx);
  SubdetectorAlignments::const_iterator j = m_detectors.find(path);
  return (j==m_detectors.end()) ? 0 : (*j).second;
}

/// Retrieve an alignment entry by its placement path
GlobalAlignment GlobalAlignmentCache::get(const string& path_name) const   {
  size_t idx, idq;
  unsigned int index = hash32(path_name.c_str()+m_sdPathLen);
  Cache::const_iterator i = m_cache.find(index);
  if ( i != m_cache.end() )  {
    return GlobalAlignment((*i).second);
  }
  else if ( m_detectors.empty() )  {
    return GlobalAlignment(0);
  }
  else if ( path_name[0] != '/' )   {
    return get(m_lcdd.world().placementPath()+'/'+path_name);
  }
  else if ( (idx=path_name.find('/',1)) == string::npos )  {
    // Escape: World volume and not found in cache --> not present
    return GlobalAlignment(0);
  }
  if ( (idq=path_name.find('/',idx+1)) != string::npos ) --idq;
  string path = path_name.substr(idx+1,idq-idx);
  SubdetectorAlignments::const_iterator j = m_detectors.find(path);
  if ( j != m_detectors.end() ) return (*j).second->get(path_name);
  return GlobalAlignment(0);
}

/// Return all entries matching a given path.
vector<GlobalAlignment> GlobalAlignmentCache::matches(const string& match, bool exclude_exact) const   {
  vector<GlobalAlignment> result;
  GlobalAlignmentCache* c = section(match);
  if ( c )  {
    size_t len = match.length();
    result.reserve(c->m_cache.size());
    for(Cache::const_iterator i=c->m_cache.begin(); i!=c->m_cache.end();++i)  {
      const Cache::value_type& v = *i;
      const char* n = v.second->GetName();
      if ( 0 == ::strncmp(n,match.c_str(),len) )   {
        if ( exclude_exact && len == ::strlen(n) ) continue;
        result.push_back(GlobalAlignment(v.second));
      }
    }
  }
  return result;
}

/// Close existing transaction stack and apply all alignments
void GlobalAlignmentCache::commit(GlobalAlignmentStack& stack)   {
  TGeoManager& mgr = m_lcdd.manager();
  mgr.UnlockGeometry();
  apply(stack);
  mgr.LockGeometry();
}

/// Retrieve branch cache by name. If not present it will be created
GlobalAlignmentCache* GlobalAlignmentCache::subdetectorAlignments(const string& nam)    {
  SubdetectorAlignments::const_iterator i = m_detectors.find(nam);
  if ( i == m_detectors.end() )   {
    GlobalAlignmentCache* ptr = new GlobalAlignmentCache(m_lcdd,nam,false);
    m_detectors.insert(make_pair(nam,ptr));
    return ptr;
  }
  return (*i).second;
}

/// Apply a complete stack of ordered alignments to the geometry structure
void GlobalAlignmentCache::apply(GlobalAlignmentStack& stack)    {
  typedef map<string,DetElement> DetElementUpdates;
  typedef map<DetElement,vector<Entry*> > sd_entries_t;
  TGeoManager& mgr = m_lcdd.manager();
  DetElementUpdates detelt_updates;
  sd_entries_t all;

  while(stack.size() > 0)    {
    Entry* e = stack.pop().release();
    DetElement det = _detector(e->detector);
    all[det].push_back(e);
    if ( e->hasMatrix() || e->needsReset() || e->resetChildren() )  {
      detelt_updates.insert(make_pair(e->detector.path(),e->detector));
    }
  }
  for(sd_entries_t::iterator i=all.begin(); i!=all.end(); ++i)  {
    DetElement det((*i).first);
    GlobalAlignmentCache* sd_cache = subdetectorAlignments(det.placement().name());
    sd_cache->apply( (*i).second );
    (*i).second.clear();
  }

  printout(INFO,"GlobalAlignmentCache","Alignments were applied. Refreshing physical nodes....");
  mgr.GetCurrentNavigator()->ResetAll();
  mgr.GetCurrentNavigator()->BuildCache();
  mgr.RefreshPhysicalNodes();

  // Provide update callback for every detector element with a changed placement
  for(DetElementUpdates::iterator i=detelt_updates.begin(); i!=detelt_updates.end(); ++i)  {
    DetElement elt((*i).second);
    printout(DEBUG,"GlobalAlignmentCache","+++ Trigger placement update for %s [2]",elt.path().c_str());
    elt->update(DetElement::PLACEMENT_CHANGED|DetElement::PLACEMENT_ELEMENT,elt.ptr());
  }
  // Provide update callback for the highest detector element
  string last_path = "?????";
  for(DetElementUpdates::iterator i=detelt_updates.begin(); i!=detelt_updates.end(); ++i)  {
    const string& path = (*i).first;
    if ( path.find(last_path) == string::npos )  {
      DetElement elt((*i).second);
      printout(DEBUG,"GlobalAlignmentCache","+++ Trigger placement update for %s [1]",elt.path().c_str());
      elt->update(DetElement::PLACEMENT_CHANGED|DetElement::PLACEMENT_HIGHEST,elt.ptr());
      last_path = (*i).first;
    }
  }
  // Provide update callback at the detector level
  for(sd_entries_t::iterator i=all.begin(); i!=all.end(); ++i)  {
    DetElement elt((*i).first);
    printout(DEBUG,"GlobalAlignmentCache","+++ Trigger placement update for %s [0]",elt.path().c_str());
    elt->update(DetElement::PLACEMENT_CHANGED|DetElement::PLACEMENT_DETECTOR,elt.ptr());
  }
}

/// Apply a vector of SD entries of ordered alignments to the geometry structure
void GlobalAlignmentCache::apply(const vector<Entry*>& changes)   {
  typedef map<string,pair<TGeoPhysicalNode*,Entry*> > Nodes;
  Nodes nodes;
  GlobalAlignmentSelector selector(*this,nodes,changes);
  for_each(m_cache.begin(),m_cache.end(),selector.reset());
  for_each(nodes.begin(),nodes.end(),GlobalAlignmentActor<node_print>(*this,nodes));
  for_each(nodes.begin(),nodes.end(),GlobalAlignmentActor<node_reset>(*this,nodes));

  for_each(changes.begin(),changes.end(),selector.reset());
  for_each(nodes.begin(),nodes.end(),GlobalAlignmentActor<node_align>(*this,nodes));
  for_each(nodes.begin(),nodes.end(),GlobalAlignmentActor<node_delete>(*this,nodes));
}
