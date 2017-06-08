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
#include "DD4hep/IOV.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionsListener.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

using namespace std;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(Interna::ConditionObject);

/// Default constructor
Interna::ConditionObject::ConditionObject()
  : NamedObject(), value(), validity(), address(), comment(), data()
{
  InstanceCount::increment(this);
}

/// Standard constructor
Interna::ConditionObject::ConditionObject(const string& nam,const string& tit)
  : NamedObject(nam, tit), value(), validity(), address(), comment(), data()
{
  InstanceCount::increment(this);
}

/// Standard Destructor
Interna::ConditionObject::~ConditionObject()  {
  InstanceCount::decrement(this);
}

/// Data offset from the opaque data block pointer to the condition
size_t Interna::ConditionObject::offset()   {
  union  _P {
    const char* c; const void* v; const ConditionObject* o;
    _P(const void* val) { v = val; }
  };
  static _P p((void*)0x1000), q(&p.o->data.grammar);
  static size_t off = q.c - p.c + sizeof(p.o->data.grammar);
  return off;
}

/// Access the bound data payload. Exception id object is unbound
void* Interna::ConditionObject::payload() const   {
  void** p = (void**)(((char*)this)+offset());
  return *p;
}

/// Move data content: 'from' will be reset to NULL
Interna::ConditionObject& Interna::ConditionObject::move(ConditionObject& /* from */)   {
  return *this;
}

/// Access safely the IOV
const Condition::iov_type* Interna::ConditionObject::iovData() const    {
  if ( iov ) return iov;
  invalidHandleError<IOV>();
  return 0;
}

/// Access safely the IOV-type
const DD4hep::IOVType* Interna::ConditionObject::iovType() const    {
  if ( iov && iov->iovType ) return iov->iovType;
  invalidHandleError<IOVType>();
  return 0;
}
