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
OpaqueDataBlock::OpaqueDataBlock() : OpaqueData(), type(0)   {
  InstanceCount::increment(this);
}

/// Copy constructor
OpaqueDataBlock::OpaqueDataBlock(const OpaqueDataBlock& c) 
  : OpaqueData(c), type(c.type)   {
  grammar = 0;
  pointer = 0;
  this->bind(c.grammar);
  if ( this->grammar->specialization.copy )  {
    this->grammar->specialization.copy(pointer, c.pointer);
  }
  else  {
    except("OpaqueDataBlock","Grammar type %s does not support object copy. Operation not allowed.",
	   this->grammar->type_name().c_str());
  }
  InstanceCount::increment(this);
}

/// Standard Destructor
OpaqueDataBlock::~OpaqueDataBlock()   {
  if ( pointer && (type&EXTERN_DATA) != EXTERN_DATA )  {
    grammar->destruct(pointer);
    if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
  }
  pointer = 0;
  grammar = 0;
  InstanceCount::decrement(this);
}

/// Copy constructor
OpaqueDataBlock& OpaqueDataBlock::operator=(const OpaqueDataBlock& c)   {
  if ( this != &c )  {
    if ( grammar == c.grammar )   {
      if ( pointer )  {
        if ( grammar ) grammar->destruct(pointer);
        if ( (type&ALLOC_DATA) == ALLOC_DATA ) ::operator delete(pointer);
      }
      pointer = 0;
      grammar = 0;
    }
    if ( grammar == 0 )  {
      this->OpaqueData::operator=(c);
      type = c.type;
      grammar = 0;
      if ( c.grammar )   {
	if ( !c.grammar->specialization.copy )  {
	  except("OpaqueDataBlock","Grammar type %s does not support object copy. Operation not allowed.",
		 c.grammar->type_name().c_str());
	}
        bind(c.grammar);
        grammar->specialization.copy(pointer,c.pointer);
        return *this;
      }
      else if ( (c.type&EXTERN_DATA) == EXTERN_DATA )   {
        pointer = c.pointer;
        return *this;
      }
    }
    except("OpaqueData","You may not bind opaque data multiple times!");
  }
  return *this;
}

/// Bind data value
void* OpaqueDataBlock::bind(const BasicGrammar* g)   {
  if ( (type&EXTERN_DATA) == EXTERN_DATA )  {
    except("OpaqueData","Extern data may not be bound!");
  }
  else if ( !grammar )  {
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

/// Bind external data value to the pointer
void OpaqueDataBlock::bindExtern(void* ptr, const BasicGrammar* gr)    {
  if ( grammar != 0 && type != EXTERN_DATA )  {
    // We cannot ingore secondary requests for data bindings.
    // This leads to memory leaks in the caller!
    except("OpaqueData","You may not bind opaque data multiple times!");
  }
  pointer = ptr;
  grammar = gr;
  type    = EXTERN_DATA;
}

/// Set data value
void* OpaqueDataBlock::bind(void* ptr, size_t size, const BasicGrammar* g)   {
  if ( (type&EXTERN_DATA) == EXTERN_DATA )  {
    except("OpaqueData","Extern data may not be bound!");
  }
  else if ( !grammar )  {
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
    except("OpaqueData","You may not bind opaque data multiple times!");
  }
  typeinfoCheck(grammar->type(),g->type(),"Opaque data blocks may not be assigned.");
  return 0;
}

#include "DD4hep/detail/Grammar_unparsed.h"
static auto s_registry = GrammarRegistry::pre_note<OpaqueDataBlock>(1);
