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
#include "DD4hep/ConditionDerived.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsDependencyCollection.h"
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
        unsigned int key, top, valid;
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
          entry.valid = 0;
          entry.top   = 0;
          entry.cond  = con;
          entry.dep   = dep;
          entry.det   = det.ptr();
          entry.key   = key;
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
static PrintLevel s_PRINT = WARNING;

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

AlignmentsManager::Result
AlignmentsManagerObject::to_world(AlignContext& new_alignments,
                                  UserPool&     pool,
                                  DetElement    det,
                                  TGeoHMatrix&  delta_to_world)  const
{
  using Conditions::Condition;
  Result result;
  DetElement par = det.parent();
  while( par.isValid() )   {
    // If we find that the parent also got updated, directly take this transformation.
    // Since we compute top-down, it is already valid!
    AlignContext::Keys::const_iterator i = new_alignments.keys.find(par.key());
    if ( i != new_alignments.keys.end() )  {
      const AlignContext::Entry& e = new_alignments.entries[(*i).second];
      // The parent entry is (not yet) valid. need to compute it first
      if ( 0 == e.valid )  {
        Result r = compute(new_alignments, pool, par);
        result.missing += r.missing;
        result.computed += r.computed;
      }
      AlignmentCondition cond(e.cond);
      AlignmentData&     align = cond.data();
      if ( s_PRINT <= INFO )  {
        printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
        printf("  with ALIGN(world) %s :", par.path().c_str());
        align.worldDelta.Print();
      }
      delta_to_world.MultiplyLeft(&align.worldDelta);
      if ( s_PRINT <= INFO )  {
        printf("  Result :"); delta_to_world.Print();
      }
      ++result.computed;
      return result;
    }
    // The parent did not get updated: We have to search the conditions pool if
    // there is a still valid condition, which we can use to build the world transformation
    // The parent's alignment condition by defintiion must be present in the pool,
    // since it got updated in the past!
    i = all_alignments->keys.find(par.key());
    if ( i != all_alignments->keys.end() )  {
      const AlignContext::Entry& e = all_alignments->entries[(*i).second];
      Condition::key_type key = e.dep->target.hash;
      AlignmentCondition cond = pool.get(key);
      AlignmentData&    align = cond.data();
      if ( s_PRINT <= INFO )  {
        printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
        printf("  with ALIGN(world) %s :", par.path().c_str());
        align.worldDelta.Print();
      }
      delta_to_world.MultiplyLeft(&align.worldDelta);
      if ( s_PRINT <= INFO ) {
        printf("  Result :"); delta_to_world.Print();
      }
      ++result.computed;
      return result;
    }
    // There is no special alignment for this detector element.
    // Hence to nominal (relative) transformation to the parent is valid
    if ( s_PRINT <= INFO )  {
      printf("Multiply-left ALIGNMENT %s:", det.path().c_str()); delta_to_world.Print();
      printf("  with NOMINAL(det) %s :", par.path().c_str());
      par.nominal().detectorTransformation().Print();
    }
    delta_to_world.MultiplyLeft(&par.nominal().detectorTransformation());
    if ( s_PRINT <= INFO )  {
      printf("  Result :"); delta_to_world.Print();
    }
    par = par.parent();
  }
  ++result.computed;
  return result;
}

/// Compute all alignment conditions of the internal dependency list
AlignmentsManager::Result AlignmentsManagerObject::compute(Pool& pool)  const  {
  return compute(pool, *dependencies);
}
  
/// Compute all alignment conditions of the specified dependency list
AlignmentsManager::Result AlignmentsManagerObject::compute(Pool& pool, const Dependencies& deps) const  {
  Result result;
  AlignContext new_alignments;
  new_alignments.entries.reserve(deps.size());
  //
  // This here is the main difference compared to other derived conditions:
  // ----------------------------------------------------------------------
  //
  // We enforce here that all computations, which require an update of the corresponding
  // alignment matrices are stored in "new_alignments", since the update callback registers
  // all new entries using this user parameter when calling  AlignmentsManager::newEntry.
  // For this reason also ALL specific update calls must base themself in the
  // Alignment update callback.
  //
  pool.compute(deps, &new_alignments);
  for(auto i=new_alignments.entries.begin(); i != new_alignments.entries.end(); ++i)  {
    Result r = compute(new_alignments, pool, (*i).det);
    result.computed += r.computed;
    result.missing  += r.missing;
  }
  return result;
}

