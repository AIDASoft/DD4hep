//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_RHOPHIPROJECTION_H
#define DD4HEP_DDEVE_RHOPHIPROJECTION_H

// Framework include files
#include "DDEve/Projection.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// class RhoPhiProjection  RhoPhiProjection.h DDEve/RhoPhiProjection.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class  RhoPhiProjection : public Projection   {
  public:
    /// Initializing constructor
    RhoPhiProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~RhoPhiProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;
    /// Root implementation macro
    ClassDefOverride(RhoPhiProjection,0);
  };
}      /* End namespace dd4hep            */
#endif /* DD4HEP_DDEVE_RHOPHIPROJECTION_H */

