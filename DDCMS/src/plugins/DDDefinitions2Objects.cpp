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
//
// DDCMS is a detector description convention developed by the CMS experiment.
//
//==========================================================================

// Framework includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Path.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ObjectsInterna.h"

#include "XML/Utilities.h"
#include "DDCMS/DDCMS.h"

// Root/TGeo include files
#include "TGeoManager.h"
#include "TGeoMaterial.h"

// C/C++ include files
#include <climits>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cms;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  namespace {

    static UInt_t unique_mat_id = 0xAFFEFEED;

    class dddefinition;
    class include;
    class constantssection;
    class constant;

    class materialsection;
    class elementarymaterial;
    class compositematerial;
  
    class rotationsection;
    class rotation;

    class pospartsection;
    class pospart;

    class logicalpartsection;
    class logicalpart;

    class solidsection;
    class tubs;
    class polycone;
    class box;
    class algorithm;    

    class vissection;
    class vis_apply;
    class vis;
    class debug;
  }

  /// Converter instances implemented in this compilation unit
  template <> void Converter<dddefinition>::operator()(xml_h element) const;
  template <> void Converter<debug>::operator()(xml_h element) const;

  template <> void Converter<constantssection>::operator()(xml_h element) const;
  template <> void Converter<constant>::operator()(xml_h element) const;

  template <> void Converter<vissection>::operator()(xml_h element) const;
  template <> void Converter<vis_apply>::operator()(xml_h element) const;
  template <> void Converter<vis>::operator()(xml_h element) const;

  template <> void Converter<materialsection>::operator()(xml_h element) const;
  template <> void Converter<elementarymaterial>::operator()(xml_h element) const;
  template <> void Converter<compositematerial>::operator()(xml_h element) const;

  template <> void Converter<rotationsection>::operator()(xml_h element) const;
  template <> void Converter<rotation>::operator()(xml_h element) const;

  template <> void Converter<logicalpartsection>::operator()(xml_h element) const;
  template <> void Converter<logicalpart>::operator()(xml_h element) const;

  template <> void Converter<pospartsection>::operator()(xml_h element) const;
  template <> void Converter<pospart>::operator()(xml_h element) const;

  template <> void Converter<solidsection>::operator()(xml_h element) const;
  template <> void Converter<polycone>::operator()(xml_h element) const;
  template <> void Converter<tubs>::operator()(xml_h element) const;
  template <> void Converter<box>::operator()(xml_h element) const;

  template <> void Converter<algorithm>::operator()(xml_h element) const;

  /// DD4hep specific
  template <> void Converter<include>::operator()(xml_h element) const;
}

/// Converter for <ConstantsSection/> tags
template <> void Converter<constantssection>::operator()(xml_h element) const  {
  ParsingContext* context = _param<ParsingContext>();
  Namespace ns(context, element);
  xml_coll_t(element, _CMU(Constant)).for_each(Converter<constant>(description,context));
}

/// Converter for <VisSection/> tags
template <> void Converter<vissection>::operator()(xml_h element) const  {
  ParsingContext* context = _param<ParsingContext>();
  Namespace ns(context, element);
  xml_coll_t(element, _CMU(vis)).for_each(Converter<vis>(description,context));
}

template <> void Converter<materialsection>::operator()(xml_h element) const   {
  ParsingContext* context = _param<ParsingContext>();
  Namespace ns(context, element);
  xml_coll_t(element, _CMU(ElementaryMaterial)).for_each(Converter<elementarymaterial>(description,context));
  xml_coll_t(element, _CMU(CompositeMaterial)).for_each(Converter<compositematerial>(description,context));
}

template <> void Converter<rotationsection>::operator()(xml_h element) const   {
  Namespace ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(Rotation)).for_each(Converter<rotation>(description,ns.context));
}

template <> void Converter<pospartsection>::operator()(xml_h element) const   {
  Namespace ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(PosPart)).for_each(Converter<pospart>(description,ns.context));
}

template <> void Converter<logicalpartsection>::operator()(xml_h element) const   {
  Namespace ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(LogicalPart)).for_each(Converter<logicalpart>(description,ns.context));
}

