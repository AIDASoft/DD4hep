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

// Framework include files
#include "DD4hep/Handle.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

    /// Helper routine called when unrelated types are assigned.
    template <typename T> void Handle<T>::bad_assignment(const std::type_info& from, const std::type_info& to) 
    {  
      invalidHandleAssignmentError(from,to);
    }

    /// Assign a new named object. Note: object references must be managed by the user
    template <typename T> void Handle<T>::assign(T* n, const std::string& nam, const std::string& tit) {
      this->m_element = n;
      if (!nam.empty())
        n->SetName(nam.c_str());
      if (!tit.empty())
        n->SetTitle(tit.c_str());
    }

    /// Access the object name (or "" if not supported by the object)
    template <typename T> const char* Handle<T>::name() const {
      return this->m_element ? this->m_element->GetName() : "";
    }

    /// Checked object access. Throws invalid handle runtime exception
    /** Very compact way to check the validity of a handle with exception thrown.  
    */
    template <typename T> T* Handle<T>::access() const   {
      if ( this->m_element ) return this->m_element;
      invalidHandleError(typeid(T));
      return 0; // We have thrown an exception before - does not harm!
    }
}   /* End namespace dd4hep      */


#ifdef DD4HEP_USE_SAFE_CAST

#define DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM, TO)                                                  \
  namespace dd4hep {  namespace detail  {                                                          \
    template <> template <> TO* safe_cast<TO>::cast<FROM>(FROM* p)                                 \
    {  return dynamic_cast<TO*>(p);  }                                                             \
    template <> template <> TO* safe_cast<TO>::cast<FROM>(const FROM* p)                           \
    {  return const_cast<TO*>(dynamic_cast<const TO*>(p));  }                                      \
    template <> template <> TO* safe_cast<TO>::cast_non_null<FROM>(FROM* p)  {                     \
      TO* ptr = dynamic_cast<TO*>(p);                                                              \
      if ( ptr ) return ptr;                                                                       \
      invalidHandleAssignmentError(typeid(FROM),typeid(TO));                                       \
      return ptr;                                                                                  \
    }                                                                                              \
    template <> template <> TO* safe_cast<TO>::cast_non_null<FROM>(const FROM* p) {                \
      return safe_cast<TO>::cast_non_null<FROM>(const_cast<FROM*>(p));                             \
    }                                                                                              \
  }}

// Predefined simple cast map
#define DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TO)                                                        \
  DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM,TO)                                                         \
  DD4HEP_SAFE_CAST_IMPLEMENTATION(TO,FROM)

// Predefined cast map including standard object types
#define DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(FROM)                                                     \
  DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TObject)                                                         \
  DD4HEP_IMPLEMENT_SAFE_CAST(FROM,NamedObject)                                                     \
  DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TNamed)                                                          \
  DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM,FROM)

#else

#define DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM, TO)                                                  \
  namespace dd4hep {  namespace detail  {                                                          \
    template <> FROM* safe_cast<FROM>::cast(FROM* p)                                               \
    {  return dynamic_cast<FROM*>(p);  }                                                           \
    template <> template <> FROM* safe_cast<FROM>::cast_non_null(FROM* p)  {                       \
       FROM* ptr = const_cast<FROM*>(dynamic_cast<const FROM*>(p));                                \
       if ( !ptr )  invalidHandleError(typeid(FROM));                                              \
       return ptr;                                                                                 \
    }                                                                                              \
  }}
// Predefined simple cast map
#define DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TO)      DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM,FROM)

// Predefined cast map including standard object types
#define DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(FROM)   DD4HEP_SAFE_CAST_IMPLEMENTATION(FROM,FROM)

#endif

// Utility for counting the number of args in the __VA_ARGS__ pack:
#define DD4HEP_HANDLE_PP_NARGS(...)  DD4HEP_HANDLE_PP_NARGS2(__VA_ARGS__, DD4HEP_HANDLE_PP_NARGS_COUNT())
#define DD4HEP_HANDLE_PP_NARGS2(...) DD4HEP_HANDLE_PP_NARGS_IMPL(__VA_ARGS__)
#define DD4HEP_HANDLE_PP_NARGS_IMPL(x1, x2, x3, x4, x5, x6, x7, x8, x9, N, ...) N
#define DD4HEP_HANDLE_PP_NARGS_COUNT() 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ERROR

// Macros to delegate to concrete, defined-arity implementations:
#define DD4HEP_HANDLE_XF(count, ...)      DD4HEP_HANDLE_XF_IMPL (count, __VA_ARGS__)
#define DD4HEP_HANDLE_XF_IMPL(count, ...) DD4HEP_HANDLE_XF_ ## count (__VA_ARGS__)

