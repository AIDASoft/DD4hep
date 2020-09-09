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
// Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DDEVE_CALOLEGO_H
#define DDEVE_CALOLEGO_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/Display.h"

// Forward declarations
class TH2F;
class TEveCalo3D;
class TEveCaloDataHist;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Lego plot for calorimeter energy deposits
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class  CaloLego : public View   {
    Display::CalodataContext m_data;
  public:
    /// Initializing constructor
    CaloLego(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~CaloLego();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;
    /// Configure a single geometry view
    virtual void ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)  override;
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config)  override;
    /// Call to import geometry topics
    void ImportGeoTopics(const std::string& title)  override;

    /// Root implementation macro
    ClassDefOverride(CaloLego,0);
  };
}      /* End namespace dd4hep    */
#endif // DDEVE_CALOLEGO_H
