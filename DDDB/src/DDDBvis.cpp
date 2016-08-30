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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBTags.h"
#include "DDDB/DDDBDimension.h"
#include "DDDB/DDDBHelper.h"

// C/C++ include files
#include <set>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::DDDB;
using DD4hep::Geometry::LCDD;
using DD4hep::Geometry::VisAttr;


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Keep all in here anonymous. Does not have to be visible outside.
  namespace {
    struct dddb_vis {};
    struct vis      {};
    struct display  {};
    struct volume   {};
    struct include  {};
  }
  template <> void Converter<dddb_vis>::operator()(xml_h element) const;
  template <> void Converter<vis>::operator()(xml_h element) const;
  template <> void Converter<display>::operator()(xml_h element) const;
  template <> void Converter<volume>::operator()(xml_h element) const;
  template <> void Converter<include>::operator()(xml_h element) const;

  /** Convert compact visualization attribute to LCDD visualization attribute
   *
   *  <vis name="SiVertexBarrelModuleVis"
   *       alpha="1.0" r="1.0" g="0.75" b="0.76"
   *       drawingStyle="wireframe"
   *       showDaughters="false"
   *       visible="true"/>
   */
  template <> void Converter<vis>::operator()(xml_h e) const {
    VisAttr attr(e.attr<string>(_U(name)));
    float r = e.hasAttr(_U(r)) ? e.attr<float>(_U(r)) : 1.0f;
    float g = e.hasAttr(_U(g)) ? e.attr<float>(_U(g)) : 1.0f;
    float b = e.hasAttr(_U(b)) ? e.attr<float>(_U(b)) : 1.0f;

    printout(DEBUG, "Compact", "++ Converting VisAttr  structure: %s.",attr.name());
    attr.setColor(r, g, b);
    if (e.hasAttr(_U(alpha)))
      attr.setAlpha(e.attr<float>(_U(alpha)));
    if (e.hasAttr(_U(visible)))
      attr.setVisible(e.attr<bool>(_U(visible)));
    if (e.hasAttr(_U(lineStyle))) {
      string ls = e.attr<string>(_U(lineStyle));
      if (ls == "unbroken")
        attr.setLineStyle(VisAttr::SOLID);
      else if (ls == "broken")
        attr.setLineStyle(VisAttr::DASHED);
    }
    else {
      attr.setLineStyle(VisAttr::SOLID);
    }
    if (e.hasAttr(_U(drawingStyle))) {
      string ds = e.attr<string>(_U(drawingStyle));
      if (ds == "wireframe")
        attr.setDrawingStyle(VisAttr::WIREFRAME);
      else if (ds == "solid")
        attr.setDrawingStyle(VisAttr::SOLID);
    }
    else {
      attr.setDrawingStyle(VisAttr::SOLID);
    }
    if (e.hasAttr(_U(showDaughters)))
      attr.setShowDaughters(e.attr<bool>(_U(showDaughters)));
    else
      attr.setShowDaughters(true);
    lcdd.addVisAttribute(attr);
  }

  template <> void Converter<include>::operator()(xml_h e) const {
    XML::DocumentHolder doc(XML::DocumentHandler().load(e, e.attr_value(_U(ref))));
    xml_h node = doc.root();
    string tag = node.tag();

    if ( tag == "display" )
      xml_coll_t(node,_U(vis)).for_each(Converter<vis>(this->lcdd,param));
    else if ( tag == "vismapping" )
      xml_coll_t(node,_U(volume)).for_each(Converter<volume>(this->lcdd,param));
    else if ( tag == "DDDB_VIS" )
      Converter<dddb_vis>(lcdd,param)(node);
    else if ( tag == "dddb_vis" )
      Converter<dddb_vis>(lcdd,param)(node);
  }

  template <> void Converter<volume>::operator()(xml_h e) const {
    dddb_dim_t dim = e;
    string path = dim.nameStr();
    string attr = dim.visStr();
    DDDBHelper* helper = _param<DDDBHelper>();
    helper->addVisAttr(path, attr);
  }

  template <> void Converter<dddb_vis>::operator()(xml_h e) const {
    xml_coll_t(e, _U(include)).for_each(                  Converter<include>(lcdd,param));
    xml_coll_t(e, _U(display)).for_each(_U(include),      Converter<include>(lcdd,param));
    xml_coll_t(e, _LBU(vismapping)).for_each(_U(include), Converter<include>(lcdd,param));
    xml_coll_t(e, _U(display)).for_each(_U(vis),          Converter<vis>    (lcdd,param));
    xml_coll_t(e, _LBU(vismapping)).for_each(_U(volume),  Converter<volume> (lcdd,param));
  }
}

/// Plugin entry point.
static long load_dddb_vis(lcdd_t& lcdd, xml_h element) {
  DDDBHelper* helper = lcdd.extension<DDDBHelper>(false);
  if ( helper )   {
    DD4hep::Converter<dddb_vis> cnv(lcdd,helper);
    cnv(element);
    return 1;
  }
  except("DDDB","+++ Failed to access cool. No DDDBHelper object defined. Run plugin DDDBInstallHelper.");
  return 0;
}
DECLARE_XML_DOC_READER(DDDB_VIS,load_dddb_vis)
