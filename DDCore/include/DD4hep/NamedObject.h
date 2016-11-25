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

#ifndef DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H
#define DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Implementation of a named object
  /**
   *  The data class behind named object handles.
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_GEOMETRY
   */
  class NamedObject {
  public:
    /// The object name
    std::string name;
    /// The object type
    std::string type;

    /// Initializing constructor
    NamedObject(const char* nam, const char* typ="");
    /// Initializing constructor
    NamedObject(const std::string& nam);
    /// Initializing constructor
    NamedObject(const std::string& nam, const std::string& typ);

    /// Standard constructor
    NamedObject() = default;
    /// Copy constructor
    NamedObject(const NamedObject& c) = default;
    /// Default destructor
    virtual ~NamedObject() = default;
    /// Assignment operator
    NamedObject& operator=(const NamedObject& c) = default;

    /// Access name
    const char* GetName()  const  {
      return name.c_str();
    }
    /// Set name (used by Handle)
    void SetName(const char* nam)  {
      name = nam;
    }
    /// Set Title (used by Handle)
    void SetTitle(const char* tit)  {
      type = tit;
    }
    /// Get name (used by Handle)
    const char* GetTitle() const  {
      return type.c_str();
    }
  };

} /* End namespace DD4hep                   */
#endif    /*  DD4HEP_GEOMETRY_INTERNAL_NAMEDOBJECT_H   */
