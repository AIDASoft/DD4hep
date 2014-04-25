// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework includes
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {
  void set_condition_values(Condition::Object* c, const Condition::Entry* e)   {
    c->type = e->type;
    c->value = e->value;
    c->comment = "----";
    c->address = "----";
    c->validity = e->validity;
    c->detector = e->detector;
  }
  Condition make_condition(const Condition::Entry* e)   {
    Condition c(e->name);
    set_condition_values(c.ptr(),e);
    return c;
  }
}


/// Standard initializing constructor
Block::Block() : grammar(0), pointer(0)   {
}

/// Standard Destructor
Block::~Block()   {
}

/// Create data block from string representation
void Block::fromString(const std::string& rep)   {
  if ( pointer && grammar )  {
    grammar->fromString(pointer,rep);
    return;
  }
  throw runtime_error("Conditions data block is unbound. Cannot parse string representation.");
}

/// Create string representation of the data block
std::string Block::str()   {
  if ( pointer && grammar )  {
    return grammar->str(pointer);
  }
  throw runtime_error("Conditions data block is unbound. Cannot create string representation.");
}

/// Initializing constructor
Condition::Condition(const string& nam) : Handle<Object>()  {
  assign(new ConditionsInterna::Object(),nam,"condition");
}

/// Assignment operator
Condition& Condition::operator=(const Condition& c)   {
  if ( this != &c )  this->m_element = c.m_element;
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
int Condition::iovType() const   {
  return access()->iov.type;
}

/// Access the IOV block
Condition::IOV& Condition::iov() const   {
  return access()->iov;
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
  if ( !g ) invalidHandleError<Condition>();
  return *g;
}

/// Replace the data block of the condition with a new value. Free old data
Condition& Condition::replace(Entry* new_condition)    {
  set_condition_values(access(),new_condition);
  return rebind();
}

/// Re-evaluate the conditions data according to the previous bound type definition
Condition& Condition::rebind()    {
  Object* o = access();
  o->data.fromString(o->value);
  o->iov.fromString(o->validity);
  printout(INFO,"Condition","+++ condition:%s : %s rebinding value:%s",
	   detector().path().c_str(), name().c_str(), o->value.c_str());
  return *this;
}

/// Clear all conditions. Auto-delete of all existing entries
void Conditions::removeElements() const   {
  if ( !isValid() )   {
    invalidHandleError<Conditions>();
  }
  data<Object>()->removeElements();
}

/// Access the number of conditons available for this detector element
size_t Conditions::count() const   {
  if ( !isValid() )   {
    invalidHandleError<Conditions>();
  }
  return data<Object>()->entries.size();
}

/// Access the full map of conditons
Conditions::Entries& Conditions::entries() const  {
  if ( !isValid() )   {
    invalidHandleError<Conditions>();
  }
  return data<Object>()->entries;
}

/** Set a single conditions value (eventually existing entries are overwritten)
 * Note: Passing a valid handle also passes ownership!!!
 *
 * Failure return 0 in case an invalid handle is present
 * Successful insertion returns 1
 * Successful overwriting an existing value returns 3
 */
int Conditions::set(Entry* cond)   {
  int status = 0;
  if ( cond )  {
    Condition c;
    Object* o = 0;
    status = 1;
    if ( isValid() )   {
      o = data<Object>();
      Entries::iterator i = o->entries.find(cond->name);
      if ( i != o->entries.end() )  {
	(*i).second.replace(cond);
	c = (*i).second;
	status = 2;
      }
    }
    else  {
      DetElement det(cond->detector);
      assign(o=new ConditionsInterna::Container(),cond->name,cond->detector.name());
      det._data().conditions = *this;  // Ugly. Need to fix this....
    }
    if ( status == 1 )   {
      c = make_condition(cond);
      o->entries.insert(make_pair(cond->name,c));
    }
    printout(INFO,"Conditions","+++ %s condition:%s : %s value:%s",
	     (status==1) ? "Added NEW" : "Replaced existing",
	     c.detector().path().c_str(), c.name().c_str(), c->value.c_str());
    delete cond;
  }
  return status;
}
