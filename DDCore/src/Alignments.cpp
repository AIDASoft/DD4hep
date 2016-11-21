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
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace DD4hep::Alignments;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  
  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

#if defined(__APPLE__) || defined(__clang__)
    /// Initializing constructor to create a new object (Specialized for AlignmentNamedObject)
    Alignment::Alignment(const string& nam)
    { assign(new Interna::AlignmentNamedObject(nam, "alignment"), nam, "alignment");    }

    /// Initializing constructor to create a new object (Specialized for AlignmentConditionObject)
    AlignmentCondition::AlignmentCondition(const string& nam)
    { assign(new AlignmentCondition::Object(nam, "alignment"), nam, "alignment");       }
#else
    /// Initializing constructor to create a new object (Specialized for AlignmentData)
    template <> Alignment::Alignment<AlignmentData>(const string& nam)
    { assign(new AlignmentData(), nam, "alignment");                                    }

    /// Initializing constructor to create a new object (Specialized for AlignmentNamedObject)
    template <> Alignment::Alignment<Interna::AlignmentNamedObject>(const string& nam)
    { assign(new Interna::AlignmentNamedObject(nam, "alignment"), nam, "alignment");    }

    /// Initializing constructor to create a new object (Specialized for AlignmentConditionObject)
    template <> AlignmentCondition::AlignmentCondition<Interna::AlignmentConditionObject>(const string& nam)
    { assign(new Object(nam, "alignment"), nam, "alignment");                           }
#endif
  } /* End namespace Aligments                    */
}   /* End namespace DD4hep                       */


/// Default constructor
Alignment::Processor::Processor() {
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& Alignment::worldTransformation()  const  {
  return data().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
const TGeoHMatrix& Alignment::detectorTransformation() const   {
  return data().detectorTransformation();
}

/// Access the IOV type
const DD4hep::IOVType& AlignmentCondition::iovType() const   {
  return *(access()->iovType());
}

/// Access the IOV block
const DD4hep::IOV& AlignmentCondition::iov() const   {
  return *(access()->iovData());
}

/// Data accessor for the use of decorators
AlignmentCondition::Data& AlignmentCondition::data()              {
  Object* o = access();
  if ( o->alignment_data )
    return *(o->alignment_data);
  Conditions::Condition c(*this);
  o->alignment_data = c.is_bound() ? &c.get<Data>() : &c.bind<Data>();
  o->alignment_data->condition = c;
  return *(o->alignment_data);
}

/// Data accessor for the use of decorators
const AlignmentCondition::Data& AlignmentCondition::data() const  {
  Object* o = access();
  if ( o->alignment_data )
    return *(o->alignment_data);
  Conditions::Condition c(*this);
  o->alignment_data = c.is_bound() ? &c.get<Data>() : &c.bind<Data>();
  o->alignment_data->condition = c;
  return *(o->alignment_data);
}

/// Check if object is already bound....
bool AlignmentCondition::is_bound()  const  {
  return isValid() ? ptr()->data.is_bound() : false;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& AlignmentCondition::worldTransformation()  const  {
  return data().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
const TGeoHMatrix& AlignmentCondition::detectorTransformation() const   {
  return data().detectorTransformation();
}

/// Default constructor
Container::Processor::Processor() {
}

/// Access the number of conditons keys available for this detector element
size_t Container::numKeys() const   {
  return access()->keys.size();
}

/// Known keys of conditions in this container
const Container::Keys& Container::keys()  const   {
  Object* o = ptr();
  if ( !o )   {
    invalidHandleError<Container>();
  }
  return o->keys;
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
