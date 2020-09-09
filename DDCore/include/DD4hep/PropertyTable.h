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
#ifndef DD4HEP_PROPERTYTABLE_H
#define DD4HEP_PROPERTYTABLE_H

#include "RVersion.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,17,0)

// Framework include files
#include "DD4hep/Handle.h"

// ROOT include files
#include "TGDMLMatrix.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Class to support the handling of optical surfaces.
  /**
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class PropertyTable : public Handle<TGDMLMatrix> {
  public:
    typedef TGDMLMatrix Object;
    
  public:
    /// Default constructor
    PropertyTable() = default;
    /// Copy constructor
    PropertyTable(const PropertyTable& e) = default;
    /// Constructor from same-type handle
    PropertyTable(const Handle<Object>& e) : Handle<Object>(e) {  }
    /// Constructor from arbitray handle
    template <typename Q>
    PropertyTable(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor using object pointer
    PropertyTable(Object* obj) : Handle<Object>(obj) { }
    /// Initializing constructor.
    PropertyTable(Detector& description,
                  const std::string& table_name,
                  const std::string& property_name,
                  size_t             num_rows,
                  size_t             num_cols);

    /// Assignment operator
    PropertyTable& operator=(const PropertyTable& m) = default;
  };

}         /* End namespace dd4hep              */
#endif    /* ROOT_VERSION                      */
#endif // DD4HEP_PROPERTYTABLE_H
