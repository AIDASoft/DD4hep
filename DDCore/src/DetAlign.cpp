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
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DD4hep/DetAlign.h"
#include "DD4hep/World.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::Alignments;


/// Access to the alignment information
Alignment DetAlign::nominal() const   {
  return access()->nominal;
}

/// Access to the survey alignment information
Alignment DetAlign::survey() const  {
  return access()->survey;
}

/// Check if alignments are at all present
bool DetAlign::hasAlignments()  const   {
  Object* o = access();
  if ( o->alignments.isValid() && !o->alignments->keys.empty() ) 
    return true;
  return false;
}

/// Access to the alignments information
Container DetAlign::alignments() const  {
  Object* o = access();
  if ( o->alignments.isValid() ) return o->alignments;
  o->alignments.assign(new Container::Object(o),"alignments","");
  return o->alignments;
}

/// Access to alignment objects.
Alignment DetAlign::get(const std::string& key, const Alignment::iov_type& iov)   {
  return alignments().get(key, iov);
}

/// Access to alignment objects.
Alignment DetAlign::get(Alignment::key_type key, const Alignment::iov_type& iov)   {
  return alignments().get(key, iov);
}

/// Access to alignment objects from a given pool. Only alignments in the pool are accessed.
Alignment DetAlign::get(const std::string& key,  const UserPool& pool)   {
  return alignments().get(key, pool);
}

/// Access to alignment objects from a given pool. Only alignments in the pool are accessed.
Alignment DetAlign::get(Alignment::key_type key, const UserPool& pool)   {
  return alignments().get(key, pool);
}

