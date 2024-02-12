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
#include <DD4hep/Printout.h>
#include <DD4hep/detail/ConditionsInterna.h>

// C/C++ include files
#include <climits>
#include <iomanip>
#include <cstdio>

using namespace dd4hep;

namespace {
  int s_have_inventory = 0;
  struct KeyTracer    {
    std::map<Condition::itemkey_type,std::string> item_names;
    std::mutex lock;
    void add(Condition::itemkey_type key,const std::string& item)   {
      if ( s_have_inventory > 0 )   {
        std::lock_guard<std::mutex> protect(lock);
        item_names.emplace(key, item);
      }
    }
    std::string get(Condition::itemkey_type key)    const    {
      auto i = item_names.find(key);
      if( i != item_names.end() )  {
        return (*i).second;
      }
      char text[32];
      std::snprintf(text,sizeof(text),"%08X",key);
      return text;
    }
  } s_key_tracer;
}

/// Setup conditions item name inventory for debugging
int dd4hep::detail::have_condition_item_inventory(int value)     {
  if ( value >= 0 )   {
    s_have_inventory = value;
  }
  return s_have_inventory;
}

std::string dd4hep::detail::get_condition_item_name(Condition::key_type key)    {
  return s_key_tracer.get(ConditionKey::KeyMaker(key).values.item_key);
}

std::string dd4hep::detail::get_condition_item_name(Condition::itemkey_type key)    {
  return s_key_tracer.get(key);
}

/// Initializing constructor for a pure, undecorated conditions object
Condition::Condition(key_type hash_key) : Handle<Object>()
{
  if ( hash_key != 0 && hash_key != ~0x0ULL )  {
    Object* o = new Object();
    assign(o,"","");
    o->hash = hash_key;
    return;
  }
  except("Condition","+++ Cannot create a condition with an invalid key: %016llX",hash_key);
}

/// Initializing constructor for a pure, undecorated conditions object
Condition::Condition(const std::string& nam, const std::string& typ) : Handle<Object>()
{
  Object* o = new Object();
  assign(o,nam,typ);
  o->hash = 0;
}

/// Initializing constructor for a pure, undecorated conditions object with payload buffer
Condition::Condition(const std::string& nam,const std::string& typ, size_t memory)
  : Handle<Object>()
{
  void* ptr = ::operator new(sizeof(Object)+memory);
  Object* o = new(ptr) Object();
  assign(o,nam,typ);
  o->hash = 0;
}

/// Output method
std::string Condition::str(int flags)  const   {
  std::stringstream output;
  Object* o = access(); 
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
  if ( 0 == (flags&NO_NAME) )
    output << std::setw(16) << std::left << o->name;
#endif
  if ( flags&WITH_IOV )  {
    const IOV* ptr_iov = o->iovData();
    output << " " << (ptr_iov ? ptr_iov->str().c_str() : "IOV:[UNKNOWN]");
  }
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  if ( flags&WITH_TYPE )
    output << " (" << o->type << ")";
#endif
  if ( flags&WITH_DATATYPE )
    output << " -> " << o->data.dataType();
  if ( flags&WITH_DATA )
    output << " Data:" << o->data.str();
#if !defined(DD4HEP_MINIMAL_CONDITIONS)
  if ( flags&WITH_ADDRESS )
    output << " " << o->address;
  if ( flags&WITH_COMMENT )
    output << " \"" << o->comment << "\"";
#endif
  return output.str();
}

/// Access the data type
int Condition::dataType() const   {
  return access()->data.type;
}

/// Access the IOV block
dd4hep::OpaqueDataBlock& Condition::data() const   {
  return access()->data;
}

/// Access the IOV type
const dd4hep::IOVType& Condition::iovType() const   {
  return *(access()->iovType());
}

/// Access the IOV block
const dd4hep::IOV& Condition::iov() const   {
  return *(access()->iovData());
}

#if !defined(DD4HEP_MINIMAL_CONDITIONS)
/// Access the type field of the condition
const std::string& Condition::type()  const   {
  return access()->type;
}

/// Access the value field of the condition as a string
const std::string& Condition::value()  const   {
  return access()->value;
}

/// Access the comment field of the condition
const std::string& Condition::comment()  const   {
  return access()->comment;
}

/// Access the address string [e.g. database identifier]
const std::string& Condition::address()  const   {
  return access()->address;
}
#endif

/// Access to the type information
const std::type_info& Condition::typeInfo() const   {
  return descriptor().type();
}

/// Hash identifier
Condition::key_type Condition::key()  const    {
  return access()->hash;
}

/// DetElement part of the identifier
Condition::detkey_type Condition::detector_key()  const   {
  return ConditionKey::KeyMaker(access()->hash).values.det_key;
}

