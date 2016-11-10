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
#ifndef DD4HEP_DDCORE_DD4HEPUI_H
#define DD4HEP_DDCORE_DD4HEPUI_H

// Framework includes
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// ROOT interactive UI for DD4hep applications
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class DD4hepUI  {
  protected:
    /// Reference to the LCDD instance object
    Geometry::LCDD& m_lcdd;
    Handle<NamedObject> m_condMgr;
    Handle<NamedObject> m_alignMgr;
    
  public:
    /// Default constructor
    DD4hepUI(Geometry::LCDD& instance);
    /// Default destructor
    virtual ~DD4hepUI();
    /// Access to the LCDD instance
    Geometry::LCDD* instance()  const;

    /// Install the DD4hep conditions manager object
    Handle<NamedObject> conditionsMgr()  const;
    /// Load conditions from file
    long loadConditions(const std::string& fname)  const;
    
    /// Install the DD4hep alignment manager object
    Handle<NamedObject> alignmentMgr()  const;
    
    /// LCDD interface: Manipulate geometry using facroy converter
    virtual long apply(const char* factory, int argc, char** argv) const;
    /// LCDD interface: Read any geometry description or alignment file
    virtual void fromXML(const std::string& fname, LCDDBuildType type = BUILD_DEFAULT) const;
    /// LCDD interface: Re-draw the entire scene
    virtual void redraw() const;
  };
  
} /* End namespace DD4hep        */
#endif // DD4HEP_DDCORE_DD4HEPUI_H
