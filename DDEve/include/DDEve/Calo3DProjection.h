// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_CALO3DPROJECTION_H
#define DD4HEP_DDEVE_CALO3DPROJECTION_H

// Framework include files
#include "DDEve/View.h"
#include "DDEve/Display.h"

class TH2F;
class TEveCalo3D;
class TEveCaloDataHist;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class Calo3DProjection  Calo3DProjection.h DDEve/Calo3DProjection.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class  Calo3DProjection : public View   {
  public:
    /// Initializing constructor
    Calo3DProjection(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~Calo3DProjection();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot);

    /// Root implementation macro
    ClassDef(Calo3DProjection,0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_CALO3DPROJECTION_H */