/// Compute the alignment delta for one detector element and it's alignment condition
static void computeDelta(AlignmentCondition cond, TGeoHMatrix& tr_delta)  {
  const AlignmentData& align = cond.data();
  const Delta&         delta = align.delta;
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
}

/// Compute all alignment conditions of the lower levels
AlignmentsManager::Result
AlignmentsManagerObject::compute(AlignContext& new_alignments, UserPool& pool, DetElement det) const  {
  Result result, temp;
  auto k=new_alignments.keys.find(det.key());
  bool has_cond = (k != new_alignments.keys.end());
  AlignContext::Entry* ent = has_cond ? &new_alignments.entries[(*k).second] : 0;

  if ( ent && ent->valid == 1 )  {
    printout(DEBUG,"ComputeAlignment","================ IGNORE %s (already valid)",det.path().c_str());
    return result;
  }
  if ( ent && ent->valid == 0 )  {
    TGeoHMatrix        tr_delta;
    AlignmentCondition cond  = ent->cond;
    AlignmentData&     align = cond.data();
    if ( s_PRINT <= INFO )  {  
      printout(INFO,"ComputeAlignment",
               "============================== Compute transformation of %s ============================== ",
               det.path().c_str());
    }
    ent->valid          = 1;
    computeDelta(cond, tr_delta);
    align.worldDelta    = tr_delta;
    temp = to_world(new_alignments, pool, det, align.worldDelta);
    result.computed += temp.computed;
    result.missing  += temp.missing;
    align.worldTrafo    = det.nominal().worldTransformation()*align.worldDelta;
    align.detectorTrafo = det.nominal().detectorTransformation()*tr_delta;
    align.trToWorld     = Geometry::_transform(&align.worldDelta);
    if ( s_PRINT <= INFO )  {  
      printout(INFO,"ComputeAlignment","Level:%d Path:%s DetKey:%08X: Cond:%s key:%16llX IOV:%s",
               det.level(), det.path().c_str(), det.key(),
               yes_no(has_cond), cond.key(), cond.iov().str().c_str());
    }
    if ( s_PRINT <= DEBUG )  {  
      printf("DetectorTrafo: '%s' -> '%s' ",det.path().c_str(), det.parent().path().c_str());
      det.nominal().detectorTransformation().Print();
      printf("Delta:       '%s' ",det.path().c_str()); tr_delta.Print();
      printf("World-Delta: '%s' ",det.path().c_str()); align.worldDelta.Print();
      printf("Nominal:     '%s' ",det.path().c_str()); det.nominal().worldTransformation().Print();
      printf("Result:      '%s' ",det.path().c_str()); align.worldTrafo.Print();
    }
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
    //
    // NOT HANDLED HERE!
    // Solution uses derived conditions to generate AlignmentCondition
    // objects with an empty Delta.
    //
    // Handled in the alignment plugin: DDAlign_AlignmentForward,
    // to be executed AFTER DDAlign_AlignmentRegister.
    // This plugin will inject the relevant dependency calls.
    // 
  }
  const DetElement::Children& children = det.children();
  for(auto c=children.begin(); c!=children.end(); ++c)    {
    DetElement child = (*c).second;
    temp = compute(new_alignments, pool, child);
    result.computed += temp.computed;
    result.missing  += temp.missing;
  }
  return result;
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
bool AlignmentsManager::adoptDependency(Dependency* dependency) const  {
  Object* o = access();
  auto res = o->dependencies->insert(dependency);
  if ( res.second )  {
    o->all_alignments->newEntry(dependency->detector, dependency, 0);
    return res.second;
  }
  return false;
}

/// Access all known dependencies
const AlignmentsManager::Dependencies& AlignmentsManager::knownDependencies()  const   {
  return *(access()->dependencies);
}

/// Compute all alignment conditions of the internal dependency list
AlignmentsManager::Result AlignmentsManager::compute(Slice& slice) const   {
  Object* o = access();
  return o->compute(*slice.pool, *(o->dependencies));
}

/// Compute all alignment conditions of the specified dependency list
AlignmentsManager::Result AlignmentsManager::compute(Slice& slice, const Dependencies& deps) const  {
  return access()->compute(*slice.pool, deps);
}

/// Compute all alignment conditions of the internal dependency list
AlignmentsManager::Result AlignmentsManager::compute(Pool& pool) const   {
  Object* o = access();
  return o->compute(pool, *(o->dependencies));
}

/// Compute all alignment conditions of the specified dependency list
AlignmentsManager::Result AlignmentsManager::compute(Pool& pool, const Dependencies& deps) const  {
  return access()->compute(pool, deps);
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
