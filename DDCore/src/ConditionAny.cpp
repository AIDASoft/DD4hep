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
#include <DD4hep/ConditionAny.h>
#include <DD4hep/detail/ConditionsInterna.h>

// C/C++ include files
#include <iomanip>

using namespace dd4hep;

namespace {
  inline const BasicGrammar* any_grammar()   {
    static const BasicGrammar* s_any_grammar = &BasicGrammar::instance<std::any>();
    return s_any_grammar;
  }
}

/// Initializing constructor for a pure, undecorated conditions object
ConditionAny::ConditionAny(key_type hash_key) : Handle<Object>()
{
  if ( hash_key != 0 && hash_key != ~0x0ULL )  {
    Object* o = new Object();
    this->assign(o, "", "");
    o->data.bind<std::any>();
    o->hash = hash_key;
    return;
  }
  except("ConditionAny","+++ Cannot create a any-condition with an invalid key: %016llX",hash_key);
}

/// Initializing constructor for a pure, undecorated conditions object
ConditionAny::ConditionAny(const std::string& nam, const std::string& typ) : Handle<Object>()
{
  Object* o = new Object();
  this->assign(o, nam, typ);
  o->data.bind<std::any>();
  o->hash = 0;
}

void ConditionAny::use_data(detail::ConditionObject* obj)   {
  if ( obj )   {
    if ( !obj->data.grammar )   {
      except("ConditionAny",
	     "+++ Cannot assign unbound conditions data to handle. [Invalid operation]");
    }
    if ( obj->data.grammar != any_grammar() )   {
      except("ConditionAny",
	     "+++ Cannot assign data of type " +
	     obj->data.grammar->type_name() +
	     " to handle holding std::any. [Invalid operation]");
    }
  }
  this->m_element = obj;
}

/// Access the IOV type
const dd4hep::IOVType& ConditionAny::iovType() const   {
  return *(this->access()->iovType());
}

/// Access the IOV block
const dd4hep::IOV& ConditionAny::iov() const   {
  return *(this->access()->iovData());
}

/// Hash identifier
ConditionAny::key_type ConditionAny::key()  const    {
  return this->access()->hash;
}

/// DetElement part of the identifier
ConditionAny::detkey_type ConditionAny::detector_key()  const   {
  return ConditionKey::KeyMaker(this->access()->hash).values.det_key;
}

/// Item part of the identifier
ConditionAny::itemkey_type ConditionAny::item_key()  const   {
  return ConditionKey::KeyMaker(access()->hash).values.item_key;
}

/// Flag operations: Get flags
ConditionAny::mask_type ConditionAny::flags()  const    {
  return access()->flags;
}

/// Flag operations: Set a conditons flag
void ConditionAny::setFlag(mask_type option)   {
  access()->setFlag(option);
}

/// Flag operations: UN-Set a conditons flag
void ConditionAny::unFlag(mask_type option)   {
  access()->unFlag(option);
}

/// Flag operations: Test for a given a conditons flag
bool ConditionAny::testFlag(mask_type option) const {
  return access()->testFlag(option);
}

/// Generic getter. Specify the exact type, not a polymorph type
std::any& ConditionAny::get() {
  OpaqueData& o = this->access()->data;
  if ( o.grammar && (o.grammar == any_grammar()) )   {
    return *(std::any*)o.ptr();
  }
  throw std::bad_cast();
}

/// Generic getter (const version). Specify the exact type, not a polymorph type
const std::any& ConditionAny::get() const {
  const OpaqueData& o = this->access()->data;
  if ( o.grammar && (o.grammar == any_grammar()) )   {
    return *(std::any*)o.ptr();
  }
  throw std::bad_cast();
}
  
/// Checks whether the object contains a value
bool ConditionAny::has_value()   const   {
  return this->get().has_value();
}

/// Access to the type information
const std::type_info& ConditionAny::any_type() const   {
  const Object* o = this->ptr();
  if ( o && o->data.grammar && (o->data.grammar == any_grammar()) )   {
    const std::any* a = (const std::any*)o->data.ptr();
    return a->type();
  }
  return typeid(void);
}

/// Access to the type information as string
const std::string ConditionAny::any_type_name() const   {
  const Object* o = this->ptr();
  if ( o && o->data.grammar && (o->data.grammar == any_grammar()) )   {
    const std::any* a = (const std::any*)o->data.ptr();
    return typeName(a->type());
  }
  return typeName(typeid(void));
}
