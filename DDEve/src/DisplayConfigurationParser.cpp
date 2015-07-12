// $Id: run_plugin.h 1663 2015-03-20 13:54:53Z gaede $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/LCDDLoad.h"
#include "DD4hep/Printout.h"
#include "XML/Conversions.h"
#include "XML/XMLElements.h"
#include "XML/DocumentHandler.h"
#include "DD4hep/DetFactoryHelper.h"

#include "DDEve/Display.h"
#include "DDEve/DisplayConfiguration.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;


namespace DD4hep  {  namespace   {
    /// Some utility class to specialize the convetrers:
    class ddeve;
    class view;
    class panel;
    class calodata;
    class calodata_configs;
    class detelement;
    class include;
    class display;
    class collection;
    class collection_configs;
  }

  typedef DisplayConfiguration::Configurations Configurations;
  typedef DisplayConfiguration::ViewConfigurations ViewConfigurations;

  /// Forward declarations for all specialized converters
  template <> void Converter<ddeve>::operator()(xml_h seq)  const;
  template <> void Converter<display>::operator()(xml_h seq)  const;
  template <> void Converter<view>::operator()(xml_h seq)  const;
  template <> void Converter<panel>::operator()(xml_h seq)  const;
  template <> void Converter<include>::operator()(xml_h seq)  const;
  template <> void Converter<calodata>::operator()(xml_h e)  const;
  template <> void Converter<calodata_configs>::operator()(xml_h e)  const;
  template <> void Converter<collection>::operator()(xml_h e)  const;
  template <> void Converter<collection_configs>::operator()(xml_h e)  const;
  template <> void Converter<detelement>::operator()(xml_h seq)  const;
}

#define DECL_TAG(x) XML::Strng_t u_##x(#x)
namespace {
  DECL_TAG(clone);
  DECL_TAG(load_geo);
  DECL_TAG(show_evt);
  DECL_TAG(use);
  DECL_TAG(emax);
  DECL_TAG(hits);
  DECL_TAG(n_eta);
  DECL_TAG(eta_min);
  DECL_TAG(eta_max);
  DECL_TAG(n_phi);
  DECL_TAG(phi_min);
  DECL_TAG(phi_max);
  DECL_TAG(calodata);
  DECL_TAG(towerH);
  DECL_TAG(visLevel);
  DECL_TAG(loadLevel);
}

static void extract(DisplayConfiguration::Config& c, xml_h e, int typ)   {
  c.name  = e.attr<string>(_U(name));
  c.type = typ;
  c.data.defaults.show_evt  = e.hasAttr(u_show_evt) ? e.attr<int>(u_show_evt) :  1;
  c.data.defaults.load_geo  = e.hasAttr(u_load_geo) ? e.attr<int>(u_load_geo) : -1;
  c.data.defaults.color = e.hasAttr(_U(color)) ? e.attr<int>(_U(color)) : 0xBBBBBB;
  c.data.defaults.alpha = e.hasAttr(_U(alpha)) ? e.attr<float>(_U(alpha)) : -1.0;
  c.data.calo3d.emax    = e.hasAttr(u_emax) ? e.attr<float>(u_emax) : 25.0;
  c.data.calo3d.towerH  = e.hasAttr(u_towerH) ? e.attr<float>(u_towerH) : 25.0;
  if ( e.hasAttr(_U(dz))   ) c.data.calo3d.dz = e.attr<float>(_U(dz));
  if ( e.hasAttr(_U(rmin)) ) c.data.calo3d.rmin = e.attr<float>(_U(rmin));
  if ( e.hasAttr(_U(threshold)) ) c.data.calo3d.threshold = e.attr<float>(_U(threshold));
  if ( e.hasAttr(u_hits)   ) c.hits = e.attr<string>(u_hits);
  if ( e.hasAttr(u_use)    ) c.use = e.attr<string>(u_use);
}

