// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_VIEW3D_H
#define DD4HEP_DDEVE_VIEW3D_H

// Framework include files
#include "DDEve/View.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class View3D  View3D.h DDEve/View3D.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class View3D : public View  {
  protected:

  public:
    /// Initializing constructor
    View3D(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~View3D();
    /// Build the 3d view and map it to the given slot
    virtual View& Build(TEveWindow* slot);

    /// Root implementation macro
    ClassDef(View3D,0);
  };
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_VIEW3D_H */
