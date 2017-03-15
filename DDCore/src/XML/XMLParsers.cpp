//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2017-03-15
//  \version  1.0
//
//==========================================================================

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"
#include "DD4hep/OpaqueData.h"
#include "DD4hep/OpaqueDataBinder.h"
#include "XML/XMLParsers.h"
#include "XML/XMLDimension.h"
#include "XML/DocumentHandler.h"


// C/C++ include files
#include <stdexcept>

using std::string;
using namespace DD4hep;
typedef DD4hep::XML::Handle_t  xml_h;
typedef DD4hep::XML::Dimension xml_dim_t;
typedef DD4hep::XML::Collection_t xml_coll_t;

namespace {
  PrintLevel s_print = DEBUG;
}

/// Set debug level for this module. Default is OFF
bool DD4hep::XML::setXMLParserDebug(bool value)   {
  bool old = s_print==ALWAYS;
  s_print = value ? ALWAYS : DEBUG;
  return old;
}

/// Convert rotation XML objects to DD4hep::Geometry::RotationZYX
void DD4hep::XML::parse(xml_h e, Geometry::RotationZYX& rot)  {
  xml_dim_t r(e);
  rot.SetComponents(r.z(), r.y(), r.x());
  printout(s_print,"Alignment<rot>",
           "  Rotation:   x=%9.3f y=%9.3f   z=%9.3f  phi=%7.4f psi=%7.4f theta=%7.4f",
           r.x(), r.y(), r.z(), rot.Phi(), rot.Psi(), rot.Theta());
}

/// Convert XML position objects to DD4hep::Geometry::Position
void DD4hep::XML::parse(xml_h e, Geometry::Position& pos)  {
  xml_dim_t p(e);
  pos.SetXYZ(p.x(), p.y(), p.z());
  printout(s_print,"Alignment<pos>","  Position:   x=%9.3f y=%9.3f   z=%9.3f",
           pos.X(), pos.Y(), pos.Z());
}

/// Convert XML pivot objects to DD4hep::Geometry::Translation3D objects
void DD4hep::XML::parse(xml_h e, Geometry::Translation3D& tr)   {
  xml_dim_t p(e);
  double x,y,z;
  tr.SetXYZ(x=p.x(), y=p.y(), z=p.z());
  printout(s_print,"<pivot>","     Pivot:      x=%9.3f y=%9.3f   z=%9.3f",x,y,z);
}

/// Convert alignment delta objects to Alignments::Delta
void DD4hep::XML::parse(xml_h e, Alignments::Delta& delta)  {
  using Alignments::Delta;
  Geometry::Position pos;
  Geometry::RotationZYX rot;
  Geometry::Translation3D piv;
  xml_h  child_rot, child_pos, child_piv;

  if ( (child_pos=e.child(_U(position),false)) )
    parse(child_pos, delta.translation);
  if ( (child_rot=e.child(_U(rotation),false)) )   {
    parse(child_rot, delta.rotation);
    if ( (child_piv=e.child(_U(pivot),false)) )
      parse(child_piv, delta.pivot);
  }
  if ( child_rot && child_pos && child_piv )
    delta.flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT|Delta::HAVE_TRANSLATION;
  else if ( child_rot && child_pos )
    delta.flags |= Delta::HAVE_ROTATION|Delta::HAVE_TRANSLATION;
  else if ( child_rot && child_piv )
    delta.flags |= Delta::HAVE_ROTATION|Delta::HAVE_PIVOT;
  else if ( child_rot )
    delta.flags |= Delta::HAVE_ROTATION;
  else if ( child_pos )
    delta.flags |= Delta::HAVE_TRANSLATION;
}

/// Parse delta into an opaque data block
void DD4hep::XML::parse_delta(Handle_t e, OpaqueDataBlock& block)   {
  Alignments::Delta& delta = block.bind<Alignments::Delta>();
  parse(e, delta);
}

/// Converts opaque maps to OpaqueDataBlock objects
void DD4hep::XML::parse_mapping(xml_h e, OpaqueDataBlock& b)   {
  string    val_type = e.attr<string>(_U(value));
  string    key_type = e.attr<string>(_U(key));
  MapBinder binder;

  OpaqueDataBinder::bind_map(binder, b, key_type, val_type);
  for(xml_coll_t i(e,_U(item)); i; ++i)  {
    // If explicit key, value data are present in attributes:
    if ( i.hasAttr(_U(key)) && i.hasAttr(_U(value)) )  {
      string key = i.attr<string>(_U(key));
      string val = i.attr<string>(_U(value));
      OpaqueDataBinder::insert_map(binder, b, key_type, key, val_type, val);
      continue;
    }
    // Otherwise interprete the data directly from the data content
    OpaqueDataBinder::insert_map(binder, b, key_type, val_type, i.text());
  }
}

/// Converts linear STL containers from their string representation.
void DD4hep::XML::parse_sequence(xml_h e, OpaqueDataBlock& block)    {
  xml_dim_t elt(e);
  string typ = elt.typeStr();
  string val = elt.hasAttr(_U(value)) ? elt.valueStr() : elt.text();
  if ( !OpaqueDataBinder::bind_sequence(block, typ, val) )  {
    except("XMLParsers",
           "++ Failed to convert unknown sequence conditions type: %s",typ.c_str());
  }
}
