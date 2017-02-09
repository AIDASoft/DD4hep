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

// Framework includes
#include "DDAlign/AlignmentsCalib.h"
#include "DDAlign/AlignmentsUpdateCall.h"

#include "DD4hep/Printout.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/DetectorTools.h"
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DD4hep/objects/AlignmentsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;
using Conditions::Condition;
using Conditions::ConditionKey;
using Conditions::ConditionsPool;
using Conditions::ConditionsSlice;
using Conditions::ConditionsManager;
using Conditions::ConditionDependency;

// ======================================================================================

/// Initializing constructor
AlignmentsCalib::AlignmentsCalib(LCDD& l, Slice& s) : lcdd(l), slice(s), derivationCall(0)
{
}

/// Default destructor
AlignmentsCalib::~AlignmentsCalib()   {
}

/// Add a new entry to the transaction list
AlignmentsCalib::key_type AlignmentsCalib::_insert(const pair<Condition::key_type,Entry*>& e)  {
  auto res = used.insert(e);
  if ( res.second ) return (*res.first).first;
  return 0;
}

/// Add a new entry to the transaction stack.
AlignmentsCalib::key_type AlignmentsCalib::_use(AlignmentCondition alignment_condition)   {
  key_type           tar_key = alignment_condition.key();
  UsedConditions::iterator i = used.find(tar_key);
  if ( i != used.end() )   {
    return (*i).first;
  }
  
  dd4hep_ptr<Entry>  entry(new Entry());
  UserPool*          pool     = slice.pool.get();
  Condition cc(alignment_condition);
  printout(INFO,"Ex","Check key: %p %p -> %s",(void*)alignment_condition.key(),(void*)cc.key(),
           alignment_condition.name());

  /// This may be the derived condition: check presence  
  cc = pool->get(tar_key);
  AlignmentCondition align_cond = pool->get(tar_key);
  if ( !align_cond.isValid() )  {
    except("AlignmentsCalib","++ The DERIVED alignment condition [%p] is invalid.",(void*)tar_key);
  }
  key_type  src_key = align_cond->source_key;
  Condition src_cond = pool->get(src_key);
  if ( !src_cond.isValid() )  {
    except("AlignmentsCalib","++ The SOURCE alignment condition [%p] for %s is invalid.",
           (void*)src_key, align_cond.name());
  }
  /// Check for the source data entry
  Proxy::const_iterator is = slice.conditions().find(src_key);
  if ( is == slice.conditions().end() )  {
    slice.insert(src_cond);
    is = slice.conditions().find(src_key);
  }
  entry->source = (*is).second;
  /// Check for the derived conditions entry
  Proxy::const_iterator ip = slice.derived().find(tar_key);
  if ( ip == slice.derived().end() )  {
    if ( derivationCall )  {
      Conditions::DependencyBuilder b(ConditionKey(align_cond.name(),align_cond.key()),derivationCall);
      b.add(ConditionKey(src_cond.name(),src_cond.key()));
      slice.insert(b.release());
      ip = slice.derived().find(tar_key);
    }
    else  {
      except("AlignmentsCalib","++ The dependency rule for [%p] %s cannot be added [no callback].",
             (void*)tar_key, align_cond.name());
    }
  }
  entry->target = (*ip).second;
  return _insert(make_pair(tar_key,entry.release()));
}

/// Registers a new condition to the conditions manager. Throws exception on failure
Condition AlignmentsCalib::_register(Condition cond)  const  {
  if ( cond.isValid() )  {
    // Need to add the new condition to the IOV pool
    // using the user pool's IOV.
    const IOV&      iov  = slice.pool->validity();
    ConditionsPool* pool = slice.manager.registerIOV(*iov.iovType, iov.keyData);
    // We must simultaneously add the newly created condition to
    // -- the IOV pool using the conditions manager
    slice.manager.registerUnlocked(pool, cond);    
    // -- the slice and the user pool
    ConditionKey key(cond.name(),cond.key());
    slice.insert(key,slice.loadInfo(cond.address()));
    slice.insert(cond);
    printout(INFO,"Ex","Inserted key: %p -> %s",(void*)key.hash,key.name.c_str());
    return cond;
  }
  invalidHandleError<Condition>();
  return Condition();
}

/// Implementation: Add a new raw(delta)-condition to the transaction stack.
Condition AlignmentsCalib::_create_source(key_type      key,
                                          const string& nam)  const
{
  Condition cond(nam,"alignment");
  cond.bind<Delta>();
  cond->setFlag(Condition::ALIGNMENT);
  cond->validate();
  cond->hash = key;
  return _register(cond);
}

/// Implementation: Add a new alignment-condition to the transaction stack.
Condition AlignmentsCalib::_create_target(DetElement    detector,
                                          key_type      key,
                                          const string& nam)  const
{
  AlignmentCondition cond(nam);
  AlignmentData&     data = Condition(cond).get<AlignmentData>();
  data.detector = detector;
  cond->setFlag(Condition::ALIGNMENT_DERIVED);
  cond->invalidate();
  cond->hash = key;
  return _register(cond);
}

/// (1) Add a new entry to the transaction stack.
AlignmentsCalib::key_type AlignmentsCalib::use(Alignment alignment)   {
  AlignmentCondition cond = alignment.condition();
  cond.access(); // Throw exception if invalid.
  return _use(cond);
}

