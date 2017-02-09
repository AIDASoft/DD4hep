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
// Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================
#ifndef DD4HEP_DDEVE_CALO2DPROJECTION_H
#define DD4HEP_DDEVE_CALO2DPROJECTION_H

// Framework include files
#include "DDEve/Projection.h"
#include "DDEve/Display.h"

// Forward declarations
class TH2F;
class TEveCalo3D;
class TEveCaloDataHist;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// 2D projection operator for calorimeters
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class  Calo2DProjection : public Projection   {
  public:
    /// Initializing constructor
    Calo2DProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~Calo2DProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;
    /// Configure a single geometry view
    virtual void ConfigureGeometry(const DisplayConfiguration::ViewConfig& config)  override;
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config)  override;
    /// Call to import geometry topics
    void ImportGeoTopics(const std::string& title)  override;

    /// Root implementation macro
    ClassDefOverride(Calo2DProjection,0);
  };

}      /* End namespace DD4hep   */
#endif /* DD4HEP_DDEVE_CALO2DPROJECTION_H */
