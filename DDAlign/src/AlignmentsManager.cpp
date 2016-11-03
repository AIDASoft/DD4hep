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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/MatrixHelpers.h"
#include "DDCond/ConditionsPool.h"
#include "DDAlign/AlignmentsManager.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    typedef Conditions::ConditionDependency Dependency;

    class AlignContext {
    public:
      struct Entry {
        const Dependency*           dep;
        DetElement::Object*         det;
        AlignmentCondition::Object* cond;
        unsigned int key, top;
      };
      struct PathOrdering {
        bool operator()(const DetElement& a, const DetElement& b) const
        { return a.path() < b.path(); }
      };
      typedef std::map<DetElement,size_t,PathOrdering>  DetectorMap;
      typedef std::map<unsigned int,size_t>             Keys;
      typedef std::vector<Entry>                        Entries;

      DetectorMap  detectors;
      Keys         keys;
      Entries      entries;
      unsigned long long int magic;
      AlignContext() : magic(magic_word()) {
        InstanceCount::increment(this);
      }
      ~AlignContext()  {
        InstanceCount::decrement(this);
      }
      void newEntry(DetElement det, const Dependency* dep, AlignmentCondition::Object* con) {
        if ( det.isValid() )  {
          Entry entry;
          unsigned int key = det.key();
          entry.top  = 0;
          entry.cond = con;
          entry.dep  = dep;
          entry.det  = det.ptr();
          entry.key  = key;
          detectors.insert(std::make_pair(det, entries.size()));
          keys.insert(std::make_pair(key, entries.size()));
          entries.insert(entries.end(), entry);
        }
      }
    };
    
    /// Access specialization
    template <> AlignmentsManager AlignmentsManager::from<LCDD>(LCDD& host)  {
      Object* obj = host.extension<Object>();
      if ( obj ) return AlignmentsManager(obj);
      except("AlignmentsManager","+++ Failed to access manager from LCDD.");
      return AlignmentsManager();
    }

  }       /* End namespace Alignments */
}         /* End namespace DD4hep     */


DD4HEP_INSTANTIATE_HANDLE_NAMED(AlignmentsManagerObject);

/// Initializing constructor
AlignmentsManagerObject::AlignmentsManagerObject() : NamedObject() {
  InstanceCount::increment(this);
  all_alignments = new AlignContext();
  dependencies = new Dependencies();
}
  
/// Default destructor
AlignmentsManagerObject::~AlignmentsManagerObject()   {
  dependencies->clear();
  deletePtr(dependencies);
  deletePtr(all_alignments);
  InstanceCount::decrement(this);
}

void AlignmentsManagerObject::to_world(AlignContext& new_alignments, UserPool& pool, DetElement det, TGeoHMatrix& mat)  const  {
  using Conditions::Condition;
  DetElement par = det.parent();
  while( par.isValid() )   {
    // If we find that the parent also got updated, directly take this transformation.
    // Since we compute top-down, it is already valid!
    AlignContext::Keys::const_iterator i = new_alignments.keys.find(par.key());
    if ( i != new_alignments.keys.end() )  {
      const AlignContext::Entry& e = new_alignments.entries[(*i).second];
      AlignmentCondition cond(e.cond);
      AlignmentData&     align = cond.data();
      mat.MultiplyLeft(&align.worldTransformation());
      return;
    }
    // The parent did not get updated: We have to search the the conditions pool if
    // there is a still valid condition, which we can use to build the world transformation
    // The parent's alignment condition by defintiion must be present in the pool,
    // since it got updated in the past!
    i = all_alignments->keys.find(par.key());
    if ( i != all_alignments->keys.end() )  {
      const AlignContext::Entry& e = all_alignments->entries[(*i).second];
      Condition::key_type key = e.dep->target.hash;
      AlignmentCondition cond = pool.get(key);
      AlignmentData&    align = cond.data();
      mat.MultiplyLeft(&align.worldTransformation());
      return;
    }
    // There is no special alignment for this detector element.
    // Hence to nominal (relative) transformation to the parent is valid
    mat.MultiplyLeft(&par.nominal().detectorTransformation());
    par = par.parent();
  }
}

/// Compute all alignment conditions of the internal dependency list
void AlignmentsManagerObject::compute(UserPool& user_pool)  const  {
  compute(user_pool, *dependencies);
}
  
