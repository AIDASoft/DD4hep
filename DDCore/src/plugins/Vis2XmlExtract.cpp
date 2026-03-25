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

// Framework includes
#include "Vis2XmlExtract.h"
#include <DD4hep/Printout.h>
#include <DD4hep/Volumes.h>
#include <XML/DocumentHandler.h>

/// C/C++ include files
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
  std::string genName(const std::string& n, const void* ptr)  {
    std::string nn = n;
    char text[32];
    ::snprintf(text,sizeof(text),"%p",ptr);
    nn += "_";
    nn += text;
    return nn;
  }
}

/// Initializing Constructor
dd4hep::detail::Vis2XmlExtract::Vis2XmlExtract( Detector& description )
  : m_detDesc(description), m_dataPtr(0) {
}

dd4hep::detail::Vis2XmlExtract::~Vis2XmlExtract() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

/// Dump logical volume in GDML format to output stream
xml_h dd4hep::detail::Vis2XmlExtract::handleVolume( const std::string& /* name */, Volume volume ) const {
  GeometryInfo& geo = *this->m_dataPtr;
  xml_h vol = geo.xmlVolumes[volume];
  if (!vol) {
    std::string n = genName(volume->GetName(),volume);
    if (volume->IsAssembly()) {
      vol = xml_elt_t(geo.doc, _U(assembly));
      vol.setAttr(_U(name), n);
    }
    else {
      vol = xml_elt_t(geo.doc, _U(volume));
      vol.setAttr(_U(name), n);
    }
    geo.doc_structure.append(vol);
    geo.xmlVolumes[volume] = vol;
    if( volume.data() )  {
      VisAttr vis = volume.visAttributes();
      if (vis.isValid()) {
        xml_ref_t xml_data = handleVis(vis.name(), vis);
        vol.setRef(_U(visref), xml_data.name());
      }
    }
  }
  return vol;
}

/// Dump logical volume in GDML format to output stream
xml_h dd4hep::detail::Vis2XmlExtract::handleVolumeVis( const std::string& /* name */, const TGeoVolume* volume ) const {
  GeometryInfo& geo = *this->m_dataPtr;
  xml_h         vol = geo.xmlVolumes[volume];
  if (!vol) {
    Volume v(volume);
    if( v.data() )  {
      VisAttr vis = v.visAttributes();
      if (vis.isValid()) {
        geo.doc_structure.append(vol = xml_elt_t(geo.doc, _U(volume)));
        vol.setAttr(_U(name), v->GetName());
        xml_ref_t xml_data = handleVis(vis.name(), vis);
        vol.setRef(_U(visref), xml_data.name());
        geo.xmlVolumes[v] = vol;
      }
    }
  }
  return vol;
}

/// Convert the geometry visualisation attributes to the corresponding Detector object(s).
xml_h dd4hep::detail::Vis2XmlExtract::handleVis( const std::string& /* name */, VisAttr attr ) const {
  GeometryInfo& geo = *this->m_dataPtr;
  xml_h vis = geo.xmlVis[attr];
  if (!vis) {
    float red = 0, green = 0, blue = 0;
    int style = attr.lineStyle();
    int draw = attr.drawingStyle();

    geo.doc_display.append(vis = xml_elt_t(geo.doc, _U(vis)));
    vis.setAttr(_U(name), attr.name());
    vis.setAttr(_U(visible), attr.visible());
    vis.setAttr(_U(show_daughters), attr.showDaughters());
    if (style == VisAttr::SOLID)
      vis.setAttr(_U(line_style), "unbroken");
    else if (style == VisAttr::DASHED)
      vis.setAttr(_U(line_style), "broken");
    if (draw == VisAttr::SOLID)
      vis.setAttr(_U(drawing_style), "solid");
    else if (draw == VisAttr::WIREFRAME)
      vis.setAttr(_U(drawing_style), "wireframe");

    xml_h col = xml_elt_t(geo.doc, _U(color));
    attr.rgb(red, green, blue);
    col.setAttr(_U(alpha), attr.alpha());
    col.setAttr(_U(R), red);
    col.setAttr(_U(B), blue);
    col.setAttr(_U(G), green);
    vis.append(col);
    geo.xmlVis[attr] = vis;
  }
  return vis;
}

/// Create geometry conversion
xml_doc_t dd4hep::detail::Vis2XmlExtract::createVis( DetElement top ) {
  if (!top.isValid()) {
    throw std::runtime_error("Attempt to call createDetector with an invalid geometry!");
  }

  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);
  printout(ALWAYS,"Vis2XmlExtract","++ ==> Dump visualisation attributes "
           "from in memory detector description...");
  xml::DocumentHandler docH;
  xml_elt_t elt(0);
  geo.doc = docH.create("visualization", docH.defaultComment());
  geo.doc_root = geo.doc.root();
  geo.doc_root.append(geo.doc_display = xml_elt_t(geo.doc, _U(display)));
  geo.doc_root.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));

  for( Volume v : geo.volumes )
    this->handleVolumeVis(v->GetName(), v.ptr());
  printout(ALWAYS,"Vis2XmlExtract","++ Handled %ld volumes.",geo.volumes.size());
  return geo.doc;
}

/// Helper constructor
dd4hep::detail::Vis2XmlExtract::GeometryInfo::GeometryInfo()
  : doc(0), doc_root(0), doc_display(0), doc_structure(0)
{
}

static long dump_output( xml_doc_t doc, int argc, char** argv ) {
  dd4hep::xml::DocumentHandler docH;
  return docH.output(doc, argc > 0 ? argv[0] : "");
}

static long create_vis( dd4hep::Detector& description, int argc, char** argv ) {
  dd4hep::detail::Vis2XmlExtract wr(description);
  dd4hep::xml::DocumentHolder doc(wr.createVis(description.world()).ptr());
  return dump_output(doc, argc, argv);
}

static long create_visASCII( dd4hep::Detector& description, int /* argc */, char** argv ) {
  dd4hep::detail::Vis2XmlExtract wr(description);
  /* xml_doc_t doc = */ wr.createVis(description.world());
  auto& geo = *wr.m_dataPtr;
  std::map<std::string, xml_comp_t> vis_map;
  for (xml_coll_t c(geo.doc_display, _U(vis)); c; ++c)
    vis_map.insert(make_pair(xml_comp_t(c).nameStr(), xml_comp_t(c)));

  const char* sep = ";";
  std::ofstream os(argv[0]);
  for (xml_coll_t c(geo.doc_structure, _U(volume)); c; ++c) {
    xml_comp_t vol = c;
    xml_comp_t ref = c.child(_U(visref));
    auto iter = vis_map.find(ref.refStr());
    if ( iter != vis_map.end() )  {
      xml_comp_t vis = iter->second;
      xml_comp_t col = vis.child(_U(color));
      os << "vol:" << vol.nameStr() << sep << "vis:" << vis.nameStr() << sep 
         << "visible:" << vis.visible() << sep << "r:"
         << col.R() << sep << "g:" << col.G() << sep << "b:" << col.B() << sep 
         << "alpha:" << col.alpha() << sep << "line_style:"
         << vis.attr < std::string > (_U(line_style)) << sep 
         << "drawing_style:" << vis.attr < std::string> (_U(drawing_style)) << sep 
         << "show_daughters:" << vis.show_daughters() << sep << std::endl;
    }
  }
  os.close();
  return 1;
}

DECLARE_APPLY(DD4hepGeometry2VIS, create_vis)
DECLARE_APPLY(DD4hepGeometry2VISASCII, create_visASCII)