/** Convert display configuration objects of  tag type ddeve/detelement
 *
 *  <detelement name="TPC" load="1" ... />
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<detelement>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  extract(c,e,DisplayConfiguration::DETELEMENT);
  configs->push_back(c);
}
template <> void Converter<panel>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  extract(c,e,DisplayConfiguration::PANEL);
  configs->push_back(c);
}
template <> void Converter<calodata_configs>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  extract(c,e,DisplayConfiguration::CALODATA);
  configs->push_back(c);
}

template <> void Converter<collection_configs>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  c.name = e.attr<string>(_U(name));
  c.type = DisplayConfiguration::COLLECTION;
  c.data.hits.color     = e.hasAttr(_U(color)) ? e.attr<int>(_U(color)) : 0xBBBBBB;
  c.data.hits.alpha     = e.hasAttr(_U(alpha)) ? e.attr<float>(_U(alpha)) : -1.0;
  c.data.hits.emax      = e.hasAttr(u_emax) ? e.attr<float>(u_emax) : 25.0;
  c.data.hits.towerH    = e.hasAttr(u_towerH) ? e.attr<float>(u_towerH) : 25.0;
  c.data.hits.threshold = e.hasAttr(_U(threshold)) ? e.attr<float>(_U(threshold)) : 0.0;
  if ( e.hasAttr(u_hits)   ) c.hits = e.attr<string>(u_hits);
  if ( e.hasAttr(u_use)    ) c.use = e.attr<string>(u_use);
  configs->push_back(c);
}

/** Convert display configuration elements of tag type ddeve/view
 *
 *    <view>
 *      <detelement name="TPC" ... />
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<view>::operator()(xml_h e)  const  {
  ViewConfigurations* configs = (ViewConfigurations*)param;
  DisplayConfiguration::ViewConfig c;
  extract(c,e,DisplayConfiguration::VIEW);
  c.type  = e.attr<string>(_U(type));
  c.show_structure = e.hasAttr(_U(structure)) ? e.attr<bool>(_U(structure)) : true;
  c.show_sensitive = e.hasAttr(_U(sensitive)) ? e.attr<bool>(_U(sensitive)) : true;
  c.name  = e.attr<string>(_U(name));
  printout(INFO,"DisplayConfiguration","+++ View: %s sensitive:%d structure:%d.",
           c.name.c_str(), c.show_sensitive, c.show_structure);
  xml_coll_t(e,_Unicode(panel)).for_each(Converter<panel>(lcdd,&c.subdetectors));
  xml_coll_t(e,_Unicode(detelement)).for_each(Converter<detelement>(lcdd,&c.subdetectors));
  xml_coll_t(e,_Unicode(calodata)).for_each(Converter<calodata_configs>(lcdd,&c.subdetectors));
  xml_coll_t(e,_Unicode(collection)).for_each(Converter<collection_configs>(lcdd,&c.subdetectors));
  configs->push_back(c);
}

/** Convert display configuration elements of tag type ddeve/calodata
 *
 *    <view>
 *      <calodata name="TPC" n_eta="5" ..../>
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<calodata>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  c.name    = e.attr<string>(_U(name));
  c.type    = DisplayConfiguration::CALODATA;
  if ( e.hasAttr(u_use)    )   {
    c.use = e.attr<string>(u_use);
    c.hits = e.attr<string>(u_hits);
  }
  else  {
    c.hits = e.attr<string>(u_hits);
    c.data.calodata.n_eta   = e.attr<int>(u_n_eta);
    c.data.calodata.eta_min = e.attr<float>(u_eta_min);
    c.data.calodata.eta_max = e.attr<float>(u_eta_max);
    c.data.calodata.n_phi   = e.attr<int>(u_n_phi);
    c.data.calodata.phi_min = e.attr<float>(u_phi_min);
    c.data.calodata.phi_max = e.attr<float>(u_phi_max);
    c.data.calodata.dz      = e.attr<float>(_U(dz));
    c.data.calodata.rmin    = e.attr<float>(_U(rmin));
    c.data.calodata.color   = e.attr<int>(_U(color));
    c.data.calodata.alpha   = e.attr<float>(_U(alpha));
    c.data.calodata.emax    = e.hasAttr(u_emax) ? e.attr<float>(u_emax) : 25.0;
    if ( e.hasAttr(u_towerH) ) c.data.calodata.towerH = e.attr<float>(u_towerH);
    if ( e.hasAttr(_U(threshold)) ) c.data.calodata.threshold = e.attr<float>(_U(threshold));
  }
  configs->push_back(c);
}

/** Convert display configuration elements of tag type ddeve/hits
 *
 *    <view>
 *      <calodata name="TPC" n_eta="5" ..../>
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<collection>::operator()(xml_h e)  const  {
  Configurations* configs = (Configurations*)param;
  DisplayConfiguration::Config c;
  c.name = e.attr<string>(_U(name));
  c.type = DisplayConfiguration::COLLECTION;
  c.hits = e.attr<string>(u_hits);
  c.data.hits.size  = e.attr<float>(_U(size));
  c.data.hits.type  = e.attr<float>(_U(type));
  c.data.hits.color = e.hasAttr(_U(color))  ? e.attr<int>(_U(color)) : kRed;
  c.data.hits.alpha = e.hasAttr(_U(alpha))  ? e.attr<float>(_U(alpha)) : 1.0;
  c.data.hits.width = e.hasAttr(_U(width))  ? e.attr<float>(_U(width)) : 1.0;
  c.data.hits.emax    = e.hasAttr(u_emax)   ? e.attr<float>(u_emax)    : 25.0;
  c.data.hits.towerH  = e.hasAttr(u_towerH) ? e.attr<float>(u_towerH)  : 25.0;
  c.data.hits.threshold = e.hasAttr(_U(threshold)) ? e.attr<float>(_U(threshold)) : 0;
  configs->push_back(c);
}

/** Convert display configuration elements of tag type ddeve/include
 *
 *    <ddeve>
 *      <view>   ...   </view>
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<include>::operator()(xml_h e)  const  {
  LCDDLoad* load = dynamic_cast<LCDDLoad*>(&this->lcdd);
  load->processXML(e,e.attr<string>(_U(ref)));
}

/** Convert display configuration elements of tag type ddeve/include
 *
 *    <ddeve>
 *      <include ref="..."/>
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<display>::operator()(xml_h e)  const  {
  Display* d = (Display*)param;
  if ( e.hasAttr(_Unicode(visLevel)) ) d->setVisLevel(e.attr<int>(_Unicode(visLevel)));
  if ( e.hasAttr(_Unicode(loadLevel)) ) d->setLoadLevel(e.attr<int>(_Unicode(loadLevel)));
}

/** Convert display configuration elements of tag type ddeve
 *
 *    <ddeve>
 *      <view>   ...   </view>
 *        ...
 *    </ddeve>
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
template <> void Converter<ddeve>::operator()(xml_h e)  const  {
  Display* disp = (Display*)param;
  DisplayConfiguration cfg(disp);
  /// Now we process all allowed elements within this tag
  xml_coll_t(e,_Unicode(display)).for_each(Converter<display>(lcdd,disp));
  xml_coll_t(e,_Unicode(include)).for_each(Converter<include>(lcdd,disp));
  xml_coll_t(e,_Unicode(calodata)).for_each(Converter<calodata>(lcdd,&cfg.calodata));
  xml_coll_t(e,_Unicode(collection)).for_each(Converter<collection>(lcdd,&cfg.collections));
  xml_coll_t(e,_Unicode(view)).for_each(Converter<view>(lcdd,&cfg.views));
  disp->ImportConfiguration(cfg);
}

#include "TEveProjections.h"
/** Basic entry point to read display configuration files
 *
 *  @author  M.Frank
 *  @version 1.0
 *  @date    01/06/2014
 */