/// Compute all alignment conditions of the specified dependency list
void AlignmentsManagerObject::compute(UserPool& pool, const Dependencies& deps) const  {
  AlignContext::DetectorMap::const_iterator i;
  AlignContext new_alignments;
  new_alignments.entries.reserve(deps.size());
  pool.compute(deps, &new_alignments);
  std::string prev = "-----";
  for(i=new_alignments.detectors.begin(); i!=new_alignments.detectors.end(); ++i)  {
    AlignContext::Entry& e = new_alignments.entries[(*i).second];
    DetElement         det = e.det;
    const std::string&   p = det.path();
    size_t idx = p.find(prev);
    if ( idx == 0 )  {
      continue;
    }
    prev = p;
    printout(DEBUG,"Alignment","Update top Node: Lvl:%d Key:%08X: %s", det.level(), det.key(), p.c_str());
    e.top = 1;
  }
  // We got now the top nodes of the new_alignments. From the top nodes we have to
  // recursively calculate all changes downwards the lower levels!
  // Note: The upper levels are already correct and do not need to be updated!
  printout(INFO,"Alignment","Working down the tree....");
  for(i=new_alignments.detectors.begin(); i != new_alignments.detectors.end(); ++i)  {
    AlignContext::Entry& e = new_alignments.entries[(*i).second];
    if ( e.top )     {
      compute(new_alignments, pool, e.det, 0);
    }
  }
}
/// Compute the alignment delta for one detector element and it's alignment condition
void computeDelta(AlignmentCondition cond, TGeoHMatrix& tr_delta)  {
  const AlignmentData& align = cond.data();
  const Delta&         delta = align.delta;
  const TGeoHMatrix&     nom = align.detectorTransformation();
  const Position&        pos = delta.translation;
  const Translation3D&   piv = delta.pivot;
  const RotationZYX&     rot = delta.rotation;

  switch(delta.flags)   {
  case Delta::HAVE_TRANSLATION+Delta::HAVE_ROTATION+Delta::HAVE_PIVOT:
    Geometry::_transform(tr_delta, Transform3D(Translation3D(pos)*piv*rot*(piv.Inverse())));
    break;
  case Delta::HAVE_TRANSLATION+Delta::HAVE_ROTATION:
    Geometry::_transform(tr_delta, Transform3D(rot,pos));
    break;
  case Delta::HAVE_ROTATION+Delta::HAVE_PIVOT:
    Geometry::_transform(tr_delta, Transform3D(piv*rot*(piv.Inverse())));
    break;
  case Delta::HAVE_ROTATION:
    Geometry::_transform(tr_delta, rot);
    break;
  case Delta::HAVE_TRANSLATION:
    Geometry::_transform(tr_delta, pos);
    break;
  default:
    break;
  }
  tr_delta.MultiplyLeft(&nom);
  align.detectorTrafo = tr_delta;
  align.worldTrafo    = tr_delta;
}
/// Compute all alignment conditions of the lower levels
void AlignmentsManagerObject::compute(AlignContext& new_alignments, UserPool& pool, DetElement det, int level) const  {
  AlignContext::Keys::const_iterator k=new_alignments.keys.find(det.key());
  bool has_cond    = k != new_alignments.keys.end();
  const AlignContext::Entry* ent = has_cond ? &new_alignments.entries[(*k).second] : 0;

  if ( ent )  {
    char fmt[128];
    TGeoHMatrix        tr_delta;
    AlignmentCondition cond(ent->cond);
    AlignmentData&     align = cond.data();
    computeDelta(cond, tr_delta);
    to_world(new_alignments, pool, det, align.worldTrafo);
    align.trToWorld = Geometry::_transform(&align.worldTrafo);
    ::snprintf(fmt,sizeof(fmt),"%%d %%%ds %%s %%08X: %%s IOV:%%s",2*level);
    printout(DEBUG,"ComputeAlignment",fmt,
             det.level(), "", has_cond ? "NO " : "YES",
             det.key(), det.path().c_str(), cond.iov().str().c_str());
  }
  else  {
    // DetElement 'det' has no specific alignment. If any of the children has one
    // and any of the parents, 'to_world' should pick up all the proper components
    // to build the proper world-transformation of the child.
    // Hence, we do not have to recompute it's value.
    // We just continue to trickle down to the children.
    //
    // Alternatively we could inject 'special' alignment conditions, which would depend
    // on the parent...
    // Under circumstances, this might be cheaper to re-compute.
  }
  const DetElement::Children& children = det.children();
  for(auto c=children.begin(); c!=children.end(); ++c)    {
    DetElement child = (*c).second;
    compute(new_alignments, pool, child, level+1);
  }
}

/// Initializing constructor
AlignmentsManager::AlignmentsManager(const std::string& nam)   {
  assign(new AlignmentsManagerObject(), nam, "alignmentmanager");
}

/// Initializing constructor
AlignmentsManager::AlignmentsManager(char const* nam)   {
  assign(new AlignmentsManagerObject(), nam ? nam : "", "alignmentmanager");
}

/// Delete the manager.
void AlignmentsManager::destroy()  {
  deletePtr(m_element);
}

/// Adopy alignment dependency for later recalculation
void AlignmentsManager::adoptDependency(Dependency* dependency) const  {
  Object* o = access();
  o->dependencies->insert(dependency);
  o->all_alignments->newEntry(dependency->detector, dependency, 0);
}

/// Access all known dependencies
const AlignmentsManager::Dependencies& AlignmentsManager::knownDependencies()  const   {
  return *(access()->dependencies);
}

/// Compute all alignment conditions of the internal dependency list
void AlignmentsManager::compute(dd4hep_ptr<UserPool>& user_pool) const   {
  Object* o = access();
  o->compute(*(user_pool.get()), *(o->dependencies));
}

/// Compute all alignment conditions of the specified dependency list
void AlignmentsManager::compute(dd4hep_ptr<UserPool>& user_pool, const Dependencies& deps) const  {
  access()->compute(*(user_pool.get()), deps);
}

/// Register new updated derived alignment during the computation step
void AlignmentsManager::newEntry(const Context& context,
                                 DetElement& det,
                                 const Dependency* dep,
                                 AlignmentCondition& con)    {
  // It must be ensured this is a valid object! Check magic word
  AlignContext* o = static_cast<AlignContext*>(context.parameter);
  if ( o && o->magic == magic_word() )  {
    o->newEntry(det, dep, con.ptr());
    return;
  }
}
