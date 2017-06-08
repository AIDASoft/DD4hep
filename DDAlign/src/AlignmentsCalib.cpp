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

#include "DD4hep/Memory.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/detail/AlignmentsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;
typedef Conditions::Condition::key_type key_type;

/**  Implementation details: Alignment context entry
 *
 *   \author  M.Frank
 *   \version 1.0
 *   \date    31/01/2017
 *   \ingroup DD4HEP_DDALIGN
 */
class AlignmentsCalib::Entry   {
public:
  Delta       delta;
  Delta       original;
  Condition   source;
  DetElement  detector;
  key_type    target = 0;
  int         dirty  = 0;
  Entry() = default;
  Entry(const Entry& c) = delete;
  Entry& operator=(const Entry& c) = delete;
};
 
/// Initializing constructor
AlignmentsCalib::AlignmentsCalib(LCDD& l, ConditionsMap& m) : lcdd(l), slice(m)
{
}

/// Default destructor
AlignmentsCalib::~AlignmentsCalib()   {
  clear();
}

/// Convenience only: Access detector element by path
DetElement AlignmentsCalib::detector(const string& path)  const   {
  DetElement det(Geometry::DetectorTools::findElement(lcdd,path));
  return det;
}

/// Implementation: Add a new entry to the transaction stack.
pair<key_type,AlignmentsCalib::Entry*>
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
      destroyHandle(src_cond);
    }
    /// Now check again if we succeeded.
    if ( !src_cond.isValid() )   {
      except("AlignmentsCalib",
             "++ The SOURCE alignment condition [%016llX]: %s#%s is invalid.",
             key.hash, detector.path().c_str(), Keys::deltaName);
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
  return *(used.insert(make_pair(tar_key,entry.release())).first);
}

/// (1) Add a new entry to an existing DetElement structure.
key_type AlignmentsCalib::set(DetElement det, const Delta& delta)   {
  return _set(det.access(), delta).first;
}

/// (2) Add a new entry to an existing DetElement structure.
key_type AlignmentsCalib::set(const string& path, const Delta& delta)   {
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
void AlignmentsCalib::clear()   {
  for(auto& e : used)   {
    e.second->source.get<Delta>() = e.second->delta;
    delete e.second;
  }
  used.clear();
}

/// Commit all pending transactions. Returns number of altered entries
AlignmentsCalculator::Result AlignmentsCalib::commit()   {
  AlignmentsCalculator::Deltas deltas;
  AlignmentsCalculator         calculator;

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
