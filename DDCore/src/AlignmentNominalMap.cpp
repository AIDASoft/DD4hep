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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignmentsNominalMap.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace dd4hep;
using align::Keys;

/// Standard constructor
AlignmentsNominalMap::AlignmentsNominalMap(DetElement wrld) : world(wrld) {
}

/// Insert a new entry to the map
bool AlignmentsNominalMap::insert(DetElement              detector,
                                  Condition::itemkey_type key,
                                  Condition               condition)   {
  auto res = data.emplace(ConditionKey(detector,key).hash,condition);
  return res.second;
}

/// Interface to access conditions by hash value
Condition AlignmentsNominalMap::get(DetElement detector, Condition::itemkey_type key) const   {
  auto res = data.find(ConditionKey(detector,key).hash);
  if ( res == data.end() )  {
    if ( key == Keys::alignmentKey )  {
      return Condition(detector.nominal().ptr());
    }
    return Condition();
  }
  return res->second;
}

/// Interface to scan data content of the conditions mapping
void AlignmentsNominalMap::scan(const Condition::Processor& processor) const  {

  /// Helper to implement partial scans.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  struct Scanner  {
    const Condition::Processor& proc;
    /// Constructor
    Scanner(const Condition::Processor& p) : proc(p)  { }
    /// Conditions callback for object processing
    int operator()(DetElement de, int /* level */)  const  {
      Condition c = de.nominal();
      return proc(c);
    }
  } scanner(processor);

  // First scan the local conditions
  for( const auto& i : data )
    processor(i);

  // We emulate here a full detector scan, access the nominal alignments and process them by the processor.
  if ( world.isValid() )   {
    DetectorScanner().scan(scanner,world,0,true);
    return;
  }
  dd4hep::except("AlignmentsNominalMap",
                 "Cannot scan conditions map for conditions of an invalid top level detector element!");
}

/// Interface to partially scan data content of the conditions mapping
void AlignmentsNominalMap::scan(DetElement   detector,
                                Condition::itemkey_type lower,
                                Condition::itemkey_type upper,
                                const Condition::Processor&   processor) const   {

  if ( detector.isValid() )   {
    Condition::detkey_type det_key = detector.key();
    Condition::key_type low = ConditionKey::KeyMaker(det_key,lower).hash;
    Condition::key_type up  = ConditionKey::KeyMaker(det_key,upper).hash;
    ConditionKey::KeyMaker align_key(detector.key(),Keys::alignmentKey);

    for(auto i=data.lower_bound(low); i != data.end() && (*i).first <= up; ++i)  {
      ConditionKey::KeyMaker k((*i).first);
      if ( low <= k.hash && up >= k.hash )   {
        processor((*i).second);
        if ( k.hash == align_key.hash )   {
          align_key.hash = 0;
        }
      }
    }
    if ( align_key.hash )  {
      if ( lower <= Keys::alignmentKey && upper >= Keys::alignmentKey )   {
        Condition c(detector.nominal().ptr());
        processor(c);
      }
    }
    return;
  }
  dd4hep::except("AlignmentsNominalMap",
                 "Cannot scan conditions map for conditions of an invalid detector element!");
}
