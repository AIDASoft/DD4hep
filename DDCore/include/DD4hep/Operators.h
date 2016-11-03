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

#ifndef DD4HEP_DD4HEP_OPERATORS_H
#define DD4HEP_DD4HEP_OPERATORS_H

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Helper functor to select elements by name (using name() member method)
  template <typename T> class ByName {
  private:
    /// Assignment operator
    template <typename Q> ByName<T>& operator=(const ByName<Q>& /* copy */)  { return *this; }
  public:
    /// Reference name
    const std::string& name;
    /// Standard constructor
    ByName(const std::string& nam) : name(nam) {}
    /// Copy constructor
    ByName(const ByName& copy) : name(copy.name) {}
    /// Operator to select from object containers
    bool operator()(const T& entry)  const  {  return entry.name()  == name;  }
    /// Operator to select from pointer containers
    bool operator()(const T* entry)  const  {  return entry->name() == name;  }
  };
  template <typename T> ByName<T> byName(const std::string& n) { return ByName<T>(n); }
  template <typename T> ByName<T> byName(const T* o) { return ByName<T>(o->name()); }
  template <typename T> ByName<T> byName(const T& o) { return ByName<T>(o.name()); }

  /// Helper functor to select elements by name (using name member variable)
  template <typename T> class ByNameAttr {
  private:
    /// Assignment operator
    template <typename Q> ByNameAttr<T>& operator=(const ByNameAttr<Q>& /* copy */)  { return *this; }
  public:
    /// Reference name
    const std::string& name;
    /// Standard constructor
    ByNameAttr(const std::string& nam) : name(nam) {}
    /// Copy constructor
    ByNameAttr(const ByNameAttr& copy) : name(copy.name) {}
    /// Operator to select from object containers
    bool operator()(const T& entry)  const  {  return entry.name  == name;  }
    /// Operator to select from pointer containers
    bool operator()(const T* entry)  const  {  return entry->name == name;  }
  };

  template <typename T> ByNameAttr<T> byNameAttr(const std::string& nam) { return ByNameAttr<T>(nam); }
  template <typename T> ByNameAttr<T> byNameAttr(const T* o) { return ByNameAttr<T>(o->name); }
  template <typename T> ByNameAttr<T> byNameAttr(const T& o) { return ByNameAttr<T>(o.name);  }

}      // End namespace DD4hep
#endif // DD4HEP_DD4HEP_OPERATORS_H
