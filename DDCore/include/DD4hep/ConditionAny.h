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
#ifndef DD4HEP_CONDITIONANY_H
#define DD4HEP_CONDITIONANY_H

// Framework include files
#include "DD4hep/Conditions.h"

// C/C++ include files
#include <any>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Conditions internal namespace
  namespace detail  {
    class ConditionObject;
  }

  /// Main condition object handle.
  /**
   *  Convenience handle for specialized Condition objects
   *  with a concrete payload of std::any.
   *
   *  Note:
   *  Conditions may be shared between several DetElement objects.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class ConditionAny : public Handle<detail::ConditionObject> {
  public:
    /// Forward definition of the key type
    using key_type     = Condition::key_type;
    /// High part of the key identifies the detector element
    using detkey_type  = Condition::detkey_type;
    /// Low part of the key identifies the item identifier
    using itemkey_type = Condition::itemkey_type;
    /// Forward definition of the object properties
    using mask_type    = Condition::mask_type;

  private:
    /// Verify that underlying data are either invalid of contain an instance of std::any.
    void use_data(detail::ConditionObject* obj);

  public:

    /// Default constructor
    ConditionAny() = default;
    /// Move constructor
    ConditionAny(ConditionAny&& c) = default;
    /// Move constructor from Condition
    ConditionAny(Condition&& c);
    /// Copy constructor
    ConditionAny(const ConditionAny& c) = default;
    /// Copy constructor from Condition
    ConditionAny(const Condition& c);
    /// Initializing constructor
    ConditionAny(Object* p);
    /// Constructor from another handle
    template <typename Q> ConditionAny(const Handle<Q>& e);
    /// Initializing constructor for a initialized std::any payload
    ConditionAny(key_type hash_key);
    /// Initializing constructor for a initialized and filled std::any payload
    template <typename PAYLOAD> ConditionAny(key_type hash_key, PAYLOAD&& data);
    /// Initializing constructor for a initialized std::any payload
    ConditionAny(const std::string& name, const std::string& type);
    /// Initializing constructor for a initialized and filled std::any payload
    template <typename PAYLOAD> ConditionAny(const std::string& name, const std::string& type, PAYLOAD&& data);

    /// Assignment move operator
    template <typename Q> ConditionAny& operator=(Handle<Q>&& c);
    /// Assignment copy operator
    template <typename Q> ConditionAny& operator=(const Handle<Q>& c);
    /// Assignment move operator
    ConditionAny& operator=(Condition&& c);
    /// Assignment copy operator
    ConditionAny& operator=(const Condition& c);
    /// Assignment move operator
    ConditionAny& operator=(ConditionAny&& c) = default;
    /// Assignment copy operator
    ConditionAny& operator=(const ConditionAny& c) = default;

    /** Interval of validity            */
    /// Access the IOV type
    const IOVType& iovType()  const;
    /// Access the IOV block
    const IOV& iov()  const;

    /** Conditions identification using integer keys.   */
    /// Hash identifier
    key_type key()  const;
    /// DetElement part of the identifier
    detkey_type  detector_key()  const;
    /// Item part of the identifier
    itemkey_type item_key()  const;

    /// Flag operations: Get condition flags
    mask_type flags()  const;
    /// Flag operations: Set a conditons flag
    void setFlag(mask_type option);
    /// Flag operations: UN-Set a conditons flag
    void unFlag(mask_type option);
    /// Flag operations: Test for a given a conditons flag
    bool testFlag(mask_type option) const;

    /// Generic getter. Specify the exact type, not a polymorph type
    std::any& get();
    /// Generic getter (const version). Specify the exact type, not a polymorph type
    const std::any& get() const;

    /// Checks whether the object contains a value
    bool has_value()   const;
    /// Access to the type information
    const std::type_info& any_type() const;
    /// Access to the type information as string
    const std::string     any_type_name() const;
    /// Access the contained object inside std::any
    template <typename T> T& as();
    /// Access the contained object inside std::any
    template <typename T> const T& as() const;
    /// Access a copy of the contained object inside std::any
    template <typename T> T value();
    /// Access the contained object inside std::any
    template <typename T> T* pointer();
    /// Access the contained object inside std::any
    template <typename T> const T* pointer() const;
  };

  /// Move constructor from Condition
  inline ConditionAny::ConditionAny(Condition&& c) : Handle<detail::ConditionObject>() {
    this->use_data(c.ptr());
  }

  /// Copy constructor from Condition
  inline ConditionAny::ConditionAny(const Condition& c) : Handle<detail::ConditionObject>()  {
    this->use_data(c.ptr());
  }

  /// Construct conditions object and bind the data
  template <typename PAYLOAD> inline 
    ConditionAny::ConditionAny(key_type hash_key, PAYLOAD&& payload)   {
    ConditionAny c(hash_key);
    c.get() = std::move(payload);
    this->m_element = c.ptr();
  }

  /// Construct conditions object and bind the payload
  template <typename PAYLOAD> inline 
    ConditionAny::ConditionAny(const std::string& name, const std::string& type, PAYLOAD&& payload)   {
    ConditionAny c(name, type);
    c.get() = std::move(payload);
    this->m_element = c.ptr();
  }

  /// Assignment move operator
  template <typename Q> inline ConditionAny& ConditionAny::operator=(Handle<Q>&& c)  {
    this->use_data(c.ptr());
    return *this;
  }

  /// Assignment copy operator
  template <typename Q> inline ConditionAny& ConditionAny::operator=(const Handle<Q>& c)   {
    this->use_data(c.ptr());
    return *this;
  }

  /// Assignment move operator
  inline ConditionAny& ConditionAny::operator=(Condition&& c)  {
    this->use_data(c.ptr());
    return *this;
  }

  /// Assignment copy operator
  inline ConditionAny& ConditionAny::operator=(const Condition& c)   {
    this->use_data(c.ptr());
    return *this;
  }

  /// Access the contained object inside std::any
  template <typename T> inline T ConditionAny::value() {
    return std::any_cast<T>(this->get());
  }

  /// Access the contained object inside std::any
  template <typename T> inline T& ConditionAny::as() {
    T* ptr_payload = std::any_cast<T>(&this->get());
    if ( ptr_payload ) return *ptr_payload;
    throw std::runtime_error("ConditionAny: Cannot access value of std::any as a reference to "+typeName(typeid(T)));
  }

  /// Access the contained object inside std::any
  template <typename T> inline const T& ConditionAny::as() const {
    const T* ptr_payload = std::any_cast<T>(&this->get());
    if ( ptr_payload ) return *ptr_payload;
    throw std::runtime_error("ConditionAny: Cannot access value of std::any as a reference to "+typeName(typeid(T)));
  }

  /// Access the contained object inside std::any
  template <typename T> inline T* ConditionAny::pointer() {
    return isValid() ? std::any_cast<T>(&this->get()) : nullptr;
  }

  /// Access the contained object inside std::any
  template <typename T> inline const T* ConditionAny::pointer() const {
    return isValid() ? std::any_cast<const T>(&this->get()) : nullptr;
  }

}          /* End namespace dd4hep                   */
#endif // DD4HEP_CONDITIONANY_H
