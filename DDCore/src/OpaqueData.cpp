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
#include "DD4hep/Primitives.h"
#include "DD4hep/OpaqueData.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/OpaqueData_inl.h"

// C/C++ header files
#include <cstring>

using namespace std;
using namespace dd4hep;

/// Create data block from string representation
bool OpaqueData::fromString(const string& rep)   {
  if ( pointer && grammar )  {
    return grammar->fromString(pointer,rep);
  }
  throw runtime_error("Opaque data block is unbound. Cannot parse string representation.");
}

/// Create string representation of the data block
string OpaqueData::str()  const  {
  if ( pointer && grammar )  {
    return grammar->str(pointer);
  }
  throw runtime_error("Opaque data block is unbound. Cannot create string representation.");
}

/// Access type id of the condition
const type_info& OpaqueData::typeInfo() const  {
  if ( pointer && grammar ) {
    return grammar->type();
  }
  throw runtime_error("Opaque data block is unbound. Cannot determine type information!");
}

/// Access type name of the condition data block
const string& OpaqueData::dataType() const   {
  if ( pointer && grammar ) {
    return grammar->type_name();
  }
  throw runtime_error("Opaque data block is unbound. Cannot determine type information!"); 
}

/// Standard initializing constructor
OpaqueDataBlock::OpaqueDataBlock() : OpaqueData(), /*destruct(0), copy(0),*/ type(0)   {
  InstanceCount::increment(this);
}

/// Copy constructor
OpaqueDataBlock::OpaqueDataBlock(const OpaqueDataBlock& c) 
  : OpaqueData(c), type(c.type)   {
  grammar = 0;
  pointer = 0;
  this->bind(c.grammar);
  this->grammar->copy(pointer,c.pointer);
  InstanceCount::increment(this);
}

/// Standard Destructor
OpaqueDataBlock::~OpaqueDataBlock()   {
  if ( pointer )  {
    grammar->destruct(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
  InstanceCount::decrement(this);
}

/// Move the data content: 'from' will be reset to NULL
bool OpaqueDataBlock::move(OpaqueDataBlock& from)   {
  pointer = from.pointer;
  grammar = from.grammar;
  ::memcpy(data,from.data,sizeof(data));
  type = from.type;
  ::memset(from.data,0,sizeof(data));
  from.type = PLAIN_DATA;
  from.pointer = 0;
  from.grammar = 0;
  return true;
}

/// Copy constructor
OpaqueDataBlock& OpaqueDataBlock::operator=(const OpaqueDataBlock& c)   {
  if ( this != &c )  {
    if ( this->grammar == c.grammar )   {
      if ( pointer )  {
        grammar->destruct(pointer);
        if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
      }
      pointer = 0;
      grammar = 0;
    }
    if ( this->grammar == 0 )  {
      this->OpaqueData::operator=(c);
      this->type = c.type;
      this->grammar = 0;
      this->bind(c.grammar);
      this->grammar->copy(pointer,c.pointer);
      return *this;
    }
    except("OpaqueData","You may not bind opaque data multiple times!");
  }
  return *this;
}

/// Bind data value
void* OpaqueDataBlock::bind(const BasicGrammar* g)   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar  = g;
    (len > sizeof(data))
      ? (pointer=::operator new(len),type=ALLOC_DATA)
      : (pointer=data,type=PLAIN_DATA);
    return pointer;
  }
  else if ( grammar == g )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("OpaqueData","You may not bind opaque multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Opaque data blocks may not be assigned.");
  return 0;
}

/// Set data value
void* OpaqueDataBlock::bind(void* ptr, size_t size, const BasicGrammar* g)   {
  if ( !grammar )  {
    size_t len = g->sizeOf();
    grammar = g;
    if ( len <= size )
      pointer=ptr, type=STACK_DATA;
    else if ( len <= sizeof(data) )
      pointer=data, type=PLAIN_DATA;
    else 
      pointer=::operator new(len),type=ALLOC_DATA;
    return pointer;
  }
  else if ( grammar == g )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("OpaqueData","You may not bind opaque multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Opaque data blocks may not be assigned.");
  return 0;
}

/// Set data value
void OpaqueDataBlock::assign(const void* ptr, const type_info& typ)  {
  if ( !grammar )   {
    except("OpaqueData","Opaque data block is unbound. Cannot copy data.");
  }
  else if ( grammar->type() != typ )  {
    except("OpaqueData","Bad data binding binding");
  }
  grammar->copy(pointer,ptr);
}

/// print Conditions object
std::ostream& operator << (std::ostream& s, const OpaqueDataBlock& data)   {
  s << data.str();
  return s;
}

#include "Parsers/Parsers.h"
DD4HEP_DEFINE_PARSER_DUMMY(OpaqueDataBlock)

#include "DD4hep/detail/BasicGrammar_inl.h"
#include "DD4hep/detail/ConditionsInterna.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(OpaqueDataBlock,eval_none<OpaqueDataBlock>)
DD4HEP_DEFINE_CONDITIONS_TYPE(OpaqueDataBlock)

