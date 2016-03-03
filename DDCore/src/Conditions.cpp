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
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep::Conditions;

std::string IOVType::str()  const   {
  char text[256];
  ::snprintf(text,sizeof(text),"%s(%d)",name.c_str(),int(type));
  return text;
}

/// Initializing constructor: Does not set reference to IOVType !
IOV::IOV() : iovType(0), keyData(0,0), optData(0)  {
  type = int(IOVType::UNKNOWN_IOV);
}

/// Initializing constructor
IOV::IOV(const IOVType* typ) : iovType(typ), keyData(0,0), optData(0)  {
  type = typ ? typ->type : int(IOVType::UNKNOWN_IOV);
}

/// Standard Destructor
IOV::~IOV()  {
}

/// Move the data content: 'from' will be reset to NULL
void IOV::move(IOV& from)   {
  ::memcpy(this,&from,sizeof(IOV));
  from.keyData.first = from.keyData.second = from.optData = 0;
  from.type = int(IOVType::UNKNOWN_IOV);
  from.iovType = 0;
}

/// Create string representation of the IOV
string IOV::str()  const  {
  char text[256];
  if ( iovType )  {
    ::snprintf(text,sizeof(text),"%s(%d):[%d-%d]",
               iovType->name.c_str(),int(iovType->type),keyData.first, keyData.second);
  }
  else  {
    ::snprintf(text,sizeof(text),"%d:[%d-%d]",type,keyData.first, keyData.second);
  }
  return text;
}

/// Check for validity containment
bool IOV::contains(const IOV& iov)  const   {
  if ( key_is_contained(iov.keyData,keyData) )  {
    unsigned int typ1 = iov.iovType ? iov.iovType->type : iov.type;
    unsigned int typ2 = iovType ? iovType->type : type;
    return typ1 == typ2;
  }
  return false;
}

/// Standard initializing constructor
Block::Block() : grammar(0), pointer(0)   {
}

/// Standard Destructor
Block::~Block()   {
}

/// Create data block from string representation
void Block::fromString(const string& rep)   {
  if ( pointer && grammar )  {
    grammar->fromString(pointer,rep);
    return;
  }
  throw runtime_error("Conditions data block is unbound. Cannot parse string representation.");
}

/// Create string representation of the data block
string Block::str()   {
  if ( pointer && grammar )  {
    return grammar->str(pointer);
  }
  throw runtime_error("Conditions data block is unbound. Cannot create string representation.");
}

/// Initializing constructor
Condition::Condition(const string& nam,const string& typ) : Handle<Object>()  {
  Object* o = new Object();
  assign(o,nam,typ);
  o->hash = hash32(nam);
}

/// Assignment operator
Condition& Condition::operator=(const Condition& c)   {
  if ( this != &c ) this->m_element = c.m_element;
  return *this;
}

/// Access the data type
int Condition::dataType() const   {
  return access()->data.type;
}

/// Access the IOV block
Block& Condition::block() const   {
  return access()->data;
}

/// Access the IOV type
const IOVType& Condition::iovType() const   {
  return *(access()->iov_type());
}

/// Access the IOV block
const IOV& Condition::iov() const   {
  return *(access()->iov_data());
}

/// Access the name of the condition
const string& Condition::name()  const   {
  return access()->name;
}

/// Access the type field of the condition
const string& Condition::type()  const   {
  return access()->type;
}

/// Access the validity field of the condition as a string
const string& Condition::validity()  const   {
  return access()->validity;
}

/// Access the value field of the condition as a string
const string& Condition::value()  const   {
  return access()->value;
}

/// Access the comment field of the condition
const string& Condition::comment()  const   {
  return access()->comment;
}

/// Access the address string [e.g. database identifier]
const string& Condition::address()  const   {
  return access()->address;
}

/// Access the hosting detector element
DetElement Condition::detector() const   {
  return access()->detector;
}

/// Access to the type information
const type_info& Condition::typeInfo() const   {
  return descriptor().type();
}

/// Access to the grammar type
const DD4hep::BasicGrammar& Condition::descriptor() const   {
  const BasicGrammar* g = access()->data.grammar;
  if ( !g ) {
    invalidHandleError<Condition>();
    // This code is never reached, since function above throws exception!
    // Needed to satisfay CppCheck
    throw runtime_error("Null pointer in Grammar object");
  }
  return *g;
}

/// Re-evaluate the conditions data according to the previous bound type definition
Condition& Condition::rebind()    {
  Object* o = access();
#if 0
  const IOV* i = o->iov;
  const IOVType* t = i->iovType;
  i->fromString(o->validity);
  if ( t != i->iovType )  {
    except("Condition","Rebinding condition with different IOV types [%d <> %d] is not allowed!",
           t ? t->type : -1, i->iovType ? i->iovType->type : -1);
  }
#endif
  o->data.fromString(o->value);
  printout(INFO,"Condition","+++ condition:%s : %s rebinding value:%s",
           detector().path().c_str(), name().c_str(), o->value.c_str());
  return *this;
}

/// Clear all conditions. Auto-delete of all existing entries
void Container::removeElements() const   {
  if ( !isValid() )   {
    invalidHandleError<Container>();
  }
  data<Object>()->removeElements();
}

/// Access the number of conditons available for this detector element
size_t Container::count() const   {
  if ( !isValid() )   {
    invalidHandleError<Container>();
  }
  return data<Object>()->size();
}

/// Access the full map of conditons
Container::Entries& Container::entries() const  {
  if ( !isValid() )   {
    invalidHandleError<Container>();
  }
  return data<Object>()->elements();
}

/// Access to condition objects
Condition Container::operator[](const std::string& key)  {
  return this->operator[](hash32(key));
}

/// Access to condition objects
Condition Container::operator[](int condition_hash)  {
  Object* c = ptr();
  if ( !c )   {
    invalidHandleError<Container>();
  }
  Entries::iterator i=c->elements().find(condition_hash);
  if ( i != c->elements().end() ) return (*i).second;
  except("ConditionContainer",
         "No condition with key 0x%08X present!",condition_hash);
  return Condition();
}
