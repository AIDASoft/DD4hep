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
#ifndef DD4HEP_DDCORE_DETECTORLOAD_H
#define DD4HEP_DDCORE_DETECTORLOAD_H

// Framework includes
#include "DD4hep/Detector.h"

// C/C++ include files
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml  { 
    class Handle_t;
    class UriReader;
  }

  class Detector;

  /// Data implementation class of the Detector interface
  /** @class DetectorLoad   DetectorLoad.h  dd4hep/DetectorLoad.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DetectorLoad  {
  public:
    friend class Detector;

  protected:
    /// Reference to the Detector instance
    Detector* m_detDesc;
    /// Default constructor
    DetectorLoad(Detector* description);
    /// Default destructor
    virtual ~DetectorLoad();

  public:
    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const std::string& fname, xml::UriReader* entity_resolver=0);
    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const xml::Handle_t& base, const std::string& fname, xml::UriReader* entity_resolver=0);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const std::string& msg_source, const xml::Handle_t& root);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const xml::Handle_t& root, DetectorBuildType type);
  };

}         /* End namespace dd4hep         */
#endif    /* DD4HEP_DDCORE_DETECTORLOAD_H */
