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
#include <DDAlign/AlignmentsCalib.h>

#include <DD4hep/Memory.h>
#include <DD4hep/Printout.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/detail/AlignmentsInterna.h>

using namespace dd4hep::align;

/// Helper class to store information about alignment calibration items
/**  Implementation details: Alignment context entry
 *
 *   \author  M.Frank
 *   \version 1.0
 *   \date    31/01/2017
 *   \ingroup DD4HEP_DDALIGN
 */
class AlignmentsCalib::Entry   {
public:
  dd4hep::Delta                  delta;
  dd4hep::Delta                  original;
  dd4hep::Condition              source;
  dd4hep::DetElement             detector;
  dd4hep::Condition::key_type    target = 0;
  int                            dirty  = 0;
  Entry() = default;
  Entry(const Entry& c) = delete;
  Entry& operator=(const Entry& c) = delete;
};
 
/// Initializing constructor
AlignmentsCalib::AlignmentsCalib(Detector& det, ConditionsMap& cond_map)
  : description(det), slice(cond_map)
{
}

/// Default destructor
AlignmentsCalib::~AlignmentsCalib()   noexcept(false)  {
  clear();
}

/// Convenience only: Access detector element by path
dd4hep::DetElement AlignmentsCalib::detector(const std::string& path)  const   {
  DetElement det(detail::tools::findElement(description,path));
  return det;
}

/// Implementation: Add a new entry to the transaction stack.
std::pair<dd4hep::Condition::key_type,AlignmentsCalib::Entry*>
AlignmentsCalib::_set(DetElement detector, const Delta& delta)   {
  ConditionKey tar_key(detector.key(),Keys::alignmentKey);
  UsedConditions::iterator i = used.find(tar_key.hash);
  if ( i != used.end() )   {
    (*i).second->delta = delta;
    return (*i);
  }

  Condition src_cond  = slice.get(detector,Keys::deltaKey);
  if ( !src_cond.isValid() )   {
    // Try to create a new condition and register it to the
    // conditions manager from the delta value.
    ConditionKey key(detector, Keys::deltaKey);
    src_cond = Condition(detector.path()+"#"+Keys::deltaName,Keys::deltaName);
    src_cond.bind<Delta>();
    src_cond->setFlag(Condition::ALIGNMENT_DELTA);
    src_cond->hash = key.hash;
    if ( !slice.insert(detector, Keys::deltaKey, src_cond) )   {
      detail::destroyHandle(src_cond);
    }
    /// Now check again if we succeeded.
    if ( !src_cond.isValid() )   {
      except("AlignmentsCalib",
             "++ The SOURCE alignment condition [%016llX]: %s#%s is invalid.",
             key.hash, detector.path().c_str(), Keys::deltaName.c_str());
    }
  }
  // Add the entry the usual way. This should also check everything again.
  dd4hep_ptr<Entry>  entry(new Entry());
  entry->original = src_cond.get<Delta>();
  entry->delta    = delta;
  entry->detector = detector;
  entry->source   = src_cond;
  entry->target   = tar_key.hash;
  entry->dirty    = 1;
  return *(used.emplace(tar_key,entry.release()).first);
}

/// (1) Add a new entry to an existing DetElement structure.
dd4hep::Condition::key_type
AlignmentsCalib::set(DetElement det, const Delta& delta)   {
  return _set(det.access(), delta).first;
}

/// (2) Add a new entry to an existing DetElement structure.
dd4hep::Condition::key_type
AlignmentsCalib::set(const std::string& path, const Delta& delta)   {
  return _set(detector(path).access(), delta).first;
}

/// Clear all delta data in the caches transaction stack.
void AlignmentsCalib::clearDeltas()   {
  if ( !used.empty() )  {
    for ( auto& e : used )  {
      e.second->delta.clear();
      e.second->dirty = 0;
    }
  }
}

/// Clear all pending entries in the working cache
void AlignmentsCalib::clear()   noexcept(false)  {
  for(auto& e : used)   {
    e.second->source.get<Delta>() = e.second->delta;
    detail::deletePtr(e.second);
  }
  used.clear();
}

/// Commit all pending transactions. Returns number of altered entries
AlignmentsCalculator::Result AlignmentsCalib::commit()   {
  std::map<DetElement, Delta> deltas;
  AlignmentsCalculator        calculator;

  /// Propagate the Delta values to the source conditions
  for ( auto& entry : used )  {
    Entry* e = entry.second;
    if ( e->dirty )  {
      /// Update the source condition with the new delta value
      deltas[e->detector]    = e->delta;
      e->source.get<Delta>() = e->delta;
      e->dirty = 0;
    }
  }
  return calculator.compute(deltas, slice);
}
