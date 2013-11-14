// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_COMPONENTPROPERTIES_H
#define DD4HEP_DDG4_COMPONENTPROPERTIES_H

// Framework include files
#include "DDG4/ComponentUtils.h"

// C/C++ include files
#include <algorithm>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <map>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  struct Property;
  struct PropertyGrammar;
  struct PropertyConfigurator {
  protected:
    virtual ~PropertyConfigurator();
  public:
    virtual void set(const PropertyGrammar& setter, const std::string&, const std::string&, void* ptr) const = 0;
  };

  /** @class PropertyGrammar
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  struct PropertyGrammar {
  protected:
    friend class Property;

    /// Default constructor
    PropertyGrammar();
    /// Default destructor
    virtual ~PropertyGrammar();
    /// Error callback on invalid conversion
    static void invalidConversion(const std::type_info& from, const std::type_info& to);
    /// Access to the type information
    virtual const std::type_info& type() const = 0;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const = 0;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const = 0;
  };

  /** @class Property
   *
   *   Standard implementation of a property mechanism.
   *   The data conversion mechanism between various properties
   *   uses internally boost::spirit to allow also conversions
   *   between types, which are initially unrelated such as
   *   e.g. vector<int> and list<short>.
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  class Property {
  protected:
    /// Pointer to the data location
    void* m_par;
    const PropertyGrammar* m_hdl;

    /// Setup property
    template <typename TYPE> void make(TYPE& value);
  public:
    /// Default constructor
    Property();
    /// Copy constructor
    Property(const Property& p);
    /// User constructor
    template <typename TYPE> Property(TYPE& val)
        : m_par(0), m_hdl(0) {
      make(val);
    }
    /// Property type name
    static std::string type(const Property& proptery);
    /// Property type name
    static std::string type(const std::type_info& proptery);
    /// Property type name
    std::string type() const;
    /// Access void data pointer
    void* ptr() const {
      return m_par;
    }
    /// Access grammar object
    const PropertyGrammar& grammar() const;
    /// Conversion to string value
    std::string str() const;
    /// Conversion from string value
    Property& str(const std::string& input);
    /// Assignment operator
    Property& operator=(const Property& p);
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

  /** @class PropertyValue
   *
   *   @author  M.Frank
   *   @date    13.08.2013
   */
  template <class TYPE> class PropertyValue : private Property {
  public:
    TYPE data;
    PropertyValue()
        : Property(data) {
    }
    /// Assignment operator
    PropertyValue& operator=(const TYPE& val) {
      data = val;
      return *this;
    }
    // Equality operator
    bool operator==(const TYPE& val) const {
      return val == data;
    }
    /// Access grammar object
    const PropertyGrammar& grammar() const {
      return this->Property::grammar();
    }
    /// Conversion to string value
    std::string str() const {
      return this->Property::str();
    }
    /// Retrieve value with data conversion
    template <typename T> T value() const {
      return this->Property::value<T>();
    }
    /// Retrieve value from stack with data conversion (large values e.g. vectors etc.)
    template <typename T> void value(TYPE& val) const {
      this->Property::value(val);
    }
    /// Set value of this property with data conversion
    template <typename T> void set(const T& val) {
      this->Property::set(val);
    }
  };

  /** @class PropertyManager
   *
   *   @author  M.Frank
   *   @date    13.08.2013
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
    template <typename T> void add(const std::string& name, T& value) {
      add(name, Property(value));
    }
    /// Bulk set of all properties
    void set(const std::string& component_name, PropertyConfigurator& setup);
    /// Apply functor on properties
    template <typename FUNCTOR> void for_each(FUNCTOR& func) {
      std::for_each(m_properties.begin(), m_properties.end(), func);
    }
    /// Dump string values
    void dump() const;
  };

}      // End namespace DD4hep

#endif // DD4HEP_DDG4_COMPONENTPROPERTIES_H
