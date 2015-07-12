// $Id: $
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
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DD4hep::Geometry::ConditionsInterna;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsInterna::ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsInterna::ConditionContainer);

/// Initializing constructor
IOV::IOV(int t) : type(t)  {
  data[0]=data[1]=data[2]=data[3]=0;
}

/// Standard Destructor
IOV::~IOV()  {
}

/// Move the data content: 'from' will be reset to NULL
void IOV::move(IOV& from)   {
  ::memcpy(this,&from,sizeof(IOV));
  from.data[0]=from.data[1]=from.data[2]=from.data[3]=0;
  from.type = UNKNOWN_IOV;
}

/// Create IOV data from string representation
void IOV::fromString(const std::string& rep)   {
  notImplemented("void IOV::fromString(const std::string& rep): "+rep);
}

/// Create string representation of the IOV
std::string IOV::str()  {
  notImplemented("void IOV::fromString(const std::string& rep)");
  return "";
}

/// Standard initializing constructor
BlockData::BlockData() : Block(), destruct(0), copy(0), type(0)   {
}

/// Standard Destructor
BlockData::~BlockData()   {
  if ( destruct )  {
    (*destruct)(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
}

/// Move the data content: 'from' will be reset to NULL
void BlockData::move(BlockData& from)   {
  pointer = from.pointer;
  grammar = from.grammar;
  ::memcpy(data,from.data,sizeof(data));
  destruct = from.destruct;
  copy = from.copy;
  type = from.type;
  ::memset(from.data,0,sizeof(data));
  from.type = PLAIN_DATA;
  from.destruct = 0;
  from.copy = 0;
  from.pointer = 0;
  from.grammar = 0;
}

/// Set data value
void BlockData::bind(const BasicGrammar* g, void (*ctor)(void*,const void*), void (*dtor)(void*))   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar  = g;
    destruct = dtor;
    copy     = ctor;
    (len > sizeof(data))
      ? (pointer=::operator new(len),type=ALLOC_DATA)
      : (pointer=data,type=PLAIN_DATA);
    return;
  }
  else if ( grammar == g )  {
    return;
  }
  typeinfoCheck(grammar->type(),g->type(),"Conditions data blocks may not be assigned.");
}

/// Set data value
void BlockData::assign(const void* ptr, const type_info& typ)  {
  if ( !grammar )   {
    throw runtime_error("Conditions data block is unbound. Cannot copy data.");
  }
  else if ( grammar->type() != typ )  {
    throw runtime_error("bad binding");
  }
  (*copy)(pointer,ptr);
}

/// Standard constructor
ConditionObject::ConditionObject() : NamedObject(), detector(), data(), iov()   {
  InstanceCount::increment(this);
}

/// Standard Destructor
ConditionObject::~ConditionObject()  {
  InstanceCount::decrement(this);
}

/// Assignment operator
ConditionObject& ConditionObject::move(ConditionObject& c)   {
  if ( this != &c )  {
    name = c.name;
    type = c.type;
    address = c.address;
    comment = c.comment;
    detector = c.detector;
    data.move(c.data);
    iov.move(c.iov);
  }
  return *this;
}

/// Initializing constructor
Entry::Entry(const DetElement& det, const string& nam,
             const string& typ, const string& valid)
  : NamedObject(nam), detector(det), type(typ), value(), validity(valid)
{
  InstanceCount::increment(this);
}

/// Copy constructor
Entry::Entry(const Entry& c)
  : NamedObject(c), detector(c.detector), type(c.type), value(c.value), validity(c.validity)
{
  InstanceCount::increment(this);
}

/// Default destructor
Entry::~Entry()   {
  InstanceCount::decrement(this);
}

/// Assignment operator
Entry& Entry::operator=(const Entry& c)   {
  if ( this != &c )  {
    this->NamedObject::operator=(c);
    detector = c.detector;
    type = c.type;
    value = c.value;
    validity = c.validity;
  }
  return *this;
}

/// Standard constructor
ConditionContainer::ConditionContainer() : NamedObject() {
  InstanceCount::increment(this);
}

/// Default destructor
ConditionContainer::~ConditionContainer() {
  removeElements();
  InstanceCount::decrement(this);
}

/// Clear all conditions. Auto-delete of all existing entries
void ConditionContainer::removeElements()    {
  for(Entries::iterator i=entries.begin(); i != entries.end(); ++i)
    delete (*i).second.ptr();
  entries.clear();
}

