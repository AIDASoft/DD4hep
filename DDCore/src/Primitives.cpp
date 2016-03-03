// $Id: $
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
#include "DD4hep/Primitives.h"
#include "DD4hep/Exceptions.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>
#include <map>

#if defined(__linux) || defined(__APPLE__)
#include <cxxabi.h>
#ifndef __APPLE__
typedef abi::__class_type_info class_t;
using   abi::__dynamic_cast;
#endif
#endif

static const std::string __typeinfoName(const std::type_info& tinfo) {
  const char* class_name = tinfo.name();
  std::string result;
#ifdef WIN32
  std::size_t off = 0;
  if ( ::strncmp(class_name, "class ", 6) == 0 ) {
    // The returned name is prefixed with "class "
    off = 6;
  }
  if ( ::strncmp(class_name, "struct ", 7) == 0 ) {
    // The returned name is prefixed with "struct "
    off = 7;
  }
  if ( off != std::string::npos ) {
    std::string tmp = class_name + off;
    size_t loc = 0;
    while( (loc = tmp.find("class ")) != std::string::npos ) {
      tmp.erase(loc, 6);
    }
    loc = 0;
    while( (loc = tmp.find("struct ")) != std::string::npos ) {
      tmp.erase(loc, 7);
    }
    result = tmp;
  }
  else {
    result = class_name;
  }
  // Change any " *" to "*"
  while ( (off=result.find(" *")) != std::string::npos ) {
    result.replace(off, 2, "*");
  }
  // Change any " &" to "&"
  while ( (off=result.find(" &")) != std::string::npos ) {
    result.replace(off, 2, "&");
  }
#elif defined(sun)
  result = class_name;
#elif !defined(__ICC)
  if (::strlen(class_name) == 1) {
    // See http://www.realitydiluted.com/mirrors/reality.sgi.com/dehnert_engr/cxx/abi.pdf
    // for details
    switch (class_name[0]) {
    case 'v':
      result = "void";
      break;
    case 'w':
      result = "wchar_t";
      break;
    case 'b':
      result = "bool";
      break;
    case 'c':
      result = "char";
      break;
    case 'h':
      result = "unsigned char";
      break;
    case 's':
      result = "short";
      break;
    case 't':
      result = "unsigned short";
      break;
    case 'i':
      result = "int";
      break;
    case 'j':
      result = "unsigned int";
      break;
    case 'l':
      result = "long";
      break;
    case 'm':
      result = "unsigned long";
      break;
    case 'x':
      result = "long long";
      break;
    case 'y':
      result = "unsigned long long";
      break;
    case 'n':
      result = "__int128";
      break;
    case 'o':
      result = "unsigned __int128";
      break;
    case 'f':
      result = "float";
      break;
    case 'd':
      result = "double";
      break;
    case 'e':
      result = "long double";
      break;
    case 'g':
      result = "__float128";
      break;
    case 'z':
      result = "ellipsis";
      break;
    }
  }
  else {
    char buff[16 * 1024];
    std::size_t len = sizeof(buff);
    int status = 0;
    result = __cxxabiv1::__cxa_demangle(class_name, buff, &len, &status);
  }
#else
  result = class_name;
  throw std::runtime_error("CXXABI is missing for ICC!");
#endif
  return result;
}

std::string DD4hep::typeName(const std::type_info& typ) {
  return __typeinfoName(typ);
}

/// Default destructor of specialized exception
DD4hep::invalid_handle_exception::~invalid_handle_exception()  {
}

void DD4hep::invalidHandleError(const std::type_info& type)
  throw(std::exception)
{
  throw invalid_handle_exception("Attempt to access invalid object of type "+typeName(type)+" [Invalid Handle]");
}

void DD4hep::invalidHandleAssignmentError(const std::type_info& from, 
                                          const std::type_info& to)
  throw(std::exception)
{
  std::string msg = "Wrong assingment from ";
  msg += typeName(from);
  msg += " to ";
  msg += typeName(to);
  msg += " not possible!!";
  throw invalid_handle_exception(msg);
}

/// Throw exception when handles are check for validity
void DD4hep::notImplemented(const std::string& msg)
  throw(std::exception)
{
  std::string m = "The requested feature " + msg + " is not implemented!";
  throw std::runtime_error(m);
}

void DD4hep::typeinfoCheck(const std::type_info& typ1, const std::type_info& typ2, const std::string& text)
  throw(std::exception)
{
  if (typ1 != typ2) {
    throw unrelated_type_error(typ1, typ2, text);
  }
}

namespace DD4hep   {
  template<> const char* Primitive<bool>::default_format()           { return "%d"; }
  template<> const char* Primitive<char>::default_format()           { return "%c"; }
  template<> const char* Primitive<unsigned char>::default_format()  { return "%02X"; }
  template<> const char* Primitive<short>::default_format()          { return "%d"; }
  template<> const char* Primitive<unsigned short>::default_format() { return "%04X"; }
  template<> const char* Primitive<int>::default_format()            { return "%d"; }
  template<> const char* Primitive<unsigned int>::default_format()   { return "%08X"; }
  template<> const char* Primitive<long>::default_format()           { return "%ld"; }
  template<> const char* Primitive<unsigned long>::default_format()  { return "%016X"; }
  template<> const char* Primitive<float>::default_format()          { return "%f"; }
  template<> const char* Primitive<double>::default_format()         { return "%g"; }
  template<> const char* Primitive<char*>::default_format()          { return "%s"; }
  template<> const char* Primitive<const char*>::default_format()    { return "%s"; }
  template<> const char* Primitive<std::string>::default_format()    { return "%s"; }

