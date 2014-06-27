// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_RHOPHIPROJECTION_H
#define DD4HEP_DDEVE_RHOPHIPROJECTION_H

// Framework include files
#include "DDEve/Projection.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class RhoPhiProjection  RhoPhiProjection.h DDEve/RhoPhiProjection.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class  RhoPhiProjection : public Projection   {
  public:
    /// Initializing constructor
    RhoPhiProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~RhoPhiProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Root implementation macro
    ClassDef(RhoPhiProjection,0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_RHOPHIPROJECTION_H */

