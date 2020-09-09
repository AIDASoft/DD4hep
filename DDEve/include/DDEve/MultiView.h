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
//
//==========================================================================
#ifndef DDEVE_MULTIVIEW_H
#define DDEVE_MULTIVIEW_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/DisplayConfiguration.h"

// Forward declarations
class TEveWindowPack;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// class MultiView  MultiView.h DDEve/MultiView.h
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class MultiView : public View  {
  public:
    /// Initializing constructor
    MultiView(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~MultiView();
    /// Build the 3d view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;
    /// Root implementation macro
    ClassDefOverride(MultiView,0);
  };
}      /* End namespace dd4hep     */
#endif // DDEVE_MULTIVIEW_H
