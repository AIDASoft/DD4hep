// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDEVE_RHOZPROJECTION_H
#define DD4HEP_DDEVE_RHOZPROJECTION_H

// Framework include files
#include "DDEve/Projection.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

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
    virtual View& Build(TEveWindow* slot);
    /// Root implementation macro
    ClassDef(RhoZProjection,0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_RHOZPROJECTION_H */