/// Item part of the identifier
Condition::itemkey_type Condition::item_key()  const   {
  return ConditionKey::KeyMaker(access()->hash).values.item_key;
}

/// Flag operations: Get flags
Condition::mask_type Condition::flags()  const    {
  return access()->flags;
}

/// Flag operations: Set a conditons flag
void Condition::setFlag(mask_type option)   {
  access()->setFlag(option);
}

/// Flag operations: UN-Set a conditons flag
void Condition::unFlag(mask_type option)   {
  access()->unFlag(option);
}

/// Flag operations: Test for a given a conditons flag
bool Condition::testFlag(mask_type option) const {
  return access()->testFlag(option);
}

/// Access to the grammar type
const dd4hep::BasicGrammar& Condition::descriptor() const   {
  const BasicGrammar* grammar = access()->data.grammar;
  if ( !grammar ) {
    invalidHandleError<Condition>();
    // This code is never reached, since function above throws exception!
    // Needed to satisfay CppCheck
    throw std::runtime_error("Null pointer in Grammar object");
  }
  return *grammar;
}

/// Default destructor. 
ConditionsSelect::~ConditionsSelect()   {
}

/// Constructor from string
ConditionKey::KeyMaker::KeyMaker(DetElement detector, const std::string& value)   {
  KeyMaker key_maker(detector.key(), detail::hash32(value));
  hash = key_maker.hash;
  s_key_tracer.add(key_maker.values.item_key, value);
}

/// Constructor from detector element and item sub-key
ConditionKey::KeyMaker::KeyMaker(DetElement detector, Condition::itemkey_type item_key)  {
  hash = KeyMaker(detector.key(), item_key).hash;
}

/// Constructor from string
ConditionKey::KeyMaker::KeyMaker(Condition::detkey_type det_key, const std::string& value)   {
  KeyMaker key_maker(det_key, detail::hash32(value));
  hash = key_maker.hash;
  s_key_tracer.add(key_maker.values.item_key, value);
}

/// Constructor from string
ConditionKey::ConditionKey(DetElement detector, const std::string& value)  {
  KeyMaker key_maker(detector.key(), value);
  hash = key_maker.hash;
  s_key_tracer.add(key_maker.values.item_key, value);
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  name = detector.path()+"#"+value;
#endif
}

/// Constructor from detector element key and item sub-key
ConditionKey::ConditionKey(Condition::detkey_type det_key, const std::string& value)    {
  KeyMaker key_maker(det_key, value);
  s_key_tracer.add(key_maker.values.item_key, value);
  hash = key_maker.hash;
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  char text[32];
  ::snprintf(text,sizeof(text),"%08X#",det_key);
  name = text+value;
#endif
}

/// Constructor from detector element key and item sub-key
ConditionKey::ConditionKey(DetElement detector, Condition::itemkey_type item_key)  {
  hash = KeyMaker(detector.key(),item_key).hash;
#ifdef DD4HEP_CONDITIONS_HAVE_NAME
  char text[32];
  ::snprintf(text,sizeof(text),"#%08X",item_key);
  name = detector.path()+text;
#endif
}

/// Hash code generation from input string
Condition::key_type ConditionKey::hashCode(DetElement detector, const char* value)  {
  KeyMaker key_maker(detector.key(), value);
  s_key_tracer.add(key_maker.values.item_key, value);
  return key_maker.hash;
}

/// Hash code generation from input string
Condition::key_type ConditionKey::hashCode(DetElement detector, const std::string& value)  {
  KeyMaker key_maker(detector.key(), value);
  s_key_tracer.add(key_maker.values.item_key, value);
  return key_maker.hash;
}

/// 32 bit hashcode of the item
Condition::itemkey_type ConditionKey::itemCode(const char* value)  {
  Condition::itemkey_type code = detail::hash32(value);
  s_key_tracer.add(code, value);
  return code;
}

/// 32 bit hashcode of the item
Condition::itemkey_type ConditionKey::itemCode(const std::string& value)   {
  Condition::itemkey_type code = detail::hash32(value);
  s_key_tracer.add(code, value);
  return code;
}

/// Conversion to string
std::string ConditionKey::toString()  const    {
  dd4hep::ConditionKey::KeyMaker key(hash);
  char text[64];
  ::snprintf(text,sizeof(text),"%08X-%08X",key.values.det_key, key.values.item_key);
#if defined(DD4HEP_CONDITIONS_HAVE_NAME)
  if ( !name.empty() )   {
    std::stringstream str;
    str << "(" << name << ") " << text;
    return str.str();
  }
#endif
  return text;
}

