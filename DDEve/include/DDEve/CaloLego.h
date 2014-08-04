// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Original Author: Matevz Tadel 2009 (MultiView.C)
//
//====================================================================
#ifndef DD4HEP_DDEVE_CALOLEGO_H
#define DD4HEP_DDEVE_CALOLEGO_H

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

  /** @class CaloLego  CaloLego.h DDEve/CaloLego.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class  CaloLego : public View   {
    Display::CalodataContext m_data;
  public:
    /// Initializing constructor
    CaloLego(Display* eve, const std::string& name);
    /// Default destructor
    virtual ~CaloLego();
    /// Build the projection view and map it to the given slot
    virtual View& Build(TEveWindow* slot);
    /// Configure a single geometry view
    virtual void ConfigureGeometry(const DisplayConfiguration::ViewConfig& config);
    /// Configure a single event scene view
    virtual void ConfigureEvent(const DisplayConfiguration::ViewConfig& config);
    /// Call to import geometry topics
    void ImportGeoTopics(const std::string& title);

    /// Root implementation macro
    ClassDef(CaloLego,0);
  };

} /* End namespace DD4hep   */

#endif /* DD4HEP_DDEVE_CALOLEGO_H */
