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
#ifndef DD4HEP_GEOMETRY_OBJECTEXTENSIONS_H
#define DD4HEP_GEOMETRY_OBJECTEXTENSIONS_H

// C/C++ include files
#include <typeinfo>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Implementation of an object supporting arbitrary user extensions
  /**
   *  Usage by inheritance of the client supporting the functionality
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class ObjectExtensions   {
  public:
    /// Definition of the extension type
    typedef std::map<const std::type_info*, void*> Extensions;
    /// Extensions copy constructor type
    typedef void* (*copy_t)(const void*, void* arg);
    /// Extensions destructor type
    typedef void (*destruct_t)(void*);
    /// Defintiion of the extension entry
    struct Entry {
      copy_t copy;
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

  public:
    /// Default constructor
    ObjectExtensions(const std::type_info& parent_type);
    /// Copy constructor
    ObjectExtensions(const ObjectExtensions& copy) = delete;
    /// Default destructor
    virtual ~ObjectExtensions();
    /// Assignment operator
    ObjectExtensions& operator=(const ObjectExtensions& copy) = delete;
    /// Move extensions to target object
    void move(ObjectExtensions& copy);
    /// Clear all extensions
    void clear(bool destroy=true);
    /// Copy object extensions from another object. Hosting type must be identical!
    void copyFrom(const Extensions& ext, void* arg);
    /// Add an extension object to the detector element
    void* addExtension(void* ptr, const std::type_info& info, copy_t ctor, destruct_t dtor);
    /// Add an extension object to the detector element
    void* addExtension(void* ptr, const std::type_info& info, destruct_t dtor);
    /// Remove an existing extension object from the instance
    void* removeExtension(const std::type_info& info, bool destroy);
    /// Access an existing extension object from the detector element
    void* extension(const std::type_info& info, bool alert) const;
    /// Access an existing extension object from the detector element
    void* extension(const std::type_info& info) const;
  };

} /* End namespace DD4hep        */
#endif    /* DD4HEP_GEOMETRY_OBJECTEXTENSIONS_H */