  /// Generic function to convert to string
  template <typename T> std::string Primitive<T>::toString(T value) {
    char text[1024];
    ::snprintf(text,sizeof(text),default_format(),value);
    return text;
  }

  /// Convert string to string
  template <> std::string Primitive<const char*>::toString(const char* value) {
    if ( value )  {
      return value;
    }
    throw std::runtime_error("Failed to convert (char*)NULL to std::string!");
  }
  /// Convert string to string
  template <> std::string Primitive<char*>::toString(char* value) {
    if ( value )  {
      return value;
    }
    throw std::runtime_error("Failed to convert (char*)NULL to std::string!");
  }
  /// Convert string to string
  template <> std::string Primitive<std::string>::toString(std::string value) {
    return value;
  }

  template std::string Primitive<bool>::toString(bool value);
  template std::string Primitive<char>::toString(char value);
  template std::string Primitive<unsigned char>::toString(unsigned char value);
  template std::string Primitive<short>::toString(short value);
  template std::string Primitive<unsigned short>::toString(unsigned short value);
  template std::string Primitive<int>::toString(int value);
  template std::string Primitive<unsigned int>::toString(unsigned int value);
  template std::string Primitive<long>::toString(long value);
  template std::string Primitive<unsigned long>::toString(unsigned long value);
  template std::string Primitive<float>::toString(float value);
  template std::string Primitive<double>::toString(double value);
}

/// Initializing Constructor
DD4hep::ComponentCast::ComponentCast(const std::type_info& t, destroy_t d, cast_t c)
  : type(t), destroy(d), cast(c) {
#ifdef __APPLE__
  abi_class = 0;
#else
  abi_class = dynamic_cast<const class_t*>(&type);
  if (!abi_class) {
    throw std::runtime_error("Class type " + typeName(type) + " is not an abi object type!");
  }
#endif
}

/// Defautl destructor
DD4hep::ComponentCast::~ComponentCast() {
}

#if 0
// Dynamic cast runtime.
// src2dst has the following possible values
//  >-1: src_type is a unique public non-virtual base of dst_type
//       dst_ptr + src2dst == src_ptr
//   -1: unspecified relationship
//   -2: src_type is not a public base of dst_type
//   -3: src_type is a multiple public non-virtual base of dst_type
extern "C" void*
__dynamic_cast(const void* __src_ptr,// Starting object.
               const abi::__class_type_info* __src_type,// Static type of object.
               const abi::__class_type_info* __dst_type,// Desired target type.
               ptrdiff_t __src2dst);// How src and dst are related.
#endif

#ifndef __APPLE__
static inline void* cast_wrap(const void* p,
                              const abi::__class_type_info* src,
                              const abi::__class_type_info* dst,
                              ptrdiff_t src2dst)
{
  return abi::__dynamic_cast(p,src,dst,src2dst);
}
#endif

/// Apply cast using typeinfo instead of dynamic_cast
void* DD4hep::ComponentCast::apply_dynCast(const ComponentCast& to, const void* ptr) const
  throw(std::exception)
{
  if (&to == this) {
    return (void*) ptr;
  }
#ifdef __APPLE__
  // First try down cast
  void *r = (*to.cast)(ptr);
  if (r)
    return r;
  // Now try the up-cast
  r = (*cast)(ptr);
  if (r)      return r;
  throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
#else
  void* r = (void*)ptr;
  if ( to.abi_class )  {
    bool cast_worked = type.__do_upcast((const class_t*)to.abi_class,&r);
    if ( cast_worked ) return r;
    r = (void*)ptr;
    cast_worked = to.type.__do_upcast((const class_t*)abi_class,&r);
    if ( cast_worked ) return r;
#if 0
    const class_t* src_type = (const class_t*)to.abi_class;
    if (src_type) {
      // First try down cast
      void *r = cast_wrap(ptr, src_type, (const class_t*) abi_class, -1);
      if ( r ) return r;
      // Now try the up-cast
      r = cast_wrap(ptr, (const class_t*) abi_class, src_type, -1);
      if (r)      return r;
    }
#endif
    throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
  }
  throw unrelated_type_error(type, to.type, "Target type is not an abi class type!");
#endif
}

/// Apply cast using typeinfo instead of dynamic_cast
void* DD4hep::ComponentCast::apply_upCast(const ComponentCast& to, const void* ptr) const
  throw(std::exception)
{
  if (&to == this) {
    return (void*) ptr;
  }
  return apply_dynCast(to, ptr);
}
  
/// Apply cast using typeinfo instead of dynamic_cast
void* DD4hep::ComponentCast::apply_downCast(const ComponentCast& to, const void* ptr) const
  throw(std::exception)
{
  if (&to == this) {
    return (void*) ptr;
  }
#ifdef __APPLE__
  void *r = (*to.cast)(ptr);
  if (r) return r;
  throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
#else
  if ( to.abi_class )  {
    // Since we have to cast a 'to' pointer up to the real pointer
    // no virtual inheritance can be supported!
    void* r = (void*)ptr;
    bool cast_worked = type.__do_upcast((const class_t*)to.abi_class,&r);
    if ( cast_worked ) return r;
#if 0
    void *r = cast_wrap(ptr, src_type, (const class_t*)abi_class, -1);
    if (r) return r;
#endif
    throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
  }
  throw unrelated_type_error(type, to.type, "Target type is not an abi class type!");
#endif
}

