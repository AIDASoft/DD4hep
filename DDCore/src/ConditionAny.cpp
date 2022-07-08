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
#include "DD4hep/Printout.h"
#include "DD4hep/ConditionAny.h"
#include "DD4hep/detail/ConditionsInterna.h"

// C/C++ include files
#include <climits>
#include <iomanip>
#include <cstdio>

using namespace std;
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
    this->assign(o,"","");
    o->data.bind<any>();
    o->hash = hash_key;
    return;
  }
  except("ConditionAny","+++ Cannot create a any-condition with an invalid key: %016llX",hash_key);
}

/// Initializing constructor for a pure, undecorated conditions object
ConditionAny::ConditionAny(const string& nam, const string& typ) : Handle<Object>()
{
  Object* o = new Object();
  this->assign(o,nam,typ);
  o->data.bind<any>();
  o->hash = 0;
}

void ConditionAny::use_data(detail::ConditionObject* obj)   {
  if ( obj )   {
    if ( !obj->data.grammar )   {
      except("ConditionAny","+++ Cannot assign unbound conditions data to handle. [Invalid operation]");
    }
    if ( obj->data.grammar != any_grammar() )   {
      except("ConditionAny",
	     "+++ Cannot assign data of type " +
	     obj->data.grammar->type_name() +
	     " to std::any. [Invalid operation]");
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

/// Generic getter. Specify the exact type, not a polymorph type
std::any& ConditionAny::get() {
  return this->access()->data.get<std::any>();
}

/// Generic getter (const version). Specify the exact type, not a polymorph type
const std::any& ConditionAny::get() const {
  return this->access()->data.get<std::any>();
}
  
/// Checks whether the object contains a value
bool ConditionAny::has_value()   const   {
  return this->get().has_value();
}

/// Access to the type information
const type_info& ConditionAny::any_type() const   {
  return this->get().type();
}

#if 0
/// Access to the grammar type
const dd4hep::BasicGrammar& ConditionAny::descriptor() const   {
  const BasicGrammar* grammar = access()->data.grammar;
  if ( !grammar ) {
    invalidHandleError<ConditionAny>();
    // This code is never reached, since function above throws exception!
    // Needed to satisfay CppCheck
    throw runtime_error("Null pointer in Grammar object");
  }
  return *grammar;
}
#endif
