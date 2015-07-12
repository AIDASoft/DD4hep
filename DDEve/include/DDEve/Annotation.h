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
// Author     : M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDEVE_ANNOTATION_H
#define DD4HEP_DDEVE_ANNOTATION_H

// Framework include files
#include "TGLAnnotation.h"

// Forward declarations
class TEveViewer;
class TGLOvlSelectRecord;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Class to add annotations to eve viewers
  /** 
   * Implements slightly larger text size and one line constructor
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class Annotation : public TGLAnnotation   {
  public:
    /// Standard constructor with initialization
    Annotation(TEveViewer* v, const std::string& text, float x, float y, Color_t c);
    /// Default destructor
    virtual ~Annotation();
    /// Mouse has entered overlay area.
    virtual Bool_t MouseEnter(TGLOvlSelectRecord& selRec);
    /// Default text size
    static float DefaultTextSize();
    /// Default margin for placement in bottom left corner
    static float DefaultMargin();
  };

}      /* End namespace DD4hep   */
#endif /* DD4HEP_DDEVE_ANNOTATION_H */

