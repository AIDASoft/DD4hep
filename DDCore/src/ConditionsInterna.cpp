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
#include "DD4hep/IOV.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace std;
using namespace dd4hep;

#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
DD4HEP_INSTANTIATE_HANDLE_NAMED(detail::ConditionObject);
#else
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(detail::ConditionObject);
#endif

namespace {
  /// Simple cast union to perform pointer arithmetic on raw byte based memory
  union  _P {
    const char* character;
    const void* p_void;
    void**      pp_void;
    const detail::ConditionObject* o;
    _P(const void* val) { p_void = val; }
  };
}

/// Default constructor
detail::ConditionObject::ConditionObject()
{
  InstanceCount::increment(this);
}

/// Standard constructor
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
detail::ConditionObject::ConditionObject(const string& nam,const string& tit)
  : NamedObject(nam, tit), data()
#else
detail::ConditionObject::ConditionObject(const string& ,const string& )
  : data()
#endif
{
  InstanceCount::increment(this);
}

/// Standard Destructor
detail::ConditionObject::~ConditionObject()  {
  InstanceCount::decrement(this);
}

/// Release object (Used by persistency mechanism)
void detail::ConditionObject::release()  {
  if ( --refCount <= 0 ) delete this;
}

/// Data offset from the opaque data block pointer to the condition
size_t detail::ConditionObject::offset()   {
  static _P p((void*)0x1000);
  static size_t off = _P(&p.o->data.grammar).character - p.character + sizeof(OpaqueData::grammar);
  return off;
}

/// Access the bound data payload. Exception id object is unbound
void* detail::ConditionObject::payload() const   {
  return *(_P(_P(this).character+offset()).pp_void);
}

/// Move data content: 'from' will be reset to NULL
detail::ConditionObject& detail::ConditionObject::move(ConditionObject& /* from */)   {
  return *this;
}

/// Access safely the IOV
const dd4hep::IOV* detail::ConditionObject::iovData() const    {
  if ( iov ) return iov;
  invalidHandleError<IOV>();
  return 0;
}

/// Access safely the IOV-type
const dd4hep::IOVType* detail::ConditionObject::iovType() const    {
  if ( iov && iov->iovType ) return iov->iovType;
  invalidHandleError<IOVType>();
  return 0;
}
