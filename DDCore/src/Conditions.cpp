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
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <climits>
#include <iomanip>

using namespace std;
using namespace DD4hep::Conditions;

/// Default constructor
Condition::Processor::Processor() {
}

/// Initializing constructor
Condition::Condition(const string& nam,const string& typ) : Handle<Object>()  {
  Object* o = new Object();
  assign(o,nam,typ);
  o->hash = ConditionKey::hashCode(nam);
}

/// Output method
string Condition::str(int flags)  const   {
  stringstream output;
  Object* o = access();
  const IOV* ptr_iov = o->iovData();
  if ( 0 == (flags&NO_NAME) )
    output << setw(16) << left << o->name;
  if ( flags&WITH_IOV )
    output << " " << (ptr_iov ? ptr_iov->str().c_str() : "IOV:[UNKNOWN]");
  if ( flags&WITH_TYPE )
    output << " (" << o->type << ")";
  if ( flags&WITH_ADDRESS )
    output << " " << o->address;
  if ( flags&WITH_DATATYPE )
    output << " -> " << o->data.dataType();
  if ( flags&WITH_DATA )
    output << " Data:" << o->data.str();
  if ( flags&WITH_COMMENT )
    output << " \"" << o->comment << "\"";
  return output.str();
}

/// Access the data type
int Condition::dataType() const   {
  return access()->data.type;
}

/// Access the IOV block
DD4hep::OpaqueData& Condition::data() const   {
  return access()->data;
}

/// Access the IOV type
const DD4hep::IOVType& Condition::iovType() const   {
  return *(access()->iovType());
}

/// Access the IOV block
const DD4hep::IOV& Condition::iov() const   {
  return *(access()->iovData());
}

/// Access the type field of the condition
const string& Condition::type()  const   {
  return access()->type;
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

/// Access to the type information
const type_info& Condition::typeInfo() const   {
  return descriptor().type();
}

/// Hash identifier
Condition::key_type Condition::key()  const    {
  return access()->hash;
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
  printout(INFO,"Condition","+++ condition:%s rebinding value:%s",
           name(), o->value.c_str());
  return *this;
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
  return access()->keys;
}

/// Insert a new key to the conditions access map. Ignores already existing keys.
bool Container::insertKey(const std::string& key_val)  {
  return access()->insertKey(key_val);
}

/// Insert a new key to the conditions access map: Allow for alias if key_val != data_val
bool Container::insertKey(const std::string& key_val, const std::string& data_val)  {
  return access()->insertKey(key_val, data_val);
}

/// Add a new key to the conditions access map
void Container::addKey(const string& key_val)  {
  access()->addKey(key_val);
}

/// Add a new key to the conditions access map: Allow for alias if key_val != data_val
void Container::addKey(const string& key_val, const string& data_val)  {
  access()->addKey(key_val, data_val);
}

/// Access to condition objects
Condition Container::get(const string& condition_key, const iov_type& iov)  {
  return Condition(access()->get(condition_key, iov).access());
}

/// Access to condition objects
Condition Container::get(key_type condition_key, const iov_type& iov)  {
  return Condition(access()->get(condition_key, iov).access());
}

/// Access to condition objects
Condition Container::get(const string& condition_key, const UserPool& pool)  {
  return Condition(access()->get(condition_key, pool).access());
}

/// Access to condition objects
Condition Container::get(key_type condition_key, const UserPool& pool)  {
  return Condition(access()->get(condition_key, pool).access());
}

/// Default destructor. 
ConditionsSelect::~ConditionsSelect()   {
}

/// Access the key of the condition
ConditionKey DD4hep::Conditions::make_key(Condition c) {
  Condition::Object* p = c.access();
  return ConditionKey(p->name,p->hash);
}

/// Constructor from string
ConditionKey::ConditionKey(const string& value) 
  : name(value), hash(hashCode(value))
{
}

/// Assignment operator from the string representation
ConditionKey& ConditionKey::operator=(const string& value)  {
  ConditionKey key(value);
  hash = hashCode(value);
  name = value;
  return *this;
}

/// Operator less (for map insertions) using the string representation
bool ConditionKey::operator<(const string& compare)  const  {  
  return hash < hashCode(compare);
}