template <> void Converter<solidsection>::operator()(xml_h element) const   {
  Namespace ns(_param<ParsingContext>(), element);
  for(xml_coll_t solid(element, _U(star)); solid; ++solid)   {
    string tag = solid.tag();
    if ( tag == "Box" )
      Converter<box>(description,ns.context)(solid);
    else if ( tag == "Polycone" )
      Converter<polycone>(description,ns.context)(solid);
    else if ( tag == "Tubs" )
      Converter<tubs>(description,ns.context)(solid);
    else
      printout(ERROR,"DDCMS","+++ Request to process unknown shape of type %s",tag.c_str());
  }
}

/// Converter for <Constant/> tags
template <> void Converter<constant>::operator()(xml_h element) const  {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t constant(element);
  xml_dim_t parent(constant.parent());
  bool      eval = parent.hasAttr(_U(eval)) ? parent.attr<bool>(_U(eval)) : false;
  string    val   = "";//ns.real_name(constant.attr<string>(_U(value)));
  string    nam   = ns.prepend(constant.nameStr());
  string    typ   = eval ? "number" : "string";

  if ( constant.hasAttr(_U(type)) ) typ = constant.typeStr();
  if ( nam == "pixbarladderfull_CFStripY" )   { // debugging
    val = ns.real_name(constant.attr<string>(_U(value)));
  }
  else {
    val = ns.real_name(constant.attr<string>(_U(value)));
  }
  printout(ns.context->debug_constants ? ALWAYS : DEBUG,
           "DDCMS","+++ Add constant object: %s = %s [type:%s]",
           nam.c_str(), val.c_str(), typ.c_str());
  Constant c(nam, val, typ);
  _toDictionary(nam, val, typ);
  description.addConstant(c);
}

/** Convert compact visualization attribute to Detector visualization attribute
 *
 *  <vis name="SiVertexBarrelModuleVis"
 *       alpha="1.0" r="1.0" g="0.75" b="0.76"
 *       drawingStyle="wireframe"
 *       showDaughters="false"
 *       visible="true"/>
 */
template <> void Converter<vis>::operator()(xml_h e) const {
  Namespace ns(_param<ParsingContext>());
  VisAttr attr(e.attr<string>(_U(name)));
  float red   = e.hasAttr(_U(r)) ? e.attr<float>(_U(r)) : 1.0f;
  float green = e.hasAttr(_U(g)) ? e.attr<float>(_U(g)) : 1.0f;
  float blue  = e.hasAttr(_U(b)) ? e.attr<float>(_U(b)) : 1.0f;

  printout(ns.context->debug_visattr ? ALWAYS : DEBUG, "Compact",
           "++ Converting VisAttr  structure: %-16s. R=%.3f G=%.3f B=%.3f",
           attr.name(), red, green, blue);
  attr.setColor(red, green, blue);
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
  description.addVisAttribute(attr);
}

/// Converter for <ElementaryMaterial/> tags
template <> void Converter<elementarymaterial>::operator()(xml_h element) const   {
  Namespace     ns(_param<ParsingContext>());
  xml_dim_t     xmat(element);
  string        nam = ns.prepend(xmat.nameStr());
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(nam.c_str());
  if ( 0 == mat )   {
    const char* matname = nam.c_str();
    double density      = xmat.density();
    //double atomicWeight = xmat.attr<double>(_CMU(atomicWeight));
    double atomicNumber = xmat.attr<double>(_CMU(atomicNumber));
    TGeoElementTable* tab = mgr.GetElementTable();
    TGeoMixture*      mix = new TGeoMixture(nam.c_str(), 1, density);
    TGeoElement*      elt = tab->FindElement(xmat.nameStr().c_str());

    printout(ns.context->debug_materials ? ALWAYS : DEBUG, "DDCMS",
             "++ Converting material %-48s  Density: %.3f.",
             ('"'+nam+'"').c_str(), density);

    if ( !elt )  {
      printout(WARNING,"DDCMS",
               "+++ Converter<ElementaryMaterial> No element present with name:%s  [FAKE IT]",
               matname);
      int n = int(atomicNumber/2e0);
      if ( n < 2 ) n = 2;
      elt = new TGeoElement(xmat.nameStr().c_str(),"CMS element",n,atomicNumber);
      //return;
    }
    if ( elt->Z() == 0 )   {
      int n = int(atomicNumber/2e0);
      if ( n < 2 ) n = 2;
      elt = new TGeoElement((xmat.nameStr()+"-CMS").c_str(),"CMS element",n,atomicNumber);
    }
    mix->AddElement(elt, 1.0);
    mix->SetRadLen(0e0);
    /// Create medium from the material
    TGeoMedium* medium = mgr.GetMedium(matname);
    if (0 == medium) {
      --unique_mat_id;
      medium = new TGeoMedium(matname, unique_mat_id, mix);
      medium->SetTitle("material");
      medium->SetUniqueID(unique_mat_id);
    }
  }
}

