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
#ifndef DD4HEP_DDEVE_CALO3DPROJECTION_H
#define DD4HEP_DDEVE_CALO3DPROJECTION_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/Display.h"

// Forward declarations
class TH2F;
class TEveCalo3D;
class TEveCaloDataHist;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// 3D projection for calorimeters
  /*
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class  Calo3DProjection : public View   {
  public:
    /// Initializing constructor
    Calo3DProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~Calo3DProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot)  override;

    /// Root implementation macro
    ClassDefOverride(Calo3DProjection,0);
  };

}      /* End namespace dd4hep            */
#endif /* DD4HEP_DDEVE_CALO3DPROJECTION_H */

