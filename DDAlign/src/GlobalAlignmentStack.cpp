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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDAlign/GlobalAlignmentStack.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

static dd4hep_ptr<GlobalAlignmentStack>& _stack()  {
  static dd4hep_ptr<GlobalAlignmentStack> s;
  return s;
}
static dd4hep_ptr<GlobalAlignmentStack>& _stack(GlobalAlignmentStack* obj)  {
  dd4hep_ptr<GlobalAlignmentStack>& s = _stack();
  s.adopt(obj);
  return s;
}
#if 0
/// Constructor with partial initialization
GlobalAlignmentStack::StackEntry::StackEntry(DetElement element, const std::string& p, const Delta& del, double ov)
  : detector(element), delta(del), path(p), overlap(ov)
{
  InstanceCount::increment(this);
}

/// Copy constructor
GlobalAlignmentStack::StackEntry::StackEntry(const StackEntry& e)
  : detector(e.detector), delta(e.delta), path(e.path), overlap(e.overlap)
{
  InstanceCount::increment(this);
}

/// Default destructor
GlobalAlignmentStack::StackEntry::~StackEntry() {
  InstanceCount::decrement(this);
}

/// Set flag to reset the entry to it's ideal geometrical position
GlobalAlignmentStack::StackEntry& GlobalAlignmentStack::StackEntry::setReset(bool new_value)   {
  new_value ? (delta.flags |= RESET_VALUE) : (delta.flags &= ~RESET_VALUE);
  return *this;
}


/// Set flag to reset the entry's children to their ideal geometrical position
GlobalAlignmentStack::StackEntry& GlobalAlignmentStack::StackEntry::setResetChildren(bool new_value)   {
  new_value ? (delta.flags |= RESET_CHILDREN) : (delta.flags &= ~RESET_CHILDREN);
  return *this;
}


/// Set flag to check overlaps
GlobalAlignmentStack::StackEntry& GlobalAlignmentStack::StackEntry::setOverlapCheck(bool new_value)   {
  new_value ? (delta.flags |= CHECKOVL_DEFINED) : (delta.flags &= ~CHECKOVL_DEFINED);
  return *this;
}


/// Set the precision for the overlap check (otherwise the default is 0.001 cm)
GlobalAlignmentStack::StackEntry& GlobalAlignmentStack::StackEntry::setOverlapPrecision(double precision)   {
  delta.flags |= CHECKOVL_DEFINED;
  delta.flags |= CHECKOVL_VALUE;
  overlap = precision;
  return *this;
}
#endif

/// Default constructor
GlobalAlignmentStack::GlobalAlignmentStack()
{
  InstanceCount::increment(this);
}

/// Default destructor
GlobalAlignmentStack::~GlobalAlignmentStack()   {
  destroyObjects(m_stack);
  InstanceCount::decrement(this);
}

/// Static client accessor
GlobalAlignmentStack& GlobalAlignmentStack::get()  {
  if ( _stack().get() ) return *_stack();
  throw runtime_error("GlobalAlignmentStack> Stack not allocated -- may not be retrieved!");
}

/// Create an alignment stack instance. The creation of a second instance will be refused.
void GlobalAlignmentStack::create()   {
  if ( _stack().get() )   {
    throw runtime_error("GlobalAlignmentStack> Stack already allocated. Multiple copies are not allowed!");
  }
  _stack(new GlobalAlignmentStack());
}

/// Check existence of alignment stack
bool GlobalAlignmentStack::exists()   {
  return _stack().get() != 0;
}

/// Clear data content and remove the slignment stack
void GlobalAlignmentStack::release()    {
  if ( _stack().get() )  {
    _stack(0);
    return;
  }
  throw runtime_error("GlobalAlignmentStack> Attempt to delete non existing stack.");
}

/// Add a new entry to the cache. The key is the placement path
bool GlobalAlignmentStack::insert(const string& full_path, dd4hep_ptr<StackEntry>& entry)  {
  if ( entry.get() && !full_path.empty() )  {
    entry->path = full_path;
    return add(entry);
  }
  throw runtime_error("GlobalAlignmentStack> Attempt to apply an invalid alignment entry.");
}

/// Add a new entry to the cache. The key is the placement path
bool GlobalAlignmentStack::insert(dd4hep_ptr<StackEntry>& entry)  {
  return add(entry);
}

/// Add a new entry to the cache. The key is the placement path
bool GlobalAlignmentStack::add(dd4hep_ptr<StackEntry>& entry)  {
  if ( entry.get() && !entry->path.empty() )  {
    Stack::const_iterator i = m_stack.find(entry->path);
    if ( i == m_stack.end() )   {
      StackEntry* e = entry.get();
      // Need to make some checks BEFORE insertion
      if ( !e->detector.isValid() )   {
        throw runtime_error("GlobalAlignmentStack> Invalid alignment entry [No such detector]");
      }
      printout(INFO,"GlobalAlignmentStack","Add node:%s",e->path.c_str());
      m_stack.insert(make_pair(e->path,entry.release()));
      return true;
    }
    throw runtime_error("GlobalAlignmentStack> The entry with path "+entry->path+
                        " cannot be re-aligned twice in one transaction.");
  }
  throw runtime_error("GlobalAlignmentStack> Attempt to apply an invalid alignment entry.");
}

/// Retrieve an alignment entry of the current stack
dd4hep_ptr<GlobalAlignmentStack::StackEntry> GlobalAlignmentStack::pop()   {
  Stack::iterator i = m_stack.begin();
  if ( i != m_stack.end() )   {
    dd4hep_ptr<StackEntry> e((*i).second);
    m_stack.erase(i);
    return e;
  }
  throw runtime_error("GlobalAlignmentStack> Alignment stack is empty. "
                      "Cannot pop entries - check size first!");
}

/// Get all pathes to be aligned
vector<const GlobalAlignmentStack::StackEntry*> GlobalAlignmentStack::entries() const    {
  vector<const StackEntry*> result;
  result.reserve(m_stack.size());
  for(Stack::const_iterator i=m_stack.begin(); i != m_stack.end(); ++i)
    result.push_back((*i).second);
  return result;
}