/// Converter for <CompositeMaterial/> tags
template <> void Converter<compositematerial>::operator()(xml_h element) const   {
  Namespace     ns(_param<ParsingContext>());
  xml_dim_t     xmat(element);
  string        nam = ns.prepend(xmat.nameStr());
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(nam.c_str());
  if ( 0 == mat )   {
    const char*  matname = nam.c_str();
    double       density = xmat.density();
    xml_coll_t   composites(xmat,_CMU(MaterialFraction));
    TGeoMixture* mix = new TGeoMixture(nam.c_str(), composites.size(), density);

    printout(ns.context->debug_materials ? ALWAYS : DEBUG, "DDCMS",
             "++ Converting material %-48s  Density: %.3f.",
             ('"'+nam+'"').c_str(), density);
    
    for (composites.reset(); composites; ++composites)   {
      xml_dim_t xfrac(composites);
      xml_dim_t xfrac_mat(xfrac.child(_CMU(rMaterial)));
      double    fraction = xfrac.fraction();
      string    fracname = ns.real_name(xfrac_mat.nameStr());

      TGeoMaterial* frac_mat = mgr.GetMaterial(fracname.c_str());
      if ( frac_mat )  {
        mix->AddElement(frac_mat, fraction);
        continue;
      }
      printout(WARNING,"DDCMS","+++ Composite material \"%s\" not present!",
               fracname.c_str());
    }
    mix->SetRadLen(0e0);
    /// Create medium from the material
    TGeoMedium* medium = mgr.GetMedium(matname);
    if (0 == medium) {
      --unique_mat_id;
      medium = new TGeoMedium(matname, unique_mat_id, mix);
      medium->SetTitle("material");
      medium->SetUniqueID(unique_mat_id);
    }
  }
}

/// Converter for <Rotation/> tags
template <> void Converter<rotation>::operator()(xml_h element) const  {
  ParsingContext* ctx = _param<ParsingContext>();
  Namespace ns(ctx);
  xml_dim_t xrot(element);
  string    nam = ns.prepend(xrot.nameStr());
  double    thetaX = xrot.hasAttr(_CMU(thetaX)) ? xrot.attr<double>(_CMU(thetaX)) : 0e0;
  double    phiX   = xrot.hasAttr(_CMU(phiX))   ? xrot.attr<double>(_CMU(phiX))   : 0e0;
  double    thetaY = xrot.hasAttr(_CMU(thetaY)) ? xrot.attr<double>(_CMU(thetaY)) : 0e0;
  double    phiY   = xrot.hasAttr(_CMU(phiY))   ? xrot.attr<double>(_CMU(phiY))   : 0e0;
  double    thetaZ = xrot.hasAttr(_CMU(thetaZ)) ? xrot.attr<double>(_CMU(thetaZ)) : 0e0;
  double    phiZ   = xrot.hasAttr(_CMU(phiZ))   ? xrot.attr<double>(_CMU(phiZ))   : 0e0;
  
  Rotation3D rot = make_rotation3D(thetaX, phiX, thetaY, phiY, thetaZ, phiZ);
  ctx->rotations[nam] = rot;
}

