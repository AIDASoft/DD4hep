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

// Predefined cast map for shapes
#define DD4HEP_IMPLEMENT_SAFE_SHAPE_CAST(FROM)                          \
  DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TGeoShape)                            \
  DD4HEP_IMPLEMENT_SAFE_CAST(FROM,TGeoBBox)

#define DD4HEP_INSTANTIATE_HANDLE(X)					\
  DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(X)                                   \
  template class dd4hep::Handle<X>

#define DD4HEP_INSTANTIATE_SHAPE_HANDLE(X)			        \
  DD4HEP_IMPLEMENT_SAFE_SHAPE_CAST(X)                                   \
  DD4HEP_INSTANTIATE_HANDLE(X)

#define DD4HEP_INSTANTIATE_HANDLE_NAMED(X)                              \
  DD4HEP_IMPLEMENT_SAFE_NAMED_CAST(X)                                   \
  namespace dd4hep {                                                    \
    template <> const char* Handle<X>::name() const			\
    { return this->m_element ? this->m_element->name.c_str() : ""; }	\
    template <> void							\
    Handle<X>::assign(X* p, const std::string& n, const std::string& t){\
      this->m_element = p;						\
      p->name = n;							\
      p->type = t;							\
    }}  								\
  template class dd4hep::Handle<X>

#define DD4HEP_INSTANTIATE_HANDLE_RAW(X)                                \
  namespace dd4hep {                                                    \
    template <> void		                                        \
    Handle<X>::assign(X* n, const std::string&, const std::string&)     \
    { this->m_element = n;}	                                        \
    template <> const char* Handle<X>::name() const { return ""; }	\
  }                                                                     \
  template class dd4hep::Handle<X>
  
#define DD4HEP_INSTANTIATE_HANDLE_UNNAMED(X)                            \
  namespace dd4hep {                                                    \
    template <> void		                                        \
    Handle<X>::assign(X* n, const std::string&, const std::string&)     \
    { this->m_element = n;}	                                        \
    template <> const char* Handle<X>::name() const { return ""; }	\
  }                                                                     \
  DD4HEP_INSTANTIATE_HANDLE(X)

#define DD4HEP_CONCAT_MACROS(name, serial)  name##_##serial
#define DD4HEP_SEGMENTATION_HANDLE_IMPLEMENTATION(X,serial)             \
  namespace {                                                           \
  typedef dd4hep::SegmentationWrapper<X> DD4HEP_CONCAT_MACROS(Wrapper,serial); }      \
  DD4HEP_IMPLEMENT_SAFE_CAST(DD4HEP_CONCAT_MACROS(Wrapper,serial),dd4hep::SegmentationObject) \
  DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DD4HEP_CONCAT_MACROS(Wrapper,serial))

#define DD4HEP_IMPLEMENT_SEGMENTATION_HANDLE(X) DD4HEP_SEGMENTATION_HANDLE_IMPLEMENTATION(X,__LINE__)
