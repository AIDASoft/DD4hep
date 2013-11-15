// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DDG4/ComponentUtils.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>
#include <map>

#ifndef _WIN32
#include <cxxabi.h>
typedef abi::__class_type_info class_t;
using abi::__dynamic_cast;
#endif

using namespace std;
using namespace DD4hep;

namespace {

  const std::string __typename(const char* class_name) {
    std::string result;
#ifdef _WIN32
    long off = 0;
    if ( ::strncmp(class_name, "class ", 6) == 0 ) {
      // The returned name is prefixed with "class "
      off = 6;
    }
    if ( ::strncmp(class_name, "struct ", 7) == 0 ) {
      // The returned name is prefixed with "struct "
      off = 7;
    }
    if ( off > 0 ) {
      std::string tmp = class_name + off;
      long loc = 0;
      while( (loc = tmp.find("class ")) > 0 ) {
        tmp.erase(loc, 6);
      }
      loc = 0;
      while( (loc = tmp.find("struct ")) > 0 ) {
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

#elif defined(__linux) || defined(__APPLE__)
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
      case 'a':
        result = "signed char";
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
      int status;
      char* realname;
      realname = abi::__cxa_demangle(class_name, 0, 0, &status);
      if (realname == 0)
        return class_name;
      result = realname;
      free(realname);
      /// substitute ', ' with ','
      std::string::size_type pos = result.find(", ");
      while (std::string::npos != pos) {
        result.replace(pos, 2, ",");
        pos = result.find(", ");
      }
    }
#endif
    return result;
  }
}

/// ABI information about type names
std::string DD4hep::typeinfoName(const std::type_info& type) {
  return __typename(type.name());
}

void DD4hep::typeinfoCheck(const std::type_info& typ1, const std::type_info& typ2, const std::string& text) {
  if (typ1 != typ2) {
    throw unrelated_type_error(typ1, typ2, text);
  }
}

string unrelated_type_error::msg(const std::type_info& typ1, const std::type_info& typ2, const string& text) {
  std::string m = "The types " + __typename(typ1.name()) + " and " + __typename(typ2.name()) + " are not related. " + text;
  return m;
}

/// Initializing Constructor
ComponentCast::ComponentCast(const std::type_info& t)
    : type(t) {
  abi_class = dynamic_cast<const class_t*>(&type);
  if (!abi_class) {
    throw std::runtime_error("Class type " + typeinfoName(type) + " is not an abi object type!");
  }
}

/// Defautl destructor
ComponentCast::~ComponentCast() {
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

static inline void* cast_wrap(const void* p,
			      const abi::__class_type_info* src,
			      const abi::__class_type_info* dst,
			      ptrdiff_t src2dst)
{
  return abi::__dynamic_cast(p,src,dst,src2dst);
  // Don't know what to do on the damned MACs....
  //return (p && src && dst && src2dst) ? 0 : 0;
}

/// Apply cast using typeinfo instead of dynamic_cast
void* ComponentCast::apply_dynCast(const ComponentCast& to, const void* ptr) const {
  if (&to == this) {
    return (void*) ptr;
  }
  const class_t* src_type = (const class_t*)to.abi_class;
  if (src_type) {
    // First try down cast
    void *r = cast_wrap(ptr, src_type, (const class_t*) abi_class, -1);
    if (r)
      return r;
    // Now try the up-cast
    r = cast_wrap(ptr, (const class_t*) abi_class, src_type, -1);
    if (r)
      return r;
    throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
  }
  throw unrelated_type_error(type, to.type, "Target type is not an abi class type!");
}

/// Apply cast using typeinfo instead of dynamic_cast
void* ComponentCast::apply_upCast(const ComponentCast& to, const void* ptr) const {
  if (&to == this) {
    return (void*) ptr;
  }
  return apply_dynCast(to, ptr);
}

/// Apply cast using typeinfo instead of dynamic_cast
void* ComponentCast::apply_downCast(const ComponentCast& to, const void* ptr) const {
  if (&to == this) {
    return (void*) ptr;
  }
  const class_t* src_type = (const class_t*)to.abi_class;
  if (src_type) {
    void *r = cast_wrap(ptr, src_type, (const class_t*)abi_class, -1);
    if (r)
      return r;
    throw unrelated_type_error(type, to.type, "Failed to apply abi dynamic cast operation!");
  }
  throw unrelated_type_error(type, to.type, "Target type is not an abi class type!");
}