/// Converter for <Rotation/> tags
template <> void Converter<logicalpart>::operator()(xml_h element) const {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string    sol_nam  = xml_dim_t(e.child(_CMU(rSolid))).nameStr();
  string    mat_nam  = xml_dim_t(e.child(_CMU(rMaterial))).nameStr();
  Material  material = description.material(ns.real_name(mat_nam));
  Solid     solid    = ns.context->shapes[sol_nam];
  Volume    volume(ns.prepend(e.nameStr()), solid, material);

  printout(ns.context->debug_volumes ? ALWAYS : DEBUG, "DDCMS",
           "+++ Volume:%-24s Solid:%-24s [%-24s] Material:%s",
           volume.name(), solid.name(), solid.type(), material.name());
  
  ns.context->volumes[volume.name()] = volume;
}

/// Converter for <Rotation/> tags
template <> void Converter<pospart>::operator()(xml_h element) const {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  int    copy   = e.attr<int>(_CMU(copyNumber));
  string parent_nam = ns.real_name(xml_dim_t(e.child(_CMU(rParent))).nameStr());
  string child_nam  = ns.real_name(xml_dim_t(e.child(_CMU(rChild))).nameStr());
  Volume parent = ns.context->volumes[parent_nam];
  Volume child  = ns.context->volumes[child_nam];
  xml_dim_t translation(e.child(_CMU(Translation),false));
  xml_dim_t rotation(e.child(_CMU(Rotation),false));
  xml_dim_t refRotation(e.child(_CMU(rRotation),false));
  Position    pos;
  RotationZYX rot;

  printout(ns.context->debug_placements ? ALWAYS : DEBUG, "DDCMS",
           "+++ %s Parent: %-24s [%s] Child: %-32s [%s] copy:%d",
           e.tag().c_str(),
           parent_nam.c_str(), parent.isValid() ? "VALID" : "INVALID",
           child_nam.c_str(),  child.isValid()  ? "VALID" : "INVALID",
           copy);
  if ( translation.ptr() )   {
    double x  = ns.attr<double>(translation,_U(x));
    double y  = ns.attr<double>(translation,_U(y));
    double z  = ns.attr<double>(translation,_U(z));
    pos = Position(x,y,z);
  }
  if ( rotation.ptr() )   {
    double x  = ns.attr<double>(rotation,_U(x));
    double y  = ns.attr<double>(rotation,_U(y));
    double z  = ns.attr<double>(rotation,_U(z));
    rot = RotationZYX(z,y,x);
  }
  PlacedVolume pv;
  if ( child.isValid() )  {
    if ( !translation.ptr() && !rotation.ptr() )
      pv = parent.placeVolume(child);
    else if ( translation.ptr() && !rotation.ptr() )
      pv = parent.placeVolume(child,pos);
    else if ( !translation.ptr() && rotation.ptr() )
      pv = parent.placeVolume(child,rot);
    else  {
      Transform3D trafo(rot,pos);
      pv = parent.placeVolume(child,rot);
    }
  }
  if ( !pv.isValid() )   {
    printout(ERROR,"DDCMS","+++ Placement FAILED!");
  }
}

/// Converter for <Polycone/> tags
template <> void Converter<polycone>::operator()(xml_h element) const {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  double startPhi = ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = ns.attr<double>(e,_CMU(deltaPhi));
  vector<double> z, rmin, rmax;
  
  for(xml_coll_t zplane(element, _CMU(ZSection)); zplane; ++zplane)   {
    rmin.push_back(ns.attr<double>(zplane,_CMU(rMin)));
    rmax.push_back(ns.attr<double>(zplane,_CMU(rMax)));
    z.push_back(ns.attr<double>(zplane,_CMU(z)));
  }
  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++ Processing shape of type %s : %s",e.tag().c_str(), nam.c_str());
  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++    startPhi=%10.3f [rad] deltaPhi=%10.3f [rad]  %4ld z-planes",
           startPhi, deltaPhi, z.size());
  Polycone pc(startPhi,deltaPhi,rmin,rmax,z);
  ns.context->shapes[nam] = pc.setName(nam);
}