/// (2) Add a new entry to an existing DetElement structure.
AlignmentsCalib::key_type AlignmentsCalib::use(DetElement detector,
                                               const string& src_nam,
                                               const string& align_nam)
{
  UserPool* pool     = slice.pool.get();
  key_type  src_key  = ConditionKey::hashCode(src_nam);
  Condition src_cond = pool->get(src_key);

  if ( !src_cond.isValid() )  {
    /// Try to create a new condition and register it to the conditions manager from the delta value.
    src_cond = _create_source(src_key, src_nam);
    /// Now check again if we succeeded.
    if ( !src_cond.isValid() )   {
      except("AlignmentsCalib","++ The SOURCE alignment condition [%p]: %s is invalid.",
             (void*)src_key, src_nam.c_str());
    }
    // Add the conditons keys to the detector element:
    Conditions::DetConditions conditions(detector);
    Conditions::Container     cond_cont = conditions.conditions();
    cond_cont.insertKey(src_nam);
  }

  /// This may be the derived condition: check presence
  string   tar_nam = align_nam.empty() ? src_nam+"/Transformations" : align_nam;
  key_type tar_key = ConditionKey::hashCode(tar_nam);
  AlignmentCondition align_cond = pool->get(tar_key);
  if ( !align_cond.isValid() )  {
    /// Try to create a new condition and register it to the conditions manager from the delta value.
    align_cond = _create_target(detector, tar_key, tar_nam);
    /// Now check again if we succeeded.
    if ( !align_cond.isValid() )  {
      except("AlignmentsCalib","++ The DERIVED alignment condition [%p]: %s is invalid.",
             (void*)tar_key, tar_nam.c_str());
    }
    // Add the conditons keys to the detector element:
    Conditions::DetConditions conditions(detector);
    Conditions::Container     cond_cont = conditions.conditions();
    cond_cont.insertKey("Alignment", tar_nam);
    cond_cont.insertKey(tar_nam);

    // Add the corresponding alignment keys:
    DetAlign da(detector);
    Alignments::Container align_cont = da.alignments();
    align_cont.insertKey("Alignment",tar_nam);
    align_cont.insertKey(tar_nam);
  }
  align_cond->source_key = src_key;

  // Now all pre-conditions should be satisfied.
  // Add the entry the usual way. This should also check everything again.
  return _use(align_cond);
}

/// (3) Add a new entry to an existing DetElement structure.
AlignmentsCalib::key_type AlignmentsCalib::use(DetElement detector)   {
  if ( detector.isValid() )  {
    string name = detector.path()+"#alignment";
    return use(detector,name);
  }
  invalidHandleError<DetElement>();
  return 0;
}

/// (4) Add a new entry to an existing DetElement structure.
AlignmentsCalib::key_type AlignmentsCalib::use(const std::string& detector,
                                               const string& src_nam,
                                               const string& align_nam)   {
  DetElement det(Geometry::DetectorTools::findElement(lcdd,detector));
  if ( det.isValid() )  {
    return use(det, src_nam, align_nam);
  }
  invalidHandleError<DetElement>();
  return 0;
}

/// (5) Add a new entry to an existing DetElement structure.
AlignmentsCalib::key_type AlignmentsCalib::use(const std::string& detector)   {
  DetElement det(Geometry::DetectorTools::findElement(lcdd,detector));
  if ( det.isValid() )  {
    return use(det);
  }
  invalidHandleError<DetElement>();
  return 0;
}

/// Clear all delta data in the caches transaction stack.
void AlignmentsCalib::clearDeltas()   {
  if ( !used.empty() )  {
    Delta empty;
    for ( auto& e : used )  {
      e.second->delta = empty;
      e.second->dirty = false;
    }
  }
}

/// Commit all pending transactions. Returns number of altered entries
void AlignmentsCalib::clear()   {
  for(auto& e : used) delete e.second;
  used.clear();
}

/// Set a new delta value in the transaction stack.
bool AlignmentsCalib::setDelta(Condition::key_type key_val, const Delta& delta)  {
  UsedConditions::iterator i = used.find(key_val);
  if ( i != used.end() )   {
    (*i).second->delta = delta;
    (*i).second->dirty = 1;
    return true;
  }
  except("AlignmentsCalib","++ FAILED setDelta: invalid entry id: %p",(void*)key_val);
  return false;
}

/// Set a new delta value in the transaction stack.
bool AlignmentsCalib::setDelta(const ConditionKey& key, const Delta& delta)   {
  return setDelta(key.hash, delta);
}

/// Propagate all Delta parameters to the source conditions
bool AlignmentsCalib::propagateDeltas()   {
  UserPool* pool = slice.pool.get();
  for ( const auto& entry : used )  {
    const Entry* e = entry.second;

    /// Invalidate the target condition to trigger the computation.
    Condition tar_cond = pool->get(e->target->key.hash);
    if ( !tar_cond.isValid() )  {
      except("AlignmentsCalib","++ The DERIVED alignment condition [%p]: %s is invalid.",
             (void*)e->target->key.hash, e->target->key.name.c_str());
    }
    tar_cond->invalidate();

    /// Update the source condition with the new delta value
    Condition src_cond = pool->get(e->source->key.hash);
    if ( !src_cond.isValid() )  {
      except("AlignmentsCalib","++ The SOURCE alignment condition [%p]: %s is invalid.",
             (void*)e->source->key.hash, e->source->key.name.c_str());
    }
    src_cond.get<Delta>() = e->delta;
    src_cond->validate();
  }
  return true;
}

/// Update Dependencies between the source conditions and the computations
bool AlignmentsCalib::updateDependencies()   {
  // As long as we do not add new conditions, the old entries are still valid and present.
  return true;
}

/// Compute all dependent conditions from the Delta parameters
AlignmentsManager::Result AlignmentsCalib::computeDependencies()   {
  AlignmentsManager mgr;
  return mgr.compute(slice);
}

/// Commit all pending transactions. Returns number of altered entries
AlignmentsManager::Result AlignmentsCalib::commit()   {
  if ( propagateDeltas() )  {
    if ( updateDependencies() )  {
      return computeDependencies();
    }
  }
  return AlignmentsManager::Result();
}

