// $Id: IDDescriptor.cpp 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Handle.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>

using namespace std;
using namespace DD4hep;

/// Do not clutter global namespace
namespace {
  typedef InstanceCount::Counter COUNT;
  typedef std::map<const std::type_info*, COUNT*>      TypeCounter;
  typedef std::map<const std::string*,    COUNT*>      StringCounter;
  static bool s_trace_instances = ::getenv("DD4HEP_TRACE") != 0;
  static std::auto_ptr<TypeCounter>       s_typCounts(new TypeCounter());
  static std::auto_ptr<StringCounter>     s_strCounts(new StringCounter());
  static InstanceCount::Counter s_nullCount;
  static InstanceCount::Counter s_thisCount;
  static InstanceCount          s_counter;
  inline TypeCounter& types()      {    return *(s_typCounts.get());  }
  inline StringCounter& strings()  {    return *(s_strCounts.get());  }
}

/// Standard Constructor
InstanceCount::InstanceCount() {
  s_thisCount.increment();
}
/// Standard destructor
InstanceCount::~InstanceCount() {
  s_thisCount.decrement();
  if ( 0 == s_thisCount.value() )   {
    StringCounter::iterator i;
    TypeCounter::iterator   j;
    dump(s_trace_instances ? ALL : NONE);
    for(i=s_strCounts->begin(); i != s_strCounts->end(); ++i)
      delete (*i).second;
    for(j=s_typCounts->begin(); j != s_typCounts->end(); ++j)
      delete (*j).second;
    s_strCounts->clear();
    s_typCounts->clear();
  }
}
/// Check if tracing is enabled.
bool InstanceCount::doTrace()    {
  return s_trace_instances;
}
/// Enable/Disable tracing
void InstanceCount::doTracing(bool value)    {
  s_trace_instances = value;
}
/// Access counter object for local caching on optimizations
InstanceCount::Counter* InstanceCount::getCounter(const std::type_info& typ)  {
  Counter* cnt = s_trace_instances ? types()[&typ] : &s_nullCount;
  return (0!=cnt) ? cnt : types()[&typ] = new Counter();
}
/// Access counter object for local caching on optimizations
InstanceCount::Counter* InstanceCount::getCounter(const std::string& typ) {
  Counter* cnt = s_trace_instances ? strings()[&typ] : &s_nullCount;
  return (0!=cnt) ? cnt : strings()[&typ] = new Counter();
}
/// Force dump of counters
void InstanceCount::dump(int typ)  {
  bool need_footer = false;
  if ( (typ & STRING) && s_strCounts.get() )    {
    if ( s_strCounts->begin() != s_strCounts->end() )   {
      StringCounter::const_iterator i;
      std::cout << "+----------------------------------------------------------------+" << std::endl;
      std::cout << "|   I n s t a n c e   c o u n t e r s   b y    N A M E           |" << std::endl;
      std::cout << "+----------+---------+-------------------------------------------+" << std::endl;
      std::cout << "|   Total  | Leaking |      Type identifier                      |" << std::endl;
      std::cout << "+----------+---------+-------------------------------------------+" << std::endl;
      for(i=s_strCounts->begin(); i != s_strCounts->end(); ++i) {
        std::cout << "|" << std::setw(10) << (*i).second->total()
                  << "|" << std::setw(9)  << (*i).second->value()
                  << "|" << (*i).first << std::endl;       
      }
      need_footer = true;
    }
  }

  if ( (typ & TYPEINFO) && s_typCounts.get() )   {
    if ( s_typCounts->begin() != s_typCounts->end() )  {
      TypeCounter::const_iterator i;
      std::cout << "+----------------------------------------------------------------+" << std::endl;
      std::cout << "|   I n s t a n c e   c o u n t e r s   b y    T Y P E I N F O   |" << std::endl;
      std::cout << "+----------+---------+-------------------------------------------+" << std::endl;
      std::cout << "|   Total  | Leaking |      Type identifier                      |" << std::endl;
      std::cout << "+----------+---------+-------------------------------------------+" << std::endl;
      for(i=s_typCounts->begin(); i != s_typCounts->end(); ++i) {
        std::cout << "|" << std::setw(10) << (*i).second->total()
                  << "|" << std::setw(9)  << (*i).second->value()
                  << "|" << typeName(*((*i).first)) << std::endl;       
      }
      need_footer = true;
    } 
  }
  if ( need_footer )  {
    std::cout << "+----------+-------+-------------------------------------------+" << std::endl;
  }
}