/// Converter for <Tubs/> tags
template <> void Converter<tubs>::operator()(xml_h element) const {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam  = e.nameStr();
  double dz   = ns.attr<double>(e,_CMU(dz));
  double rmin = ns.attr<double>(e,_CMU(rMin));
  double rmax = ns.attr<double>(e,_CMU(rMax));
  double startPhi = ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = ns.attr<double>(e,_CMU(deltaPhi));

  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++ Processing shape of type %s : %s",e.tag().c_str(), nam.c_str());
  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++    dz=%10.3f [cm] rmin=%10.3f [cm] rmax=%10.3f [cm]"
           " startPhi=%10.3f [rad] deltaPhi=%10.3f [rad]", dz, rmin, rmax, startPhi, deltaPhi);

  Tube tube(rmin,rmax,dz,startPhi,deltaPhi);
  ns.context->shapes[nam] = tube.setName(nam);
}

/// Converter for <Box/> tags
template <> void Converter<box>::operator()(xml_h element) const {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  double dx  = ns.attr<double>(e,_CMU(dx));
  double dy  = ns.attr<double>(e,_CMU(dy));
  double dz  = ns.attr<double>(e,_CMU(dz));

  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++ Processing shape of type %s : %s",e.tag().c_str(), nam.c_str());
  printout(ns.context->debug_shapes ? ALWAYS : DEBUG, "DDCMS",
           "+++    dx=%10.3f [cm] dy=%10.3f [cm] dz=%10.3f [cm]", dx, dy, dz);
  Box box(dx,dy,dz);
  ns.context->shapes[nam] = box.setName(nam);
}

/// DD4hep specific Converter for <Include/> tags
template <> void Converter<include>::operator()(xml_h element) const   {
  xml::DocumentHolder doc(xml::DocumentHandler().load(element, element.attr_value(_U(ref))));
  xml_h  node = doc.root();
  Converter<dddefinition>(description,_param<ParsingContext>())(node);
}

/// Converter for <Algorithm/> tags
template <> void Converter<algorithm>::operator()(xml_h element) const  {
  Namespace ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string name = e.nameStr();
  try {
    SensitiveDetector sd;
    Segmentation      seg;
    string type = ns.real_name(e.nameStr());

    // SensitiveDetector and Segmentation currently are undefined. Let's keep it like this
    // until we found something better.....

    DetElement det(PluginService::Create<NamedObject*>(type, &description, ns.context, &element, &sd));
    if (det.isValid()) {
      // setChildTitles(make_pair(name, det));
      if ( sd.isValid() )  {
        det->flag |= DetElement::Object::HAVE_SENSITIVE_DETECTOR;
      }
      if ( seg.isValid() )  {
        seg->sensitive = sd;
        seg->detector  = det;
      }
    }
#if 0
    if (!det.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &description, ns.context, &element, &sd);
      except("DDCMS","Failed to execute subdetector creation plugin. " + dbg.missingFactory(type));
    }
#endif
    ///description.addDetector(det);
    printout(ERROR, "DDCMS", "++ FAILED  NOT ADDING SUBDETECTOR %s", name.c_str());
    return;
  }
  catch (const exception& exc) {
    printout(ERROR, "DDCMS", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), exc.what());
    terminate();
  }
  catch (...) {
    printout(ERROR, "DDCMS", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), "UNKNONW Exception");
    terminate();
  }
}

template <> void Converter<debug>::operator()(xml_h dbg) const {
  Namespace ns(_param<ParsingContext>());
  if ( dbg.hasChild(_CMU(debug_constants)) )  ns.context->debug_constants  = true;
  if ( dbg.hasChild(_CMU(debug_materials)) )  ns.context->debug_materials  = true;
  if ( dbg.hasChild(_CMU(debug_shapes)) )     ns.context->debug_shapes     = true;
  if ( dbg.hasChild(_CMU(debug_volumes)) )    ns.context->debug_volumes    = true;
  if ( dbg.hasChild(_CMU(debug_placements)) ) ns.context->debug_placements = true;
  if ( dbg.hasChild(_CMU(debug_namespaces)) ) ns.context->debug_namespaces = true;
}

