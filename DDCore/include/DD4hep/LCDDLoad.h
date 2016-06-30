// $Id: $
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
#ifndef DD4HEP_DDCORE_LCDDLOAD_H
#define DD4HEP_DDCORE_LCDDLOAD_H

// Framework includes
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML  { 
    class Handle_t;
    class UriReader;
  }

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {
    class LCDD;
  }

  /// Data implementation class of the LCDD interface
  /** @class LCDDLoad   LCDDLoad.h  DD4hep/LCDDLoad.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class LCDDLoad  {
  public:
    friend class Geometry::LCDD;

  protected:
    /// Reference to the LCDD instance
    Geometry::LCDD* m_lcdd;
    /// Default constructor
    LCDDLoad(Geometry::LCDD* lcdd);
    /// Default destructor
    virtual ~LCDDLoad();

  public:
    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const std::string& fname, XML::UriReader* entity_resolver=0);
    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const XML::Handle_t& base, const std::string& fname, XML::UriReader* entity_resolver=0);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const std::string& msg_source, const XML::Handle_t& root);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const XML::Handle_t& root, LCDDBuildType type);

  };

}         /* End namespace DD4hep     */
#endif    /* DD4HEP_DDCORE_LCDDLOAD_H */