#define DD4HEP_HANDLE_XF_1(CODE,x1)                              DD4HEP_TEMPLATE_HANDLE(CODE,x1)
#define DD4HEP_HANDLE_XF_2(CODE,x1,x2)                           DD4HEP_IMPLEMENT_SAFE_CAST(x1,x2)  DD4HEP_HANDLE_XF_1(CODE,x1)
#define DD4HEP_HANDLE_XF_3(CODE,x1,x2,x3)                        DD4HEP_IMPLEMENT_SAFE_CAST(x1,x3)  DD4HEP_HANDLE_XF_2(CODE,x1,x2)
#define DD4HEP_HANDLE_XF_4(CODE,x1,x2,x3,x4)                     DD4HEP_IMPLEMENT_SAFE_CAST(x1,x4)  DD4HEP_HANDLE_XF_3(CODE,x1,x2,x3)
#define DD4HEP_HANDLE_XF_5(CODE,x1,x2,x3,x4,x5)                  DD4HEP_IMPLEMENT_SAFE_CAST(x1,x5)  DD4HEP_HANDLE_XF_4(CODE,x1,x2,x3,x4)
#define DD4HEP_HANDLE_XF_6(CODE,x1,x2,x3,x4,x5,x6)               DD4HEP_IMPLEMENT_SAFE_CAST(x1,x6)  DD4HEP_HANDLE_XF_5(CODE,x1,x2,x3,x4,x5)
#define DD4HEP_HANDLE_XF_7(CODE,x1,x2,x3,x4,x5,x6,x7)            DD4HEP_IMPLEMENT_SAFE_CAST(x1,x7)  DD4HEP_HANDLE_XF_6(CODE,x1,x2,x3,x4,x5,x6)
#define DD4HEP_HANDLE_XF_8(CODE,x1,x2,x3,x4,x5,x6,x7,x8)         DD4HEP_IMPLEMENT_SAFE_CAST(x1,x8)  DD4HEP_HANDLE_XF_7(CODE,x1,x2,x3,x4,x5,x6,x7)
#define DD4HEP_HANDLE_XF_9(CODE,x1,x2,x3,x4,x5,x6,x7,x8,x9)      DD4HEP_IMPLEMENT_SAFE_CAST(x1,x9)  DD4HEP_HANDLE_XF_8(CODE,x1,x2,x3,x4,x5,x6,x7,x8)
#define DD4HEP_HANDLE_XF_10(CODE,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10) DD4HEP_IMPLEMENT_SAFE_CAST(x1,x10) DD4HEP_HANDLE_XF_9(CODE,x1,x2,x3,x4,x5,x6,x7,x8,x9)


#define DD4HEP_TEMPLATE_HANDLE(CODE,X) DD4HEP_TEMPLATE_HANDLE_CODE_##CODE(X) template class dd4hep::Handle<X>
#define DD4HEP_TEMPLATE_HANDLE_CODE_NONE(X)
#define DD4HEP_TEMPLATE_HANDLE_CODE_CAST_ONLY(X)       DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(X)
#define DD4HEP_TEMPLATE_HANDLE_CODE_NAMED(X)                            \
  namespace dd4hep {                                                    \
    template <> const char* Handle<X>::name() const			\
    { return this->m_element ? this->m_element->name.c_str() : ""; }	\
    template <> void							\
    Handle<X>::assign(X* p, const std::string& n, const std::string& t){\
      this->m_element = p;						\
      p->name = n;							\
      p->type = t;							\
    }}                                                                  \
    DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(X)

#define DD4HEP_TEMPLATE_HANDLE_CODE_UNNAMED(X)                          \
  namespace dd4hep {                                                    \
    template <> void		                                        \
    Handle<X>::assign(X* n, const std::string&, const std::string&)     \
    { this->m_element = n;}	                                        \
    template <> const char* Handle<X>::name() const { return ""; }	\
  }                                                                     \
  DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(X)

#define DD4HEP_TEMPLATE_HANDLE_CODE_RAW(X)                              \
  namespace dd4hep {                                                    \
    template <> void		                                        \
    Handle<X>::assign(X* n, const std::string&, const std::string&)     \
    { this->m_element = n;}	                                        \
    template <> const char* Handle<X>::name() const { return ""; }	\
  }                                                                     \
  DD4HEP_SAFE_CAST_IMPLEMENTATION(X,X)

// Delegation macro; this is the only macro you need to call from elsewhere:
#define DD4HEP_INSTANTIATE_HANDLE_CODE(CODE,...)  DD4HEP_HANDLE_XF(DD4HEP_HANDLE_PP_NARGS(__VA_ARGS__),CODE,__VA_ARGS__)
#define DD4HEP_INSTANTIATE_HANDLE(...)            DD4HEP_INSTANTIATE_HANDLE_CODE(CAST_ONLY,__VA_ARGS__)
#define DD4HEP_INSTANTIATE_SHAPE_HANDLE(...)      DD4HEP_INSTANTIATE_HANDLE_CODE(CAST_ONLY,__VA_ARGS__,TGeoShape,TGeoBBox)
#define DD4HEP_INSTANTIATE_HANDLE_RAW(...)        DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,__VA_ARGS__)
#define DD4HEP_INSTANTIATE_HANDLE_NAMED(...)      DD4HEP_INSTANTIATE_HANDLE_CODE(NAMED,__VA_ARGS__)
#define DD4HEP_INSTANTIATE_HANDLE_UNNAMED(...)    DD4HEP_INSTANTIATE_HANDLE_CODE(UNNAMED,__VA_ARGS__)

#define DD4HEP_CONCAT_MACROS(name, serial)  name##_##serial
#define DD4HEP_SEGMENTATION_HANDLE_IMPLEMENTATION(serial,name,...)                    \
  namespace {                                                                         \
  typedef dd4hep::SegmentationWrapper<name> DD4HEP_CONCAT_MACROS(Wrapper,serial); }   \
  DD4HEP_INSTANTIATE_HANDLE_CODE(UNNAMED,DD4HEP_CONCAT_MACROS(Wrapper,serial),__VA_ARGS__)

#define DD4HEP_INSTANTIATE_SEGMENTATION_HANDLE(...)                                   \
  DD4HEP_SEGMENTATION_HANDLE_IMPLEMENTATION(__LINE__,__VA_ARGS__,dd4hep::SegmentationObject)
