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

/// Framework include files
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/Printout.h>
#include <XML/Utilities.h>
#include <DDCAD/ASSIMPReader.h>

// C/C++ include files

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Handle<TObject> create_CAD_Shape(Detector&, xml_h e)   {
  xml_elt_t elt(e);
  string fname = elt.attr<string>(_U(ref));
  auto shapes = cad::ASSIMPReader().read(fname);
  if ( shapes.empty() )   {
    except("CAD_Shape","+++ CAD file: %s does not contain any "
           "understandable tessellated shapes.", fname.c_str());
  }
  Solid solid;
  size_t count = shapes.size();
  if ( count == 1 )   {
    solid = shapes[0].release();
  }
  else   {
    if ( elt.hasAttr(_U(item)) )  {
      size_t which = elt.attr<int>(_U(item));
      solid = shapes[which].release();
    }
    else if ( elt.hasAttr(_U(mesh)) )  {
      size_t which = elt.attr<int>(_U(mesh));
      solid = shapes[which].release();
    }
    else  {
      except("CAD_Shape","+++ CAD file: %s does contains %ld tessellated shapes. "
             "You need to add a selector.", fname.c_str(), shapes.size());
    }
  }
  if ( elt.hasAttr(_U(name)) ) solid->SetName(elt.attr<string>(_U(name)).c_str());
  return solid;
}
DECLARE_XML_SHAPE(CAD_Shape__shape_constructor,create_CAD_Shape)


static Handle<TObject> create_CAD_MultiShape(Detector&, xml_h e)   {
  xml_elt_t elt(e);
  string fname = elt.attr<string>(_U(ref));
  auto shapes = cad::ASSIMPReader().read(fname);
  if ( shapes.empty() )   {
    except("CAD_Shape","+++ CAD file: %s does not contain any "
           "understandable tessellated shapes.", fname.c_str());
  }
  Assembly assembly("assembly");
  for(size_t i=0; i < shapes.size(); ++i)   {
    Solid solid = shapes[i].release();
    if ( solid.isValid() )   {
      Volume vol(_toString(int(i),"vol_%d"), solid, Detector::getInstance().material("Air"));
      assembly.placeVolume(vol);
    }
  }
  if ( elt.hasAttr(_U(name)) ) assembly->SetName(elt.attr<string>(_U(name)).c_str());
  return assembly;
}
DECLARE_XML_SHAPE(CAD_MultiShape__volume_constructor,create_CAD_MultiShape)
