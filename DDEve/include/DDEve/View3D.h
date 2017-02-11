//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//==========================================================================
#ifndef DD4HEP_DDEVE_VIEW3D_H
#define DD4HEP_DDEVE_VIEW3D_H

// Framework include files
#include "DDEve/View.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// class View3D  View3D.h DDEve/View3D.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class View3D : public View  {
  protected:

  public:
    /// Initializing constructor
    View3D(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~View3D();
    /// Build the 3d view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;

    /// Root implementation macro
    ClassDefOverride(View3D,0);
  };
}      /* End namespace DD4hep   */
#endif /* DD4HEP_DDEVE_VIEW3D_H  */
