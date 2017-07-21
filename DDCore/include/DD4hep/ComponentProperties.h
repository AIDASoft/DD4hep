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
#ifndef DD4HEP_DDG4_COMPONENTPROPERTIES_H
#define DD4HEP_DDG4_COMPONENTPROPERTIES_H

// Framework include files

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class Property;
  class BasicGrammar;
  class PropertyGrammar;

  /// Interface class to configure properties in components
  /**
   *  Placeholder interface.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class PropertyConfigurator {
  protected:
    /// Default destructor
    virtual ~PropertyConfigurator();
  public:
    virtual void set(const PropertyGrammar& setter, const std::string&, const std::string&, void* ptr) const = 0;
  };


  /// Class describing the grammar representation of a given data type
  /**
   *  Note: This class cannot be saved to a ROOT file!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class PropertyGrammar {
  protected:
    friend class Property;
    const BasicGrammar& m_grammar;  //! This member is not ROOT persistent as the entire class is not.
  public:
    /// Default constructor
    PropertyGrammar(const BasicGrammar& g);
    /// Default destructor
    virtual ~PropertyGrammar();
    /// Error callback on invalid conversion
    static void invalidConversion(const std::type_info& from, const std::type_info& to);
    /// Error callback on invalid conversion
    static void invalidConversion(const std::string& value, const std::type_info& to);
    /// Access to the type information
    virtual const std::type_info& type() const;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const;
  };

  /// The property class to assign options to actions.
  /**
   *   Standard implementation of a property mechanism.
   *   The data conversion mechanism between various properties
   *   uses internally boost::spirit to allow also conversions
   *   between types, which are initially unrelated such as
   *   e.g. vector<int> and list<short>.
   *
   *  Note: This class cannot be saved to a ROOT file!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class Property {
  protected:
    /// Pointer to the data location
    void* m_par = 0;
    /// Reference to the grammar of this property (extended type description)
    const PropertyGrammar* m_hdl = 0;

    /// Setup property
    template <typename TYPE> void make(TYPE& value);
  public:
    /// Default constructor
    Property() = default;
    /// Copy constructor
    Property(const Property& p) = default;
    /// User constructor
    template <typename TYPE> Property(TYPE& val) : m_par(0), m_hdl(0) {
      make(val);
    }
    /// Property type name
    static std::string type(const Property& proptery);
    /// Property type name
    static std::string type(const std::type_info& proptery);
    /// Access void data pointer
    void* ptr() const {      return m_par;    }
    /// Property type name
    std::string type() const;
    /// Access grammar object
    const PropertyGrammar& grammar() const;
    /// Conversion to string value
    std::string str() const;
    /// Conversion from string value
    Property& str(const std::string& input);
    /// Conversion from string value
    const Property& str(const std::string& input)  const;
    /// Assignment operator
    Property& operator=(const Property& p) = default;
    /// Assignment operator / set new balue
    Property& operator=(const char* val);
    /// Assignment operator / set new balue
    //Property& operator=(const std::string& val);
    /// Assignment operator / set new balue
    template <typename TYPE> Property& operator=(const TYPE& val);
    /// Retrieve value
    template <typename TYPE> TYPE value() const;
    /// Retrieve value from stack (large values e.g. vectors etc.)
    template <typename TYPE> void value(TYPE& value) const;
    /// Set value of this property
    template <typename TYPE> void set(const TYPE& value);
  };

  /// Concrete template instantiation of a combined property value pair.
  /**
   *  Note: This class cannot be saved to a ROOT file!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  template <class TYPE> class PropertyValue : private Property {
  public:
    TYPE data;
    /// Default constructor
    PropertyValue() : Property(data) {}
    /// Copy constructor
    PropertyValue(const PropertyValue& c) = default;
    /// Assignment operator
    PropertyValue& operator=(const PropertyValue& c) = default;
    /// Assignment operator
    PropertyValue& operator=(const TYPE& val) { data = val; return *this;      }
    /// Equality operator
    bool operator==(const TYPE& val) const { return val == data;               }
    /// Access grammar object
    const PropertyGrammar& grammar() const { return this->Property::grammar(); }
    /// Conversion to string value
    std::string str() const                { return this->Property::str();     }
    /// Retrieve value with data conversion
    template <typename T> T value() const  { return this->Property::value<T>();}
    /// Retrieve value from stack with data conversion (large values e.g. vectors etc.)
    template <typename T>
    void value(TYPE& val) const            { this->Property::value(val);       }
    /// Set value of this property with data conversion
    template <typename T> void set(const T& val)  { this->Property::set(val);  }
  };

  /// Manager to ease the handling of groups of properties.
  /**
   *  Note: This class cannot be saved to a ROOT file!
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class PropertyManager {
  public:
    /// Property array definition
    typedef std::map<std::string, Property> Properties;
  protected:
    /// Property array/map
    Properties m_properties;

    /// Verify that this property does not exist (throw exception if the name was found)
    void verifyNonExistence(const std::string& name) const;
    /// Verify that this property exists (throw exception if the name was not found)
    Properties::iterator verifyExistence(const std::string& name);
    /// Verify that this property exists (throw exception if the name was not found)
    Properties::const_iterator verifyExistence(const std::string& name) const;

  public:
    /// Default constructor
    PropertyManager();
    /// Default destructor
    virtual ~PropertyManager();
    /// Access total number of properties
    size_t size()  const;
    /// Check for existence
    bool exists(const std::string& name) const;
    /// Access property by name (CONST)
    const Property& property(const std::string& name) const;
    /// Access property by name
    Property& property(const std::string& name);
    /// Access property by name
    Property& operator[](const std::string& name);
    /// Access property by name
    const Property& operator[](const std::string& name) const;
    /// Add a new property
    void add(const std::string& name, const Property& property);
    /// Add a new property
    template <typename T> void add(const std::string& name, T& value)   {
      add(name, Property(value));
    }
    /// Bulk set of all properties
    void set(const std::string& component_name, PropertyConfigurator& setup);
    /// Apply functor on properties
    template <typename FUNCTOR> void for_each(FUNCTOR& func)   {
      std::for_each(m_properties.begin(), m_properties.end(), func);
    }
    /// Apply functor on properties
    template <typename FUNCTOR> void for_each(const FUNCTOR& func)   {
      std::for_each(m_properties.begin(), m_properties.end(), func);
    }
    /// Export properties of another instance
    void adopt(const PropertyManager& copy);
    /// Dump string values
    void dump() const;
  };

  /// Property object as base class for all objects supporting properties
  /** 
   *  Note: This class cannot be saved to a ROOT file!
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class PropertyConfigurable  {
  protected:
    /// Property pool
    PropertyManager m_properties;

  public:
    /// Standard constructor
    PropertyConfigurable();
    /// Default destructor
    virtual ~PropertyConfigurable();
    /// Access to the properties of the object
    PropertyManager& properties() {
      return m_properties;
    }
    /// Check property for existence
    bool hasProperty(const std::string& name) const;
    /// Access single property
    Property& property(const std::string& name);
    /// Declare property
    template <typename T> void declareProperty(const std::string& nam, T& val);
    /// Declare property
    template <typename T> void declareProperty(const char* nam, T& val);
  };

  /// Declare property
  template <typename T> 
  void PropertyConfigurable::declareProperty(const std::string& nam, T& val) {
    m_properties.add(nam, val);
  }

  /// Declare property
  template <typename T> 
  void PropertyConfigurable::declareProperty(const char* nam, T& val) {
    m_properties.add(nam, val);
  }

}      // End namespace dd4hep
#endif // DD4HEP_DDG4_COMPONENTPROPERTIES_H