static long setup_DDEve(lcdd_t& lcdd, const xml_h& e) {
  Display* display = lcdd.extension<Display>();
  static bool first = true;
  if ( first )   {
    first = false;
#define add_root_enum(x) XML::_toDictionary(XML::Strng_t(#x),int(x))
    add_root_enum(kOrange);
    add_root_enum(kBlue);
    add_root_enum(kAzure);
    add_root_enum(kRed);
    add_root_enum(kCyan);
    add_root_enum(kPink);
    add_root_enum(kGreen);
    add_root_enum(kViolet);
    add_root_enum(kYellow);
    add_root_enum(kTeal);
    add_root_enum(kMagenta);
    add_root_enum(kSpring);
    add_root_enum(kTeal);

    add_root_enum(kDot);
    add_root_enum(kPlus);
    add_root_enum(kStar);
    add_root_enum(kCircle);
    add_root_enum(kMultiply);
    add_root_enum(kFullDotSmall);
    add_root_enum(kFullDotMedium);
    add_root_enum(kFullDotLarge);
    add_root_enum(kFullCircle);
    add_root_enum(kFullSquare);
    add_root_enum(kFullTriangleUp);
    add_root_enum(kFullTriangleDown);
    add_root_enum(kOpenCircle);
    add_root_enum(kOpenSquare);
    add_root_enum(kOpenTriangleUp);
    add_root_enum(kOpenDiamond);
    add_root_enum(kOpenCross);
    add_root_enum(kFullStar);
    add_root_enum(kOpenStar);
    add_root_enum(kOpenTriangleDown);
    add_root_enum(kFullDiamond);
    add_root_enum(kFullCross);
    add_root_enum(TEveProjection::kPT_RPhi);
    add_root_enum(TEveProjection::kPT_RhoZ);
    add_root_enum(TEveProjection::kPT_3D);
    add_root_enum(TEveProjection::kPP_Plane);
    add_root_enum(TEveProjection::kPP_Distort);
    add_root_enum(TEveProjection::kPP_Full);
    add_root_enum(TEveProjection::kGM_Polygons);
    add_root_enum(TEveProjection::kGM_Segments);
  }
  (DD4hep::Converter<DD4hep::ddeve>(lcdd,display))(e);
  return 1;
}
DECLARE_XML_DOC_READER(ddeve,setup_DDEve)
