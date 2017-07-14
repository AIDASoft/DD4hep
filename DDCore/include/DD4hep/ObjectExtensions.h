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
#ifndef DD4HEP_DDCORE_OBJECTEXTENSIONS_H
#define DD4HEP_DDCORE_OBJECTEXTENSIONS_H

// Framework include files
#include "DD4hep/ExtensionEntry.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Implementation of an object supporting arbitrary user extensions
  /**
   *  Usage by inheritance of the client supporting the functionality
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class ObjectExtensions   {
  public:
    /// The extensions object
    std::map<unsigned long long int, ExtensionEntry*>    extensions;   //!

  public:
    /// Default constructor
    ObjectExtensions(const std::type_info& parent_type);
    /// Copy constructor
    ObjectExtensions(const ObjectExtensions& copy) = delete;
    /// Default destructor
    virtual ~ObjectExtensions();
    /// Assignment operator
    ObjectExtensions& operator=(const ObjectExtensions& copy) = delete;
    /// Initialize non-persistent object containers (e.g. after loading from ROOT file)
    void initialize();
    /// Move extensions to target object
    void move(ObjectExtensions& copy);
    /// Clear all extensions
    void clear(bool destroy=true);
    /// Copy object extensions from another object. Hosting type must be identical!
    void copyFrom(const std::map<unsigned long long int,ExtensionEntry*>& ext, void* arg);
    /// Add an extension object to the detector element
    void* addExtension(unsigned long long int key, ExtensionEntry* entry);
    /// Remove an existing extension object from the instance
    void* removeExtension(unsigned long long int key, bool destroy);
    /// Access an existing extension object from the detector element
    void* extension(unsigned long long int key, bool alert) const;
    /// Access an existing extension object from the detector element
    void* extension(unsigned long long int key) const;
  };

} /* End namespace dd4hep        */
#endif    /* DD4HEP_DDCORE_OBJECTEXTENSIONS_H */
