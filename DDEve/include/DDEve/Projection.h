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
#ifndef DD4HEP_DDEVE_PROJECTION_H
#define DD4HEP_DDEVE_PROJECTION_H

// Framework include files
#include "DDEve/View.h"

// Eve include files
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// class Projection  Projection.h DDEve/Projection.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class Projection : public View  {
  protected:
    /// Reference to the projection manager
    TEveProjectionManager *m_projMgr;
    /// Reference to the projection axis
    TEveProjectionAxes* m_axis;

    /// Call an element to a event element list
    virtual TEveElement* ImportElement(TEveElement* el, TEveElementList* list);
    /// Call an element to a geometry element list
    virtual TEveElement* ImportGeoElement(TEveElement* element, TEveElementList* list)  override;
    /// Call an element to a geometry element list
    virtual TEveElement* ImportGeoTopic(TEveElement* element, TEveElementList* list) override;
    /// Call an element to a event element list
    virtual TEveElement* ImportEventElement(TEveElement* element, TEveElementList* list)  override;


  public:
    /// Initializing constructor
    Projection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~Projection();
    /// Map the projection view to the slot
    virtual View& Map(TEveWindow* slot)  override;
    /// Add projection axis to the view
    virtual Projection& AddAxis();
    /// Create Rho-Phi projection 
    virtual Projection& CreateRhoPhiProjection();
    /// Create Rho-Z projection
    virtual Projection& CreateRhoZProjection();

    virtual void SetDepth(Float_t d);

    ClassDefOverride(Projection,0);
  };
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDEVE_PROJECTION_H */
