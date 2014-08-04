// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_ANNOTATION_H
#define DD4HEP_DDEVE_ANNOTATION_H

// Framework include files
#include "TGLAnnotation.h"

// Forward declarations
class TEveViewer;
class TGLOvlSelectRecord;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /// Class to add annotations to eve viewers
  /** 
   * Implements slightly larger text size and one line constructor
   *
   * @author  M.Frank
   * @version 1.0
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

