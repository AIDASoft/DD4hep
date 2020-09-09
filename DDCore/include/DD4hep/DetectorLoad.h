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
#ifndef DD4HEP_DETECTORLOAD_H
#define DD4HEP_DETECTORLOAD_H

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
  /** 
   *  This class is a view on the generic Detector implementation and deals with
   *  all issues parsing XML files.
   *
   *  The calls "fromXML" and "fromCompact" in the Detector implementation
   *  call this subclass.
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class DetectorLoad  {
  public:
    friend class Detector;

  protected:
    /// Reference to the Detector instance
    Detector* m_detDesc = 0;

  protected:
    /// Default constructor (protected, for sub-classes)
    DetectorLoad(Detector* description);

  public:
#ifdef G__ROOT
    /// No defautl constructor
    DetectorLoad() = default;
#else
    /// No defautl constructor
    DetectorLoad() = delete;
#endif
    /// No move constructor
    DetectorLoad(DetectorLoad&& copy) = delete;
    /// Default copy constructor
    DetectorLoad(const DetectorLoad& copy) = default;

    /// Default constructor (public, if used as a handle)
    DetectorLoad(Detector& description);
    /// Default destructor
    virtual ~DetectorLoad();
    /// Copy assignment constructor
    DetectorLoad& operator=(const DetectorLoad& copy) = default;
    /// No move assignment
    DetectorLoad& operator=(DetectorLoad&& copy) = delete;

    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const std::string& fname, xml::UriReader* entity_resolver=0);
    /// Process XML unit and adopt all data from source structure.
    virtual void processXML(const xml::Handle_t& base, const std::string& fname, xml::UriReader* entity_resolver=0);
    /// Process XML unit and adopt all data from source string in momory.
    virtual void processXMLString(const char* xmldata);
    /// Process XML unit and adopt all data from source string in momory.
    /** Subsequent parsers may use the entity resolver.
     */
    virtual void processXMLString(const char* xmldata, xml::UriReader* entity_resolver);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const std::string& msg_source, const xml::Handle_t& root);
    /// Process a given DOM (sub-) tree
    virtual void processXMLElement(const xml::Handle_t& root, DetectorBuildType type);
  };

}         /* End namespace dd4hep         */
#endif // DD4HEP_DETECTORLOAD_H
