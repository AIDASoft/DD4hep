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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the BasicGrammar include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_DDCORE_EXTENSIONENTRY_H
#define DD4HEP_DDCORE_EXTENSIONENTRY_H

#include "DD4hep/Primitives.h"

// C/C++ include files
#include <typeinfo>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Definition of the extension entry interface class
  /** Base class for the object extension mechanism.
   *
   *   \author  M.Frank
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  class ExtensionEntry {
  protected:
    /// Default constructor
    ExtensionEntry() = default;
    /// Copy constructor
    ExtensionEntry(const ExtensionEntry& copy) = default;
  public:
    /// Default destructor
    virtual ~ExtensionEntry() = default;
    /// Callback on invalid call invokation
    void invalidCall(const char* tag)  const;
    /// Virtual object accessor
    virtual void* object() const    = 0;
    /// Virtual object copy operator
    virtual void* copy(void*) const = 0;
    /// Virtual object destructor
    virtual void  destruct() const  = 0;
    /// Virtual entry clone function
    virtual ExtensionEntry* clone(void* arg)  const = 0;
    /// Hash value
    virtual unsigned long long int hash64()  const = 0;
  };

  namespace detail  {

    /// Implementation class for the object extension mechanism.
    /**  This implementation class supports the object extension mechanism
     *   for dd4hep. 
     *
     *   Note:
     *   The double-template implementation is necessary to support extensions
     *   using a virtual inheritance relationship between the interface and the
     *   concrete implementation of the extension object.
     *
     *   \author  M.Frank
     *   \date    13.08.2013
     *   \ingroup DD4HEP
     */
    template <typename Q,typename T> class SimpleExtension : public ExtensionEntry  {
    protected:
      T* ptr = 0;
      mutable Q* iface = 0;  //!
    public:
      /// Default constructor
      SimpleExtension() = delete;
      /// Initializing constructor
      SimpleExtension(T* p) : ptr(p) { iface = dynamic_cast<Q*>(p); }
      /// Copy constructor
      SimpleExtension(const SimpleExtension& copy) = default;
      /// Default destructor
      virtual ~SimpleExtension() = default;
      /// Assignment operator
      SimpleExtension& operator=(const SimpleExtension& copy) = default;
      /// Virtual object copy operator
      virtual void* copy(void*) const override { invalidCall("copy"); return 0;   }
      /// Virtual object destructor. Function may still be called without side-effects.
      virtual void  destruct()  const override {                                  }
      /// Virtual object accessor
      virtual void* object()    const override
      { return iface ? iface : (iface=dynamic_cast<Q*>(ptr));                     }
      /// Virtual entry clone function
      virtual ExtensionEntry* clone(void*)  const  override
      { invalidCall("clone"); return 0;                                           }
      /// Hash value
      virtual unsigned long long int hash64()  const override
      {  return detail::typeHash64<Q>();                                       }
    };
      
    /// Implementation class for the object extension mechanism.
    /**  This implementation class supports the object extension mechanism
     *   for dd4hep. It is ensured, that on the object destruction or
     *   on request the reference to the user object may be destructed.
     *
     *   Note: User object must be taken from the heap using "new".
     *   Note:
     *   The double-template implementation is necessary to support extensions
     *   using a virtual inheritance relationship between the interface and the
     *   concrete implementation of the extension object.
     *
     *   \author  M.Frank
     *   \date    13.08.2013
     *   \ingroup DD4HEP
     */
    template <typename Q,typename T> class DeleteExtension : public ExtensionEntry  {
    protected:
      T* ptr = 0;
      mutable Q* iface = 0;  //!
    public:
      /// Default constructor
      DeleteExtension() = delete;
      /// Initializing constructor
      DeleteExtension(T* p) : ptr(p)  { iface = dynamic_cast<Q*>(p); }
      /// Copy constructor
      DeleteExtension(const DeleteExtension& copy) = default;
      /// Default destructor
      virtual ~DeleteExtension() = default;
      /// Assignment operator
      DeleteExtension& operator=(const DeleteExtension& copy) = default;
      /// Virtual object copy operator
      virtual void* copy(void*)  const override  { invalidCall("copy"); return 0; }
      /// Virtual object destructor
      virtual void  destruct()   const override  { delete ptr;                    }
      /// Virtual object accessor
      virtual void* object()     const override
      { return iface ? iface : (iface=dynamic_cast<Q*>(ptr));                     }
      /// Virtual entry clone function
      virtual ExtensionEntry* clone(void* arg)  const  override
      {  return new DeleteExtension((T*)this->copy(arg));                         }
      /// Hash value
      virtual unsigned long long int hash64()  const override
      {  return detail::typeHash64<Q>();                                       }
    };

    /// Implementation class for the object extension mechanism.
    /**  This implementation class supports the object extension mechanism
     *   for dd4hep. It is ensured, that on the object destruction or
     *   on request the reference to the user object may be destructed.
     *
     *   Note: User object must be taken from the heap using "new".
     *   Note:
     *   The double-template implementation is necessary to support extensions
     *   using a virtual inheritance relationship between the interface and the
     *   concrete implementation of the extension object.
     *
     *   \author  M.Frank
     *   \date    13.08.2013
     *   \ingroup DD4HEP
     */
    template <typename Q,typename T> class CopyDeleteExtension : public ExtensionEntry  {
    protected:
      T* ptr = 0;
      mutable Q* iface = 0;  //!
    public:
      /// Default constructor
      CopyDeleteExtension() = delete;
      /// Initializing constructor
      CopyDeleteExtension(T* p) : ptr(p)  { iface = dynamic_cast<Q*>(p);          }
      /// Copy constructor
      CopyDeleteExtension(const CopyDeleteExtension& copy) = default;
      /// Default destructor
      virtual ~CopyDeleteExtension() = default;
      /// Assignment operator
      CopyDeleteExtension& operator=(const CopyDeleteExtension& copy) = default;
      /// Virtual object copy operator
      virtual void* copy(void*)  const override  { return new T(*ptr);            }
      /// Virtual object destructor
      virtual void  destruct()   const override  { delete ptr;                    }
      /// Virtual object accessor
      virtual void* object()     const override
      { return iface ? iface : (iface=dynamic_cast<Q*>(ptr));                     }
      /// Virtual entry clone function
      virtual ExtensionEntry* clone(void* arg)  const  override
      {  return new CopyDeleteExtension((T*)this->copy(arg));                     }
      /// Hash value
      virtual unsigned long long int hash64()  const override
      {  return detail::typeHash64<Q>();                                       }
    };
  }     // End namespace detail
}       // End namespace dd4hep
#endif  /* DD4HEP_DDCORE_EXTENSIONENTRY_H */
