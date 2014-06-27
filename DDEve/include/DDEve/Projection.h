// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_PROJECTION_H
#define DD4HEP_DDEVE_PROJECTION_H

// Framework include files
#include "DDEve/View.h"

// Eve include files
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class Projection  Projection.h DDEve/Projection.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class Projection : public View  {
  protected:
    /// Reference to the projection manager
    TEveProjectionManager *m_projMgr;
    /// Reference to the projection axis
    TEveProjectionAxes* m_axis;

    /// Call an element to a event element list
    virtual TEveElement* ImportElement(TEveElement* el, TEveElementList* list);

  public:
    /// Initializing constructor
    Projection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~Projection();
    /// Map the projection view to the slot
    virtual View& Map(TEveWindow* slot);
    /// Add projection axis to the view
    virtual Projection& AddAxis();
    /// Create Rho-Phi projection 
    virtual Projection& CreateRhoPhiProjection();
    /// Create Rho-Z projection
    virtual Projection& CreateRhoZProjection();

    virtual void SetDepth(Float_t d);

    ClassDef(Projection,0);
  };
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_PROJECTION_H */
