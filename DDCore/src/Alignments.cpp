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
#include "DD4hep/AlignmentData.h"
#include "DD4hep/objects/AlignmentsInterna.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace DD4hep::Alignments;

/// Default constructor
Alignment::Alignment() : Handle<Object>() {
}

/// Initializing constructor to create a new object
Alignment::Alignment(const string& nam) {
  m_element = new NamedAlignmentObject(nam, "alignment");
}

/// Access the number of conditons keys available for this detector element
size_t Container::numKeys() const   {
  return access()->keys.size();
}

/// Access to alignment objects
Alignment Container::get(const string& alignment_key, const iov_type& iov)  {
  Object* o = ptr();
  if ( o )  {
    Alignment c = o->get(alignment_key, iov);
    if ( c.isValid() )  {
      return c;
    }
    invalidHandleError<Alignment>();
  }
  invalidHandleError<Container>();
  return Alignment();
}

/// Access to alignment objects
Alignment Container::get(key_type alignment_key, const iov_type& iov)  {
  Object* o = ptr();
  if ( o )  {
    Alignment c = o->get(alignment_key, iov);
    if ( c.isValid() )  {
      return c;
    }
    invalidHandleError<Alignment>();
  }
  invalidHandleError<Container>();
  return Alignment();
}

/// Access to alignment objects
Alignment Container::get(const string& alignment_key, const UserPool& pool)  {
  Object* o = ptr();
  if ( o )  {
    Alignment c = o->get(alignment_key, pool);
    if ( c.isValid() )  {
      return c;
    }
    invalidHandleError<Alignment>();
  }
  invalidHandleError<Container>();
  return Alignment();
}

/// Access to alignment objects
Alignment Container::get(key_type alignment_key, const UserPool& pool)  {
  Object* o = ptr();
  if ( o )  {
    Alignment c = o->get(alignment_key, pool);
    if ( c.isValid() )  {
      return c;
    }
    invalidHandleError<Alignment>();
  }
  invalidHandleError<Container>();
  return Alignment();
}
