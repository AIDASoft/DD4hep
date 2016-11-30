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
#include "DD4hep/AlignmentsKeyAssign.h"
#include "DD4hep/objects/AlignmentsInterna.h"
#include "DD4hep/DetAlign.h"

using std::string;
using namespace DD4hep::Alignments;

/// Add a new key to the conditions access map
const AlignmentsKeyAssign&
AlignmentsKeyAssign::addKey(const string& key_value)  const  {
  DetAlign conds(detector);
  conds.alignments().addKey(key_value);
  return *this;
}

/// Add a new key to the conditions access map: Allow for alias if key_val != data_val
const AlignmentsKeyAssign&
AlignmentsKeyAssign::addKey(const string& key_value, const string& data_value)  const  {
  DetAlign conds(detector);
  conds.alignments().addKey(key_value, data_value);
  return *this;
}

 
