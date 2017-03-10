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
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Handle.h"
#include "DD4hep/Memory.h"
// C/C++ include files
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

using namespace std;
using namespace DD4hep;

/// Do not clutter global namespace
namespace {
  typedef InstanceCount::Counter COUNT;
  typedef std::map<const std::type_info*, COUNT*> TypeCounter;
  typedef std::map<const std::string*, COUNT*> StringCounter;
  static bool s_trace_instances = ::getenv("DD4HEP_TRACE") != 0;
  static dd4hep_ptr<TypeCounter> s_typCounts(new TypeCounter());
  static dd4hep_ptr<StringCounter> s_strCounts(new StringCounter());
  static InstanceCount::Counter s_nullCount;
  static InstanceCount::Counter s_thisCount;
  static InstanceCount s_counter;
  inline TypeCounter& types() {
    return *(s_typCounts.get());
  }
  inline StringCounter& strings() {
    return *(s_strCounts.get());
  }
  int s_global = 1;
  struct _Global {
    _Global() {}
    ~_Global() { s_global = 0; }
  } s_globalObj;
  int on_exit_destructors()  {
    static bool first = true;
    if ( first && s_global == 0 && s_trace_instances )  {
      first = false;
      ::printf("Static out of order destructors occurred. Reference count table is unreliable.....\n");
    }
    return 1;
  }
}

/// Standard Constructor
InstanceCount::InstanceCount() {
  s_thisCount.increment();
}
/// Standard destructor
InstanceCount::~InstanceCount() {
  s_thisCount.decrement();
  if (0 == s_thisCount.value()) {
    StringCounter::iterator i;
    TypeCounter::iterator j;
    dump(s_trace_instances ? ALL : NONE);
    for (i = s_strCounts->begin(); i != s_strCounts->end(); ++i)
      delete (*i).second;
    for (j = s_typCounts->begin(); j != s_typCounts->end(); ++j)
      delete (*j).second;
    s_strCounts->clear();
    s_typCounts->clear();
  }
}
/// Check if tracing is enabled.
bool InstanceCount::doTrace() {
  return s_trace_instances;
}
/// Enable/Disable tracing
void InstanceCount::doTracing(bool value) {
  s_trace_instances = value;
}
/// Access counter object for local caching on optimizations
InstanceCount::Counter* InstanceCount::getCounter(const std::type_info& typ) {
  Counter* cnt = s_trace_instances ? types()[&typ] : &s_nullCount;
  return (0 != cnt) ? cnt : types()[&typ] = new Counter();
}

/// Access counter object for local caching on optimizations
InstanceCount::Counter* InstanceCount::getCounter(const std::string& typ) {
  Counter* cnt = s_trace_instances ? strings()[&typ] : &s_nullCount;
  return (0 != cnt) ? cnt : strings()[&typ] = new Counter();
}

/// Increment count according to string information
void InstanceCount::increment(const std::string& typ) {
  if ( s_global )
    getCounter(typ)->increment();
  else
    on_exit_destructors();
}

/// Decrement count according to string information
void InstanceCount::decrement(const std::string& typ) {
  if ( s_global )
    getCounter(typ)->decrement();
  else
    on_exit_destructors();
}

/// Increment count according to type information
void InstanceCount::increment(const std::type_info& typ) {
  if ( s_global )
    getCounter(typ)->increment();
  else
    on_exit_destructors();
}

/// Decrement count according to type information
void InstanceCount::decrement(const std::type_info& typ) {
  if ( s_global )
    getCounter(typ)->decrement();
  else
    on_exit_destructors();
}

/// Force dump of counter
void InstanceCount::dump(int typ) {
  bool need_footer = false;
  if ((typ & STRING) && s_strCounts.get()) {
    if ( !s_strCounts->empty() )  {
      cout << "+--------------------------------------------------------------------------+" << endl;
      cout << "|   I n s t a n c e   c o u n t e r s   b y    N A M E                     |" << endl;
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      cout << "|   Total  |  Max    | Leaking |      Type identifier                      |" << endl;
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      long tot_instances=0, max_instances=0, now_instances=0;
      for ( const auto& i : *s_strCounts ) {
        cout << "|" << setw(10) << i.second->total()
             << "|" << setw(9)  << i.second->maximum()
             << "|" << setw(9)  << i.second->value()
             << "|" << i.first->substr(0,80) << endl;
        tot_instances += i.second->total();
        max_instances += i.second->maximum();
        now_instances += i.second->value();
      }
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      cout << "|" << setw(10) << tot_instances
           << "|" << setw(9)  << max_instances
           << "|" << setw(9)  << now_instances
           << "|" << "Grand total (Sum of all counters)" << endl;
      need_footer = true;
    }
  }
  if ((typ & TYPEINFO) && s_typCounts.get()) {
    if ( !s_typCounts->empty() ) {
      cout << "+--------------------------------------------------------------------------+" << endl;
      cout << "|   I n s t a n c e   c o u n t e r s   b y    T Y P E I N F O             |" << endl;
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      cout << "|   Total  |  Max    | Leaking |      Type identifier                      |" << endl;
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      long tot_instances=0, max_instances=0, now_instances=0;
      for ( const auto& i : *s_typCounts ) {
        string nam = typeName(*(i.first));
        if ( nam.length() > 80 ) nam = nam.substr(0,80)+" ...";
        cout << "|" << setw(10) << i.second->total()
             << "|" << setw(9)  << i.second->maximum()
             << "|" << setw(9)  << i.second->value()
             << "|" << nam << endl;
        tot_instances += i.second->total();
        max_instances += i.second->maximum();
        now_instances += i.second->value();
      }
      cout << "+----------+---------+---------+-------------------------------------------+" << endl;
      cout << "|" << setw(10) << tot_instances
           << "|" << setw(9)  << max_instances
           << "|" << setw(9)  << now_instances
           << "|" << "Grand total (Sum of all counters)" << endl;
      need_footer = true;
    }
  }
  if (need_footer) {
    cout << "+----------+-------+-------------------------------------------+" << endl;
  }
}
