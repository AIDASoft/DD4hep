//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-02-02
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_PLUGINTESTER_H 
#define DD4HEP_PLUGINTESTER_H 1

// Framework includes

// C/C++ include files
#include <typeinfo>
#include <string>
#include <map>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  class PluginTester  {
  public:
    /// Definition of the extension type
    typedef std::pair<const std::type_info*,std::string> key_type;
    typedef std::map<key_type, void*> Extensions;
    /// Extensions destructor type
    typedef void (*destruct_t)(void*);
    /// Defintiion of the extension entry
    struct Entry {
      destruct_t destruct;
      int id;
    };
    typedef std::map<const std::type_info*, Entry> ExtensionMap;

    /// The extensions object
    Extensions    extensions; //!
    /// Pointer to the extension map
    ExtensionMap* extensionMap; //!

    /// Function to be passed as dtor if object should NOT be deleted!
    static void _noDelete(void*) {}

    /// Templated destructor function
    template <typename T> static void _delete(void* ptr) {
      delete (T*) (ptr);
    }
    
    /// Add an extension object to the detector element
    void* addExtension(void* ptr, const std::string& name, const std::type_info& info, destruct_t dtor);
    /// Access an existing extension object from the detector element
    void* extension(const std::string& name, const std::type_info& info, bool alert) const;
    /// Remove an existing extension object from the instance
    void* removeExtension(const std::string& name, const std::type_info& info, bool destroy);

  public:
    /// Default constructor
    PluginTester();
    /// Copy constructor
    PluginTester(const PluginTester& copy) = delete;
    /// Default destructor
    virtual ~PluginTester();
    /// Assignment operator
    PluginTester& operator=(const PluginTester& copy) = delete;
    /// Clear all extensions
    void clear(bool destroy=true);
    template<typename Q> Q* addExtension(Q* ptr, const std::string& name)  {
      return (Q*)addExtension(ptr, name, typeid(Q), _delete<Q>);
    }
    template<typename Q> Q* extension(const std::string& name, bool alert=true)  {
      return (Q*)extension(name, typeid(Q), alert);
    }
    template<typename Q> Q* removeExtension(const std::string& name, bool destroy=true)  {
      return (Q*)removeExtension(name, typeid(Q), destroy);
    }
  };
  
} /* End namespace dd4hep             */
#endif  // DD4HEP_PLUGINTESTER_H