template <> void Converter<vis_apply>::operator()(xml_h /* dddefinition */) const {
  struct VisPatcher: public detail::GeoScan {
    Detector& detector;
    VisPatcher(Detector& d) : detail::GeoScan(d.world()), detector(d)  {    }
    void patch()   const  {
      printout(INFO,"Detector","+++ Applying DD4hep visualization attributes....");
      for (auto i = m_data->rbegin(); i != m_data->rend(); ++i) {
        for( const TGeoNode* n : (*i).second )  {
          Volume  vol(n->GetVolume());
          VisAttr vis = detector.visAttributes(vol.name());
          printout(DEBUG,"Vis","+++ %s  vis-attrs:%s",vol.name(), yes_no(vis.isValid()));
          vol.setVisAttributes(vis);
        }
      }
    }
  };
  VisPatcher(description).patch();
}

/// Converter for <DDDefinition/> tags
template <> void Converter<dddefinition>::operator()(xml_h element) const {
  static int num_calls = 0;
  static ParsingContext ctxt;
  xml_elt_t dddef(element);
  string fname = xml::DocumentHandler::system_path(element);
  bool open_geometry  = true;
  bool close_geometry = true;
  ++num_calls;

  Path   path(fname);
  string ns = path.filename().substr(0,path.filename().rfind('.'));
  ctxt.namespaces.push_back(ns+'_');
  xml_coll_t(dddef, _U(debug)).for_each(Converter<debug>(description,&ctxt));
  
  // Here we define the order how XML elements are processed.
  // Be aware of dependencies. This can only defined once.
  // At the end it is a limitation of DOM....
  printout(INFO,"DDCMS","+++ Processing the CMS detector description %s",fname.c_str());
  xml_coll_t(dddef, _CMU(ConstantsSection)).for_each(Converter<constantssection>(description,&ctxt));
  xml_coll_t(dddef, _CMU(VisSection)).for_each(Converter<vissection>(description,&ctxt));
  xml_coll_t(dddef, _CMU(IncludeSection)).for_each(_CMU(Include), Converter<include>(description,&ctxt));

  xml_coll_t(dddef, _CMU(RotationSection)).for_each(Converter<rotationsection>(description,&ctxt));
  xml_coll_t(dddef, _CMU(MaterialSection)).for_each(Converter<materialsection>(description,&ctxt));

  xml_coll_t(dddef, _CMU(SolidSection)).for_each(Converter<solidsection>(description,&ctxt));
  xml_coll_t(dddef, _CMU(LogicalPartSection)).for_each(Converter<logicalpartsection>(description,&ctxt));
  xml_coll_t(dddef, _CMU(PosPartSection)).for_each(Converter<pospartsection>(description,&ctxt));

  /// Analyse algorithms to be called
  if ( !ctxt.geo_inited && dddef.hasChild(_CMU(Algorithm)) )  {
    ctxt.geo_inited = true;
    description.init();
  }
  else if ( num_calls == 1 && open_geometry )   {
    ctxt.geo_inited = true;
    description.init();
  }
  xml_coll_t(dddef, _CMU(Algorithm)).for_each(Converter<algorithm>(description,&ctxt));

  /// This should be the end of all processing....close the geometry
  if ( --num_calls == 0 && close_geometry )  {
    Converter<vis_apply> cnv(description,&ctxt);
    cnv(dddef);
    description.endDocument();
  }
  ctxt.namespaces.pop_back();
  printout(INFO,"DDDefinition","+++ Finished processing %s",fname.c_str());
}

static long load_dddefinition(Detector& description, xml_h element) {
  Converter<dddefinition>converter(description);
  converter(element);
  return 1;
}

// Now declare the factory entry for the plugin mechanism
DECLARE_XML_DOC_READER(DDDefinition,load_dddefinition)
