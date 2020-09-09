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
#ifndef DDEVE_RHOZPROJECTION_H
#define DDEVE_RHOZPROJECTION_H

// Framework include files
#include "DDEve/Projection.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// class RhoZProjection  RhoZProjection.h DDEve/RhoZProjection.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class  RhoZProjection : public Projection   {
  public:
    /// Initializing constructor
    RhoZProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~RhoZProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;
    /// Root implementation macro
    ClassDefOverride(RhoZProjection,0);
  };
}      /* End namespace dd4hep          */
#endif // DDEVE_RHOZPROJECTION_H

