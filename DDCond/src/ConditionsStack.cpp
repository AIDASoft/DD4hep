// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDCond/ConditionsStack.h"
#include "DD4hep/objects/ConditionsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static dd4hep_ptr<ConditionsStack>& _stack()  {
  static dd4hep_ptr<ConditionsStack> s;
  return s;
}
static dd4hep_ptr<ConditionsStack>& _stack(ConditionsStack* obj)  {
  dd4hep_ptr<ConditionsStack>& s = _stack();
  s = dd4hep_ptr<ConditionsStack>(obj);
  return s;
}

/// Standard constructor
ConditionsStack::ConditionsStack()  {
  InstanceCount::increment(this);
}

/// Standard destructor
ConditionsStack::~ConditionsStack()  {
  destroyObjects(m_stack)();
  InstanceCount::decrement(this);
}

/// Static client accessor
ConditionsStack& ConditionsStack::get()  {
  if ( _stack().get() ) return *_stack();
  throw runtime_error("ConditionsStack> Stack not allocated -- may not be retrieved!");
}

/// Create an conditions stack instance. The creation of a second instance will be refused.
void ConditionsStack::create()   {
  if ( _stack().get() )   {
    throw runtime_error("ConditionsStack> Stack already allocated. Multiple copies are not allowed!");
  }
  _stack(new ConditionsStack());
}

/// Check existence of conditions stack
bool ConditionsStack::exists()   {
  return _stack().get() != 0;
}

/// Clear data content and remove the slignment stack
void ConditionsStack::release()    {
  if ( _stack().get() )  {
    _stack(0);
    return;
  }
  throw runtime_error("ConditionsStack> Attempt to delete non existing stack.");
}

/// Retrieve an conditions entry of the current stack
dd4hep_ptr<ConditionsStack::Entry> ConditionsStack::pop()   {
  Stack::iterator i = m_stack.begin();
  if ( i != m_stack.end() )   {
    Entry* e = (*i).second;
    m_stack.erase(i);
    return dd4hep_ptr<Entry>(e);
  }
  throw runtime_error("ConditionsStack> pop: Conditions stack is empty. Check size first!");
}

/// Get all pathes to be aligned
vector<const ConditionsStack::Entry*> ConditionsStack::entries() const    {
  vector<const Entry*> result;
  result.reserve(m_stack.size());
  for(Stack::const_iterator i=m_stack.begin(); i != m_stack.end(); ++i)
    result.push_back((*i).second);
  return result;
}

/// Push new entry into the stack
void ConditionsStack::insert(dd4hep_ptr<Entry>& data)   {
  if ( data.get() && !data->name.empty() )  {
    DetElement det = data->detector;
    // Need to make some checks BEFORE insertion
    if ( !det.isValid() )   {
      throw runtime_error("ConditionsStack> Invalid conditions data [No such detector]");
    }
    string    path = det.path()+'!'+data->name;
    Stack::const_iterator i = m_stack.find(path);
    printout(DEBUG,"ConditionsStack","+++ %s  name:%s type:%s value:%s  Validity:%s",
             det.path().c_str(), data->name.c_str(), data->type.c_str(), 
             data->value.c_str(), data->validity.c_str());
    if ( i == m_stack.end() )   {
      m_stack.insert(make_pair(path,data.release()));
      return;
    }
    throw runtime_error("ConditionsStack> The data with path "+path+
                        " cannot be set twice in one transaction. [Intended inhibit]");
  }
  throw runtime_error("ConditionsStack> Attempt to apply an invalid conditions data.");
}
