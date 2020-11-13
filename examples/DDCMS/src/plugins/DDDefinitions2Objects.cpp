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
#include "DD4hep/DetectorHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/GeoHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/detail/SegmentationsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/ObjectsInterna.h"

#include "XML/Utilities.h"
#include "DDCMS/DDCMS.h"

// Root/TGeo include files
#include "TSystem.h"
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
    double convertRadToDeg(double r)   {
      return r*360e0/(2e0*M_PI);
    }
    class disabled_algo;
    class include_constants;
    class include_load;
    class include_unload;
    class print_xml_doc;
    class constantssection;
    class constant;
    class resolve   {
    public:
      std::vector<xml::Document> includes;
      std::map<std::string,std::string>  unresolvedConst, allConst, originalConst;
    };

    class materialsection;
    class elementaryelement;
    class elementarymaterial;
    class compositematerial;
  
    class rotationsection;
    class rotation;
    class transform3d;

    class pospartsection;
    class division;
    class pospart;

    class logicalpartsection;
    class logicalpart;

    class solidsection;
    class extrudedpolygon;
    class shapeless;
    class trapezoid;
    class ellipsoid;
    class ellipticaltube;
    class pseudotrap;
    class polyhedra;
    class polycone;
    class torus;
    class trd1;
    class trunctubs;
    class cuttubs;
    class tubs;
    class orb;
    class box;
    class cone;
    class sphere;
    class unionsolid;
    class intersectionsolid;
    class subtractionsolid;
    
    class algorithm;    

    class vissection;
    class vis;
    class debug;
  }

  /// Converter instances implemented in this compilation unit
  template <> void Converter<debug>::operator()(xml_h element) const;
  template <> void Converter<print_xml_doc>::operator()(xml_h element) const;
  template <> void Converter<disabled_algo>::operator()(xml_h element) const;
  
  /// Converter for <ConstantsSection/> tags
  template <> void Converter<constantssection>::operator()(xml_h element) const;
  template <> void Converter<constant>::operator()(xml_h element) const;
  template <> void Converter<resolve>::operator()(xml_h element) const;

  /// Converter for <VisSection/> tags
  template <> void Converter<vissection>::operator()(xml_h element) const;
  /// Convert compact visualization attributes
  template <> void Converter<vis>::operator()(xml_h element) const;

  /// Converter for <MaterialSection/> tags
  template <> void Converter<materialsection>::operator()(xml_h element) const;
  template <> void Converter<elementaryelement>::operator()(xml_h element) const;
  template <> void Converter<elementarymaterial>::operator()(xml_h element) const;
  template <> void Converter<compositematerial>::operator()(xml_h element) const;

  /// Converter for <RotationSection/> tags
  template <> void Converter<rotationsection>::operator()(xml_h element) const;
  /// Converter for <Rotation/> tags
  template <> void Converter<rotation>::operator()(xml_h element) const;
  template <> void Converter<transform3d>::operator()(xml_h element) const;

  /// Generic converter for  <LogicalPartSection/> tags
  template <> void Converter<logicalpartsection>::operator()(xml_h element) const;
  template <> void Converter<logicalpart>::operator()(xml_h element) const;

  /// Converter for <PosPartSection/> tags
  template <> void Converter<pospartsection>::operator()(xml_h element) const;
  /// Converter for <PosPart/> tags
  template <> void Converter<pospart>::operator()(xml_h element) const;
  /// Converter for <Division/> tags
  template <> void Converter<division>::operator()(xml_h element) const;

  /// Generic converter for solids: <SolidSection/> tags
  template <> void Converter<solidsection>::operator()(xml_h element) const;
  /// Converter for <UnionSolid/> tags
  template <> void Converter<unionsolid>::operator()(xml_h element) const;
  /// Converter for <SubtractionSolid/> tags
  template <> void Converter<subtractionsolid>::operator()(xml_h element) const;
  /// Converter for <IntersectionSolid/> tags
  template <> void Converter<intersectionsolid>::operator()(xml_h element) const;
  /// Converter for <PseudoTrap/> tags
  template <> void Converter<pseudotrap>::operator()(xml_h element) const;
  /// Converter for <ExtrudedPolygon/> tags
  template <> void Converter<extrudedpolygon>::operator()(xml_h element) const;
  /// Converter for <ShapelessSolid/> tags
  template <> void Converter<shapeless>::operator()(xml_h element) const;
  /// Converter for <Trapezoid/> tags
  template <> void Converter<trapezoid>::operator()(xml_h element) const;
  /// Converter for <Polyhedra/> tags
  template <> void Converter<polyhedra>::operator()(xml_h element) const;
  /// Converter for <Polycone/> tags
  template <> void Converter<polycone>::operator()(xml_h element) const;
  /// Converter for <Ellipsoid/> tags
  template <> void Converter<ellipsoid>::operator()(xml_h element) const;
  /// Converter for <EllipticalTube/> tags
  template <> void Converter<ellipticaltube>::operator()(xml_h element) const;
  /// Converter for <Torus/> tags
  template <> void Converter<torus>::operator()(xml_h element) const;
  /// Converter for <Tubs/> tags
  template <> void Converter<tubs>::operator()(xml_h element) const;
  /// Converter for <CutTubs/> tags
  template <> void Converter<cuttubs>::operator()(xml_h element) const;
  /// Converter for <TruncTubs/> tags
  template <> void Converter<trunctubs>::operator()(xml_h element) const;
  /// Converter for <Sphere/> tags
  template <> void Converter<sphere>::operator()(xml_h element) const;
  /// Converter for <Trd1/> tags
  template <> void Converter<trd1>::operator()(xml_h element) const;
  /// Converter for <Cone/> tags
  template <> void Converter<cone>::operator()(xml_h element) const;
  /// Converter for <Box/> tags
  template <> void Converter<box>::operator()(xml_h element) const;
  /// Converter for <Orb/> tags
  template <> void Converter<orb>::operator()(xml_h element) const;

  /// Converter for <Algorithm/> tags
  template <> void Converter<algorithm>::operator()(xml_h element) const;

  /// DD4hep specific: Load include file
  template <> void Converter<include_load>::operator()(xml_h element) const;
  /// DD4hep specific: Unload include file
  template <> void Converter<include_unload>::operator()(xml_h element) const;
  /// DD4hep specific: Process constants objects
  template <> void Converter<include_constants>::operator()(xml_h element) const;
}

/// Converter for <ConstantsSection/> tags
template <> void Converter<constantssection>::operator()(xml_h element) const  {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(Constant)).for_each(Converter<constant>(description,_ns.context,optional));
}

/// Converter for <VisSection/> tags
template <> void Converter<vissection>::operator()(xml_h element) const  {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(vis)).for_each(Converter<vis>(description,_ns.context,optional));
}

/// Converter for <MaterialSection/> tags
template <> void Converter<materialsection>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(ElementaryMaterial)).for_each(Converter<elementaryelement>(description,_ns.context,optional));
  xml_coll_t(element, _CMU(ElementaryMaterial)).for_each(Converter<elementarymaterial>(description,_ns.context,optional));
  xml_coll_t(element, _CMU(CompositeMaterial)).for_each(Converter<compositematerial>(description,_ns.context,optional));
}

template <> void Converter<rotationsection>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(Rotation)).for_each(Converter<rotation>(description,_ns.context,optional));
  xml_coll_t(element, _CMU(ReflectionRotation)).for_each(Converter<rotation>(description,_ns.context,optional));
}

template <> void Converter<pospartsection>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(Division)).for_each(Converter<division>(description,_ns.context,optional));
  xml_coll_t(element, _CMU(PosPart)).for_each(Converter<pospart>(description,_ns.context,optional));
  xml_coll_t(element, _CMU(Algorithm)).for_each(Converter<algorithm>(description,_ns.context,optional));
}

/// Generic converter for  <LogicalPartSection/> tags
template <> void Converter<logicalpartsection>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>(), element);
  xml_coll_t(element, _CMU(LogicalPart)).for_each(Converter<logicalpart>(description,_ns.context,optional));
}

template <> void Converter<disabled_algo>::operator()(xml_h element) const   {
  ParsingContext* c = _param<ParsingContext>();
  c->disabledAlgs.insert(element.attr<string>(_U(name)));
}

/// Generic converter for  <SolidSection/> tags
template <> void Converter<solidsection>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>(), element);
  for(xml_coll_t solid(element, _U(star)); solid; ++solid)   {
    string tag = solid.tag();
    if ( tag == "Box" )
      Converter<box>(description,_ns.context,optional)(solid);
    else if ( tag == "Polycone" )
      Converter<polycone>(description,_ns.context,optional)(solid);
    else if ( tag == "Polyhedra" )
      Converter<polyhedra>(description,_ns.context,optional)(solid);
    else if ( tag == "Tubs" )
      Converter<tubs>(description,_ns.context,optional)(solid);
    else if ( tag == "CutTubs" )
      Converter<cuttubs>(description,_ns.context,optional)(solid);
    else if ( tag == "TruncTubs" )
      Converter<trunctubs>(description,_ns.context,optional)(solid);
    else if ( tag == "Tube" )
      Converter<tubs>(description,_ns.context,optional)(solid);
    else if ( tag == "Trd1" )
      Converter<trd1>(description,_ns.context,optional)(solid);
    else if ( tag == "Cone" )
      Converter<cone>(description,_ns.context,optional)(solid);
    else if ( tag == "Sphere" )
      Converter<sphere>(description,_ns.context,optional)(solid);
    else if ( tag == "Ellipsoid" )
      Converter<ellipsoid>(description,_ns.context,optional)(solid);
    else if ( tag == "EllipticalTube" )
      Converter<ellipticaltube>(description,_ns.context,optional)(solid);
    else if ( tag == "Orb" )
      Converter<orb>(description,_ns.context,optional)(solid);
    else if ( tag == "Torus" )
      Converter<torus>(description,_ns.context,optional)(solid);
    else if ( tag == "PseudoTrap" )
      Converter<pseudotrap>(description,_ns.context,optional)(solid);
    else if ( tag == "ExtrudedPolygon" )
      Converter<extrudedpolygon>(description,_ns.context,optional)(solid);
    else if ( tag == "Trapezoid" )
      Converter<trapezoid>(description,_ns.context,optional)(solid);
    else if ( tag == "UnionSolid" )
      Converter<unionsolid>(description,_ns.context,optional)(solid);
    else if ( tag == "SubtractionSolid" )
      Converter<subtractionsolid>(description,_ns.context,optional)(solid);
    else if ( tag == "IntersectionSolid" )
      Converter<intersectionsolid>(description,_ns.context,optional)(solid);
    else if ( tag == "ShapelessSolid" )
      Converter<shapeless>(description,_ns.context,optional)(solid);
    else  {
      string nam = xml_dim_t(solid).nameStr();
      printout(ERROR,"DDCMS","+++ Request to process unknown shape '%s' [%s]",
               nam.c_str(), tag.c_str());
    }
  }
}

/// Converter for <Constant/> tags
template <> void Converter<constant>::operator()(xml_h element) const  {
  Namespace _ns(_param<ParsingContext>());
  resolve*  res  = _option<resolve>();
  xml_dim_t constant = element;
  xml_dim_t par  = constant.parent();
  bool      eval = par.hasAttr(_U(eval)) ? par.attr<bool>(_U(eval)) : false;
  string    val  = constant.valueStr();
  string    nam  = constant.nameStr();
  string    real = _ns.prepend(nam);
  string    typ  = eval ? "number" : "string";
  size_t    idx  = val.find('[');
  
  if ( constant.hasAttr(_U(type)) )
    typ = constant.typeStr();

  if ( idx == string::npos || typ == "string" )  {
    try  {
      _ns.addConstant(nam, val, typ);
      res->allConst[real] = val;
      res->originalConst[real] = val;
    }
    catch(const exception& e)   {
      printout(INFO,"DDCMS","++ Unresolved constant: %s = %s [%s]. Try to resolve later. [%s]",
               real.c_str(), val.c_str(), typ.c_str(), e.what());
    }
    return;
  }
  // Setup the resolution mechanism in Converter<resolve>
  while ( idx != string::npos )  {
    ++idx;
    size_t idp = val.find(':',idx);
    size_t idq = val.find(']',idx);
    if ( idp == string::npos || idp > idq )
      val.insert(idx,_ns.name);
    else if ( idp != string::npos && idp < idq )
      val[idp] = NAMESPACE_SEP;
    idx = val.find('[',idx);
  }
  //MSF NS while ( (idx=val.find(':')) != string::npos ) val[idx]=NAMESPACE_SEP;
  printout(_ns.context->debug.constants ? ALWAYS : DEBUG,
           "Constant","Unresolved: %s -> %s",real.c_str(),val.c_str());
  res->allConst[real] = val;
  res->originalConst[real] = val;
  res->unresolvedConst[real] = val;
}

/** Convert compact visualization attribute to Detector visualization attribute
 *
 *  <vis name="SiVertexBarrelModuleVis"
 *       alpha="1.0" r="1.0" g="0.75" b="0.76"
 *       drawingStyle="wireframe"
 *       showDaughters="false"
 *       visible="true"/>
 */
template <> void Converter<vis>::operator()(xml_h e) const   {
  Namespace _ns(_param<ParsingContext>());
  VisAttr attr(e.attr<string>(_U(name)));
  xml_dim_t dim(e);
  float alpha = dim.alpha(1.0);
  float red   = dim.r(1.0);
  float green = dim.g(1.0);
  float blue  = dim.b(1.0);

  printout(_ns.context->debug.visattr ? ALWAYS : DEBUG, "Compact",
           "++ Converting VisAttr  structure: %-16s. R=%.3f G=%.3f B=%.3f",
           attr.name(), red, green, blue);
  attr.setColor(alpha, red, green, blue);
  if ( e.hasAttr(_U(visible)) )
    attr.setVisible(e.attr<bool>(_U(visible)));
  if ( e.hasAttr(_U(lineStyle)) ) {
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
template <> void Converter<elementaryelement>::operator()(xml_h element) const   {
  Namespace     _ns(_param<ParsingContext>());
  xml_dim_t     xmat(element);
  string        nam = _ns.prepend(xmat.nameStr());
  TGeoManager&  mgr = description.manager();
  TGeoElementTable* tab = mgr.GetElementTable();
  TGeoElement*      elt1 = tab->FindElement(xmat.nameStr().c_str());
  TGeoElement*      elt2 = tab->FindElement(nam.c_str());
  static bool first = true;
  if ( first && _ns.context->debug.materials )   {
    first = false;
    printout(ALWAYS, "DDCMS"," +++ Units: gram:   %7.3f ",dd4hep::g);
    printout(ALWAYS, "DDCMS"," +++ Units: cm3:    %7.3f ",dd4hep::cm3);
    printout(ALWAYS, "DDCMS"," +++ Units: cm:     %7.3f ",dd4hep::cm);
    printout(ALWAYS, "DDCMS"," +++ Units: mole:   %7.3f ",dd4hep::mole);
    printout(ALWAYS, "DDCMS"," +++ Units: g/cm3:  %7.3f ",dd4hep::g/dd4hep::cm3);
    printout(ALWAYS, "DDCMS"," +++ Units: g/mole: %7.3f ",dd4hep::g/dd4hep::mole);    
  }
  if ( !elt1 || !elt2 )  {
    double atomicNumber = xmat.attr<double>(_CMU(atomicNumber));
    double atomicWeight = xmat.attr<double>(_CMU(atomicWeight))/(dd4hep::g/dd4hep::mole);
    int n = int(atomicNumber);
    printout(_ns.context->debug.materials ? ALWAYS : DEBUG, "DDCMS",
             "+++ Converting element  %-32s  atomic number: %3d  weight: %8.3f   [g/mol]",
             ('"'+nam+'"').c_str(), n, atomicWeight);
    elt1 = new TGeoElement(nam.c_str(),"CMS element", n, atomicWeight);
    tab->AddElement(elt1);
  }
}

/// Converter for <ElementaryMaterial/> tags
template <> void Converter<elementarymaterial>::operator()(xml_h element) const   {
  Namespace     _ns(_param<ParsingContext>());
  xml_dim_t     xmat(element);
  string        nam = _ns.prepend(xmat.nameStr());
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(nam.c_str());
  if ( 0 == mat )   {
    const char* matname = nam.c_str();
    double density      = xmat.density() / (dd4hep::g/dd4hep::cm3);
    //double atomicWeight = xmat.attr<double>(_CMU(atomicWeight));
    //double atomicNumber = xmat.attr<double>(_CMU(atomicNumber));
    TGeoElementTable* tab = mgr.GetElementTable();
    TGeoMixture*      mix = new TGeoMixture(nam.c_str(), 1, density);
    TGeoElement*      elt = tab->FindElement(nam.c_str());

    printout(_ns.context->debug.materials ? ALWAYS : DEBUG, "DDCMS",
             "+++ Converting material %-48s  Density: %8.3f [g/cm3] ROOT: %8.3f [g/cm3]",
             ('"'+nam+'"').c_str(), density, mix->GetDensity());

    if ( !elt )  {
      printout(WARNING,"DDCMS",
               "+++ Converter<ElementaryMaterial> No element present with name:%s  [FAKE IT]",
               matname);
    }
#if 0
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
      elt = new TGeoElement((xmat.nameStr()+"-CMS").c_str(),"CMS element", n, atomicNumber);
    }
#endif
    mix->AddElement(elt, 1.0);
    mix->SetRadLen(0e0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,12,0)
    mix->ComputeDerivedQuantities();
#endif
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
  Namespace     _ns(_param<ParsingContext>());
  xml_dim_t     xmat(element);
  string        nam = _ns.prepend(xmat.nameStr());
  TGeoManager&  mgr = description.manager();
  TGeoMaterial* mat = mgr.GetMaterial(nam.c_str());
  if ( 0 == mat )   {
    const char*  matname = nam.c_str();
    double       density = xmat.density() / (dd4hep::g/dd4hep::cm3);
    xml_coll_t   composites(xmat,_CMU(MaterialFraction));
    TGeoMixture* mix = new TGeoMixture(nam.c_str(), composites.size(), density);

    for (composites.reset(); composites; ++composites)   {
      xml_dim_t xfrac(composites);
      xml_dim_t xfrac_mat(xfrac.child(_CMU(rMaterial)));
      double    fraction = xfrac.fraction();
      string    fracname = _ns.real_name(xfrac_mat.nameStr());

      TGeoMaterial* frac_mat = mgr.GetMaterial(fracname.c_str());
      if ( frac_mat )  {
        mix->AddElement(frac_mat, fraction);
        continue;
      }
      printout(WARNING,"DDCMS","+++ Composite material \"%s\" not present!",
               fracname.c_str());
    }
    mix->SetRadLen(0e0);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,12,0)
    mix->ComputeDerivedQuantities();
#endif
    printout(_ns.context->debug.materials ? ALWAYS : DEBUG, "DDCMS",
             "++  Converting material %-48s  Density: %8.3f [g/cm3] ROOT: %8.3f [g/cm3]",
             ('"'+nam+'"').c_str(), density, mix->GetDensity());
    
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
  Namespace  _ns(ctx);
  xml_dim_t  xrot(element);
  string     nam    = xrot.nameStr();
  PrintLevel lvl = ctx->debug.rotations ? ALWAYS : DEBUG;
  double     thetaX = xrot.hasAttr(_CMU(thetaX)) ? _ns.attr<double>(xrot,_CMU(thetaX)) : 0e0;
  double     phiX   = xrot.hasAttr(_CMU(phiX))   ? _ns.attr<double>(xrot,_CMU(phiX))   : 0e0;
  double     thetaY = xrot.hasAttr(_CMU(thetaY)) ? _ns.attr<double>(xrot,_CMU(thetaY)) : 0e0;
  double     phiY   = xrot.hasAttr(_CMU(phiY))   ? _ns.attr<double>(xrot,_CMU(phiY))   : 0e0;
  double     thetaZ = xrot.hasAttr(_CMU(thetaZ)) ? _ns.attr<double>(xrot,_CMU(thetaZ)) : 0e0;
  double     phiZ   = xrot.hasAttr(_CMU(phiZ))   ? _ns.attr<double>(xrot,_CMU(phiZ))   : 0e0;
  Rotation3D rot = make_rotation3D(thetaX, phiX, thetaY, phiY, thetaZ, phiZ);

  _ns.addRotation(nam, rot);
  if ( isActivePrintLevel(lvl) )   {
    double det;
    Position x, y, z;
    stringstream str;
    rot.GetComponents(x,y,z);
    det = (x.Cross(y)).Dot(z);
    str << "+++ rotation: " << nam
	<< " " << ((det>=0) ? "RIGHT" : "LEFT") << "-handed "
	<< scientific << setprecision(2) << setw(9) << x << y << z << rot;
    printout(lvl, "DDCMS",
	     "+++ Adding rotation: %-18s: (theta/phi)[rad] X: %6.3f %6.3f Y: %6.3f %6.3f Z: %6.3f %6.3f",
	     _ns.prepend(nam).c_str(),thetaX,phiX,thetaY,phiY,thetaZ,phiZ);
    printout(lvl, "DDCMS", str.str().c_str());
  }
}

/// Converter for <Logicalpart/> tags
template <> void Converter<logicalpart>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string    sol = e.child(_CMU(rSolid)).attr<string>(_U(name));
  string    mat = e.child(_CMU(rMaterial)).attr<string>(_U(name));
  _ns.addVolume(Volume(e.nameStr(), _ns.solid(sol), _ns.material(mat)));
}

/// Helper converter
template <> void Converter<transform3d>::operator()(xml_h element) const {
  Namespace    _ns(_param<ParsingContext>());
  Transform3D* tr = _option<Transform3D>();
  xml_dim_t   e(element);
  xml_dim_t   translation = e.child(_CMU(Translation),false);
  xml_dim_t   rotation    = e.child(_CMU(Rotation),false);
  xml_dim_t   rRotation   = e.child(_CMU(rRotation),false);
  xml_dim_t   rReflectionRotation = e.child(_CMU(rReflectionRotation),false);
  Position    pos;
  Rotation3D  rot;

  if ( translation.ptr() )   {
    double x = _ns.attr<double>(translation,_U(x));
    double y = _ns.attr<double>(translation,_U(y));
    double z = _ns.attr<double>(translation,_U(z));
    pos = Position(x,y,z);
  }
  if ( rotation.ptr() )   {
    double x = _ns.attr<double>(rotation,_U(x));
    double y = _ns.attr<double>(rotation,_U(y));
    double z = _ns.attr<double>(rotation,_U(z));
    rot = RotationZYX(z,y,x);
  }
  else if ( rRotation.ptr() )   {
    rot = _ns.rotation(rRotation.nameStr());
  }
  else if ( rReflectionRotation.ptr() )   {
    rot = _ns.rotation(rReflectionRotation.nameStr());
  }
  *tr = Transform3D(rot,pos);
}

/// Converter for <PosPart/> tags
template <> void Converter<pospart>::operator()(xml_h element) const {
  Namespace  _ns(_param<ParsingContext>());
  xml_dim_t  e(element);
  int        copy        = e.attr<int>(_CMU(copyNumber));
  string     parent_nam  = _ns.attr<string>(e.child(_CMU(rParent)),_U(name));
  string     child_nam   = _ns.attr<string>(e.child(_CMU(rChild)),_U(name));
  Volume     parent      = _ns.volume(parent_nam);
  Volume     child       = _ns.volume(child_nam, false);
  PrintLevel lvl         = _ns.context->debug.placements ? ALWAYS : DEBUG;
  bool       dbg         = isActivePrintLevel(lvl);

  if ( dbg )   {
    printout(lvl, "DDCMS", "+++ %s Parent: %-24s [%s] Child: %-32s [%s] copy:%d",
           e.tag().c_str(),
           parent_nam.c_str(), parent.isValid() ? "VALID" : "INVALID",
           child_nam.c_str(),  child.isValid()  ? "VALID" : "INVALID",
           copy);
  }
  PlacedVolume pv;
  if ( child.isValid() )   {
    Transform3D trafo;
    Converter<transform3d>(description,param,&trafo)(element);
    pv = parent.placeVolume(child, copy, trafo);
    if ( dbg )   {
      stringstream str;
      Rotation3D   r;
      Position     p, x, y, z;
      xml_dim_t    rRotation = e.child(_CMU(rRotation),false);
      xml_dim_t    rReflectionRotation = e.child(_CMU(rReflectionRotation),false);
      trafo.GetDecomposition(r, p);
      r.GetComponents(x,y,z);
      double det = (x.Cross(y)).Dot(z);
      str << "+++ Rotation: " << (const char*)((det >=0) ? "RIGHT" : "LEFT") << "-handed  ";
      if ( rRotation.ptr() ) str << rRotation.nameStr();
      if ( rReflectionRotation.ptr() ) str << rReflectionRotation.nameStr();
      str << "  Pos: " << p << "  Rotation:" << r;
      printout(lvl, "DDCMS", str.str().c_str());
    }
  }
  if ( !pv.isValid() )   {
    printout(ERROR,"DDCMS","+++ Placement FAILED! Parent:%s Child:%s Valid:%s",
             parent.name(), child_nam.c_str(), yes_no(child.isValid()));
  }
}

template <typename TYPE>
static void convert_boolean(ParsingContext* ctx, xml_h element)   {
  Namespace   _ns(ctx);
  xml_dim_t   e(element);
  string      nam = e.nameStr();
  Solid       solids[2];
  Solid       boolean;
  int         cnt = 0;

  if ( e.hasChild(_CMU(rSolid)) )  {   // Old version
    for(xml_coll_t c(element, _CMU(rSolid)); cnt<2 && c; ++c, ++cnt)
      solids[cnt] = _ns.solid(c.attr<string>(_U(name)));
  }
  else  {
    if ( (solids[0] = _ns.solid(e.attr<string>(_CMU(firstSolid)))).isValid() ) ++cnt;
    if ( (solids[1] = _ns.solid(e.attr<string>(_CMU(secondSolid)))).isValid() ) ++cnt;
  }
  if ( cnt != 2 )   {
    except("DDCMS","+++ Failed to create boolean solid %s. Found only %d parts.",nam.c_str(), cnt);
  }
  printout(_ns.context->debug.placements ? ALWAYS : DEBUG, "DDCMS",
           "+++ BooleanSolid: %s Left: %-32s Right: %-32s",
           nam.c_str(), solids[0]->GetName(), solids[1]->GetName());

  if ( solids[0].isValid() && solids[1].isValid() )  {
    Transform3D trafo;
    Converter<transform3d>(*ctx->description,ctx,&trafo)(element);
    boolean = TYPE(solids[0],solids[1],trafo);
  }
  if ( !boolean.isValid() )
    except("DDCMS","+++ FAILED to construct subtraction solid: %s",nam.c_str());
  _ns.addSolid(nam,boolean);
}

/// Converter for <SubtractionSolid/> tags
template <> void Converter<unionsolid>::operator()(xml_h element) const   {
  convert_boolean<UnionSolid>(_param<ParsingContext>(),element);
}

/// Converter for <SubtractionSolid/> tags
template <> void Converter<subtractionsolid>::operator()(xml_h element) const   {
  convert_boolean<SubtractionSolid>(_param<ParsingContext>(),element);
}

/// Converter for <SubtractionSolid/> tags
template <> void Converter<intersectionsolid>::operator()(xml_h element) const   {
  convert_boolean<IntersectionSolid>(_param<ParsingContext>(),element);
}

/// Converter for <Polycone/> tags
template <> void Converter<polycone>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  vector<double> z, rmin, rmax;
  
  for(xml_coll_t zplane(element, _CMU(RZPoint)); zplane; ++zplane)   {
    rmin.push_back(0.0);
    rmax.push_back(_ns.attr<double>(zplane,_U(r)));
    z.push_back(_ns.attr<double>(zplane,_U(z)));
  }
  for(xml_coll_t zplane(element, _CMU(ZSection)); zplane; ++zplane)   {
    rmin.push_back(_ns.attr<double>(zplane,_CMU(rMin)));
    rmax.push_back(_ns.attr<double>(zplane,_CMU(rMax)));
    z.push_back(_ns.attr<double>(zplane,_U(z)));
  }
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Polycone: startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]  %4ld z-planes",
           startPhi, deltaPhi, z.size());
  _ns.addSolid(nam, Polycone(startPhi,deltaPhi,rmin,rmax,z));
}

/// Converter for <ExtrudedPolygon/> tags
template <> void Converter<extrudedpolygon>::operator()(xml_h element) const  {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  vector<double> pt_x, pt_y, sec_x, sec_y, sec_z, sec_scale;
  
  for(xml_coll_t sec(element, _CMU(ZXYSection)); sec; ++sec)   {
    sec_z.push_back(_ns.attr<double>(sec,_U(z)));
    sec_x.push_back(_ns.attr<double>(sec,_U(x)));
    sec_y.push_back(_ns.attr<double>(sec,_U(y)));
    sec_scale.push_back(_ns.attr<double>(sec,_CMU(scale),1.0));
  }
  for(xml_coll_t pt(element, _CMU(XYPoint)); pt; ++pt)   {
    pt_x.push_back(_ns.attr<double>(pt,_U(x)));
    pt_y.push_back(_ns.attr<double>(pt,_U(y)));
  }
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   ExtrudedPolygon: %4ld points %4ld zxy sections",
           pt_x.size(), sec_z.size());
  _ns.addSolid(nam,ExtrudedPolygon(pt_x,pt_y,sec_z,sec_x,sec_y,sec_scale));
}

/// Converter for <Polyhedra/> tags
template <> void Converter<polyhedra>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double numSide  = _ns.attr<int>(e,_CMU(numSide));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  vector<double> z, rmin, rmax;
  
  for(xml_coll_t zplane(element, _CMU(RZPoint)); zplane; ++zplane)   {
    rmin.push_back(0.0);
    rmax.push_back(_ns.attr<double>(zplane,_U(r)));
    z.push_back(_ns.attr<double>(zplane,_U(z)));
  }
  for(xml_coll_t zplane(element, _CMU(ZSection)); zplane; ++zplane)   {
    rmin.push_back(_ns.attr<double>(zplane,_CMU(rMin)));
    rmax.push_back(_ns.attr<double>(zplane,_CMU(rMax)));
    z.push_back(_ns.attr<double>(zplane,_U(z)));
  }
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Polyhedra:startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]  %4d sides %4ld z-planes",
           startPhi, deltaPhi, numSide, z.size());
  _ns.addSolid(nam, Polyhedra(numSide,startPhi,deltaPhi,z,rmin,rmax));
}

/// Converter for <Sphere/> tags
template <> void Converter<sphere>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double rinner   = _ns.attr<double>(e,_CMU(innerRadius));
  double router   = _ns.attr<double>(e,_CMU(outerRadius));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  double startTheta = _ns.attr<double>(e,_CMU(startTheta));
  double deltaTheta = _ns.attr<double>(e,_CMU(deltaTheta));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Sphere:   r_inner=%8.3f [cm] r_outer=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f startTheta=%8.3f delteTheta=%8.3f [rad]",
           rinner, router, startPhi, deltaPhi, startTheta, deltaTheta);
  _ns.addSolid(nam, Sphere(rinner, router, startTheta, deltaTheta, startPhi, deltaPhi));
}

/// Converter for <Ellipsoid/> tags: Same as sphere, but with scale
template <> void Converter<ellipsoid>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam  = e.nameStr();
  double dx   = _ns.attr<double>(e,_CMU(xSemiAxis));
  double dy   = _ns.attr<double>(e,_CMU(ySemiAxis));
  double dz   = _ns.attr<double>(e,_CMU(zSemiAxis));
  double zBot = _ns.attr<double>(e,_CMU(zBottomCut),0.0);
  double zTop = _ns.attr<double>(e,_CMU(zTopCut),0.0);

  printout(WARNING, "DDCMS",
           "+   Ellipsoid UNSUPPORTED. '%s' REPLACED BY BOX "
           " xSemiAxis=%8.3f ySemiAxis=%8.3f zSemiAxis=%8.3f zBottomCut=%8.3f zTopCut=%8.3f [cm]",
           nam.c_str(), dx, dy, dz, zBot, zTop);
  _ns.addSolid(nam, Box(1,1,1));
}

/// Converter for <Torus/> tags
template <> void Converter<torus>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double r        = _ns.attr<double>(e,_CMU(torusRadius));
  double rinner   = _ns.attr<double>(e,_CMU(innerRadius));
  double router   = _ns.attr<double>(e,_CMU(outerRadius));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Torus:    r=%8.3f [cm] r_inner=%8.3f [cm] r_outer=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]",
           r, rinner, router, startPhi, deltaPhi);
  _ns.addSolid(nam, Torus(r, rinner, router, startPhi, deltaPhi));
}

/// Converter for <Pseudotrap/> tags
template <> void Converter<pseudotrap>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dx1      = _ns.attr<double>(e,_CMU(dx1));
  double dy1      = _ns.attr<double>(e,_CMU(dy1));
  double dx2      = _ns.attr<double>(e,_CMU(dx2));
  double dy2      = _ns.attr<double>(e,_CMU(dy2));
  double dz       = _ns.attr<double>(e,_U(dz));
  double r        = _ns.attr<double>(e,_U(radius));
  bool   atMinusZ = _ns.attr<bool>  (e,_CMU(atMinusZ));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Pseudotrap:  dz=%8.3f [cm] dx1:%.3f dy1:%.3f dx2=%.3f dy2=%.3f radius:%.3f atMinusZ:%s",
           dz, dx1, dy1, dx2, dy2, r, yes_no(atMinusZ));
  _ns.addSolid(nam, PseudoTrap(dx1, dx2, dy1, dy2, dz, r, atMinusZ));
}

/// Converter for <Trapezoid/> tags
template <> void Converter<trapezoid>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dz       = _ns.attr<double>(e,_U(dz));
  double alp1     = _ns.attr<double>(e,_CMU(alp1));
  double bl1      = _ns.attr<double>(e,_CMU(bl1));
  double tl1      = _ns.attr<double>(e,_CMU(tl1));
  double h1       = _ns.attr<double>(e,_CMU(h1));
  double alp2     = _ns.attr<double>(e,_CMU(alp2));
  double bl2      = _ns.attr<double>(e,_CMU(bl2));
  double tl2      = _ns.attr<double>(e,_CMU(tl2));
  double h2       = _ns.attr<double>(e,_CMU(h2));
  double phi      = _ns.attr<double>(e,_U(phi),0.0);
  double theta    = _ns.attr<double>(e,_U(theta),0.0);
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Trapezoid:  dz=%8.3f [cm] alp1:%.3f bl1=%.3f tl1=%.3f alp2=%.3f bl2=%.3f tl2=%.3f h2=%.3f phi=%.3f theta=%.3f",
           dz, alp1, bl1, tl1, h1, alp2, bl2, tl2, h2, phi, theta);
  _ns.addSolid(nam, Trap(dz, theta, phi, h1, bl1, tl1, alp1, h2, bl2, tl2, alp2));
}

/// Converter for <Trd1/> tags
template <> void Converter<trd1>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dx1      = _ns.attr<double>(e,_CMU(dx1));
  double dy1      = _ns.attr<double>(e,_CMU(dy1));
  double dx2      = _ns.attr<double>(e,_CMU(dx2),0.0);
  double dy2      = _ns.attr<double>(e,_CMU(dy2),0.0);
  double dz       = _ns.attr<double>(e,_CMU(dz));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Trd1:       dz=%8.3f [cm] dx1:%.3f dy1:%.3f dx2:%.3f dy2:%.3f",
           dz, dx1, dy1, dx2, dy2);
  if ( dy1 == dy2 )
    _ns.addSolid(nam, Trd1(dx1, dx2, dy1, dz));
  else
    _ns.addSolid(nam, Trd2(dx1, dx2, dy1, dy2, dz));
}

/// Converter for <Tubs/> tags
template <> void Converter<tubs>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dz       = _ns.attr<double>(e,_CMU(dz));
  double rmin     = _ns.attr<double>(e,_CMU(rMin));
  double rmax     = _ns.attr<double>(e,_CMU(rMax));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi),0.0);
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi),2*M_PI);
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Tubs:     dz=%8.3f [cm] rmin=%8.3f [cm] rmax=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]", dz, rmin, rmax, startPhi, deltaPhi);
  _ns.addSolid(nam, Tube(rmin,rmax,dz,startPhi,deltaPhi));
}

/// Converter for <CutTubs/> tags
template <> void Converter<cuttubs>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dz       = _ns.attr<double>(e,_CMU(dz));
  double rmin     = _ns.attr<double>(e,_CMU(rMin));
  double rmax     = _ns.attr<double>(e,_CMU(rMax));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  double lx       = _ns.attr<double>(e,_CMU(lx));
  double ly       = _ns.attr<double>(e,_CMU(ly));
  double lz       = _ns.attr<double>(e,_CMU(lz));
  double tx       = _ns.attr<double>(e,_CMU(tx));
  double ty       = _ns.attr<double>(e,_CMU(ty));
  double tz       = _ns.attr<double>(e,_CMU(tz));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   CutTube:  dz=%8.3f [cm] rmin=%8.3f [cm] rmax=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]...",
           dz, rmin, rmax, startPhi, deltaPhi);
  _ns.addSolid(nam, CutTube(rmin,rmax,dz,startPhi,deltaPhi,lx,ly,lz,tx,ty,tz));
}

/// Converter for <TruncTubs/> tags
template <> void Converter<trunctubs>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam        = e.nameStr();
  double zhalf      = _ns.attr<double>(e,_CMU(zHalf));
  double rmin       = _ns.attr<double>(e,_CMU(rMin));
  double rmax       = _ns.attr<double>(e,_CMU(rMax));
  double startPhi   = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi   = _ns.attr<double>(e,_CMU(deltaPhi));
  double cutAtStart = _ns.attr<double>(e,_CMU(cutAtStart));
  double cutAtDelta = _ns.attr<double>(e,_CMU(cutAtDelta));
  bool   cutInside  = _ns.attr<bool>(e,_CMU(cutInside));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   TruncTube:zHalf=%8.3f [cm] rmin=%8.3f [cm] rmax=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f [rad] atStart=%8.3f [cm] atDelta=%8.3f [cm] inside:%s",
           zhalf, rmin, rmax, startPhi, deltaPhi, cutAtStart, cutAtDelta, yes_no(cutInside));
  _ns.addSolid(nam, TruncatedTube(zhalf,rmin,rmax,startPhi,deltaPhi,cutAtStart,cutAtDelta,cutInside));
}

/// Converter for <EllipticalTube/> tags
template <> void Converter<ellipticaltube>::operator()(xml_h element) const   {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  double dx  = _ns.attr<double>(e,_CMU(xSemiAxis));
  double dy  = _ns.attr<double>(e,_CMU(ySemiAxis));
  double dz  = _ns.attr<double>(e,_CMU(zHeight));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   EllipticalTube xSemiAxis=%8.3f [cm] ySemiAxis=%8.3f [cm] zHeight=%8.3f [cm]",dx,dy,dz);
  _ns.addSolid(nam, EllipticalTube(dx,dy,dz));
}

/// Converter for <Cone/> tags
template <> void Converter<cone>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam      = e.nameStr();
  double dz       = _ns.attr<double>(e,_CMU(dz));
  double rmin1    = _ns.attr<double>(e,_CMU(rMin1));
  double rmin2    = _ns.attr<double>(e,_CMU(rMin2));
  double rmax1    = _ns.attr<double>(e,_CMU(rMax1));
  double rmax2    = _ns.attr<double>(e,_CMU(rMax2));
  double startPhi = _ns.attr<double>(e,_CMU(startPhi));
  double deltaPhi = _ns.attr<double>(e,_CMU(deltaPhi));
  double phi2     = startPhi + deltaPhi;
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Cone:     dz=%8.3f [cm]"
           " rmin1=%8.3f [cm] rmax1=%8.3f [cm]"
           " rmin2=%8.3f [cm] rmax2=%8.3f [cm]"
           " startPhi=%8.3f [rad] deltaPhi=%8.3f [rad]",
           dz, rmin1, rmax1, rmin2, rmax2, startPhi, deltaPhi);
  _ns.addSolid(nam, ConeSegment(dz,rmin1,rmax1,rmin2,rmax2,startPhi,phi2));
}

/// Converter for </> tags
template <> void Converter<shapeless>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Shapeless: THIS ONE CAN ONLY BE USED AT THE VOLUME LEVEL -> Assembly%s", nam.c_str());
  _ns.addSolid(nam, Box(1,1,1));
}

/// Converter for <Box/> tags
template <> void Converter<box>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  double dx  = _ns.attr<double>(e,_CMU(dx));
  double dy  = _ns.attr<double>(e,_CMU(dy));
  double dz  = _ns.attr<double>(e,_CMU(dz));
  printout(_ns.context->debug.shapes ? ALWAYS : DEBUG, "DDCMS",
           "+   Box:      dx=%8.3f [cm] dy=%8.3f [cm] dz=%8.3f [cm]", dx, dy, dz);
  _ns.addSolid(nam, Box(dx,dy,dz));
}

/// Converter for <Box/> tags
template <> void Converter<orb>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string nam = e.nameStr();
  double r = _ns.attr<double>(e,_U(radius));
  printout(WARNING, "DDCMS",
           "+   Orb UNSUPPORTED. %s REPLACED BY BOX:      r=%8.3f [cm]", nam.c_str(), r);
  _ns.addSolid(nam, Box(r,r,r));
}

/// DD4hep specific Converter for <Include/> tags: process only the constants
template <> void Converter<include_load>::operator()(xml_h element) const   {
  TString fname = element.attr<string>(_U(ref)).c_str();
  const char* path = gSystem->Getenv("DDCMS_XML_PATH");
  xml::Document doc;
  if ( path && gSystem->FindFile(path,fname) )
    doc = xml::DocumentHandler().load(fname.Data());
  else
    doc = xml::DocumentHandler().load(element, element.attr_value(_U(ref)));
  fname = xml::DocumentHandler::system_path(doc.root());
  printout(_param<ParsingContext>()->debug.includes ? ALWAYS : DEBUG,
           "DDCMS","+++ Processing the CMS detector description %s",fname.Data());
  _option<resolve>()->includes.push_back(doc);
}

/// DD4hep specific Converter for <Include/> tags: process only the constants
template <> void Converter<include_unload>::operator()(xml_h element) const   {
  string fname = xml::DocumentHandler::system_path(element);
  xml::DocumentHolder(xml_elt_t(element).document()).assign(0);
  printout(_param<ParsingContext>()->debug.includes ? ALWAYS : DEBUG,
           "DDCMS","+++ Finished processing %s",fname.c_str());
}

/// DD4hep specific Converter for <Include/> tags: process only the constants
template <> void Converter<include_constants>::operator()(xml_h element) const   {
  xml_coll_t(element, _CMU(ConstantsSection)).for_each(Converter<constantssection>(description,param,optional));
}

namespace {

  //  The meaning of the axis index is the following:
  //    for all volumes having shapes like box, trd1, trd2, trap, gtra or para - 1,2,3 means X,Y,Z;
  //    for tube, tubs, cone, cons - 1 means Rxy, 2 means phi and 3 means Z;
  //    for pcon and pgon - 2 means phi and 3 means Z;
  //    for spheres 1 means R and 2 means phi.

  enum class DDAxes { x = 1, y = 2, z = 3, rho = 1, phi = 2, undefined };
  const std::map<std::string, DDAxes> axesmap{{"x", DDAxes::x},
                                              {"y", DDAxes::y},
                                              {"z", DDAxes::z},
                                              {"rho", DDAxes::rho},
                                              {"phi", DDAxes::phi},
                                              {"undefined", DDAxes::undefined}};
}  // namespace

/// Converter for <Division/> tags
template <>
void Converter<division>::operator()(xml_h element) const {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string childName = e.nameStr();
  if (strchr(childName.c_str(), NAMESPACE_SEP) == nullptr)
    childName = _ns.prepend(childName);

  string parentName = _ns.attr<string>(e, _CMU(parent));
  if (strchr(parentName.c_str(), NAMESPACE_SEP) == nullptr)
    parentName = _ns.prepend(parentName);
  string axis = _ns.attr<string>(e, _CMU(axis));

  // If you divide a tube of 360 degrees the offset displaces
  // the starting angle, but you still fill the 360 degrees
  double offset = e.hasAttr(_CMU(offset)) ? _ns.attr<double>(e, _CMU(offset)) : 0e0;
  double width = e.hasAttr(_CMU(width)) ? _ns.attr<double>(e, _CMU(width)) : 0e0;
  int nReplicas = e.hasAttr(_CMU(nReplicas)) ? _ns.attr<int>(e, _CMU(nReplicas)) : 0;

  printout(_ns.context->debug.placements ? ALWAYS : DEBUG,
           "DD4CMS",
           "+++ Start executing Division of %s along %s (%d) with offset %6.3f and %6.3f to produce %s....",
           parentName.c_str(),
           axis.c_str(),
           axesmap.at(axis),
           offset,
           width,
           childName.c_str());

  Volume parent = _ns.volume(parentName);

  const TGeoShape* shape = parent.solid();
  TClass* cl = shape->IsA();
  if (cl == TGeoTubeSeg::Class()) {
    const TGeoTubeSeg* sh = (const TGeoTubeSeg*)shape;
    double widthInDeg = convertRadToDeg(width);
    double startInDeg = convertRadToDeg(offset);
    int numCopies = (int)((sh->GetPhi2() - sh->GetPhi1()) / widthInDeg);

    printout(_ns.context->debug.placements ? ALWAYS : DEBUG,
             "DD4CMS",
             "+++    ...divide %s along %s (%d) with offset %6.3f deg and %6.3f deg to produce %d copies",
             parent.solid().type(),
             axis.c_str(),
             axesmap.at(axis),
             startInDeg,
             widthInDeg,
             numCopies);
    Volume child = parent.divide(childName, static_cast<int>(axesmap.at(axis)), numCopies, startInDeg, widthInDeg);

    _ns.context->volumes[childName] = child;
    printout(_ns.context->debug.placements ? ALWAYS : DEBUG,
             "DD4CMS",
             "+++ %s Parent: %-24s [%s] Child: %-32s [%s] is multivolume [%s]",
             e.tag().c_str(),
             parentName.c_str(),
             parent.isValid() ? "VALID" : "INVALID",
             child.name(),
             child.isValid() ? "VALID" : "INVALID",
             child->IsVolumeMulti() ? "YES" : "NO");
  } else if (cl == TGeoTrd1::Class() ) {
    double dy = static_cast<const TGeoTrd1*>(shape)->GetDy();
    printout(_ns.context->debug.placements ? ALWAYS : DEBUG,
             "DD4CMS",
             "+++    ...divide %s along %s (%d) with offset %6.3f cm and %6.3f cm to produce %d copies in %6.3f",
             parent.solid().type(),
             axis.c_str(),
             axesmap.at(axis),
             -dy + offset + width,
             width,
             nReplicas,
             dy);

    Volume child = parent.divide(childName, static_cast<int>(axesmap.at(axis)), nReplicas, -dy + offset + width, width);

    _ns.context->volumes[childName] = child;
    printout(_ns.context->debug.placements ? ALWAYS : DEBUG,
             "DD4CMS",
             "+++ %s Parent: %-24s [%s] Child: %-32s [%s] is multivolume [%s]",
             e.tag().c_str(),
             parentName.c_str(),
             parent.isValid() ? "VALID" : "INVALID",
             child.name(),
             child.isValid() ? "VALID" : "INVALID",
             child->IsVolumeMulti() ? "YES" : "NO");
  } else {
    printout(ERROR, "DD4CMS", "++ FAILED Division of a %s is not implemented yet!", parent.solid().type());
  }
}

/// Converter for <Algorithm/> tags
template <> void Converter<algorithm>::operator()(xml_h element) const  {
  Namespace _ns(_param<ParsingContext>());
  xml_dim_t e(element);
  string name = e.nameStr();
  if ( _ns.context->disabledAlgs.find(name) != _ns.context->disabledAlgs.end() )   {
    printout(INFO,"DDCMS","+++ Skip disabled algorithms: %s",name.c_str());
    return;
  }
  try {
    size_t            idx;
    SensitiveDetector sd;
    string            type = "DDCMS_"+_ns.real_name(name);
    while ( (idx=type.find(NAMESPACE_SEP)) != string::npos ) type[idx]='_';

    // SensitiveDetector and Segmentation currently are undefined. Let's keep it like this
    // until we found something better.....
    printout(_ns.context->debug.algorithms ? ALWAYS : DEBUG,
             "DDCMS","+++ Start executing algorithm %s....",type.c_str());
    LogDebug context(e.nameStr(),true);
    long ret = PluginService::Create<long>(type, &description, _ns.context, &element, &sd);
    if ( ret == 1 )    {
      printout(_ns.context->debug.algorithms ? ALWAYS : DEBUG,
               "DDCMS", "+++ Executed algorithm: %08lX = %s", ret, name.c_str());
      return;      
    }
#if 0
    DetElement det(PluginService::Create<NamedObject*>(type, &description, _ns.context, &element, &sd));
    if (det.isValid())   {
      // setChildTitles(make_pair(name, det));
      if ( sd.isValid() )   {
        det->flag |= DetElement::Object::HAVE_SENSITIVE_DETECTOR;
      }
      if ( seg.isValid() )   {
        seg->sensitive = sd;
        seg->detector  = det;
      }
    }
    if (!det.isValid())   {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type, &description, _ns.context, &element, &sd);
      except("DDCMS","Failed to execute subdetector creation plugin. " + dbg.missingFactory(type));
    }
    description.addDetector(det);
#endif
    ///description.addDetector(det);
    printout(ERROR, "DDCMS", "++ FAILED  NOT ADDING SUBDETECTOR %08lX = %s",ret, name.c_str());
    return;
  }
  catch (const exception& exc)   {
    printout(ERROR, "DDCMS", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), exc.what());
    terminate();
  }
  catch (...)   {
    printout(ERROR, "DDCMS", "++ FAILED    to convert subdetector: %s: %s", name.c_str(), "UNKNONW Exception");
    terminate();
  }
}

template <> void Converter<debug>::operator()(xml_h dbg) const {
  Namespace _ns(_param<ParsingContext>());
  if ( dbg.hasChild(_CMU(debug_visattr))    ) _ns.context->debug.visattr    = true;
  if ( dbg.hasChild(_CMU(debug_constants))  ) _ns.context->debug.constants  = true;
  if ( dbg.hasChild(_CMU(debug_materials))  ) _ns.context->debug.materials  = true;
  if ( dbg.hasChild(_CMU(debug_rotations))  ) _ns.context->debug.rotations  = true;
  if ( dbg.hasChild(_CMU(debug_shapes))     ) _ns.context->debug.shapes     = true;
  if ( dbg.hasChild(_CMU(debug_volumes))    ) _ns.context->debug.volumes    = true;
  if ( dbg.hasChild(_CMU(debug_placements)) ) _ns.context->debug.placements = true;
  if ( dbg.hasChild(_CMU(debug_namespaces)) ) _ns.context->debug.namespaces = true;
  if ( dbg.hasChild(_CMU(debug_includes))   ) _ns.context->debug.includes   = true;
  if ( dbg.hasChild(_CMU(debug_algorithms)) ) _ns.context->debug.algorithms = true;
  LogDebug::setDebugAlgorithms(_ns.context->debug.algorithms);
}

template <> void Converter<resolve>::operator()(xml_h /* element */) const {
  ParsingContext* ctx = _param<ParsingContext>();
  resolve*        res = _option<resolve>();
  Namespace       _ns(ctx);
  int count = 0;

  printout(ctx->debug.constants ? ALWAYS : DEBUG,
           "DDCMS","+++ RESOLVING %ld unknown constants.....",res->unresolvedConst.size());
  while ( !res->unresolvedConst.empty() )   {
    for(auto i=res->unresolvedConst.begin(); i!=res->unresolvedConst.end(); ++i )   {
      const string& n = (*i).first;
      string  rep;
      string& v   = (*i).second;
      size_t idx, idq;
      for(idx=v.find('[',0); idx != string::npos; idx = v.find('[',idx+1) )   {
        idq = v.find(']',idx+1);
        rep = v.substr(idx+1,idq-idx-1);
        auto r = res->allConst.find(rep);
        if ( r != res->allConst.end() )  {
          rep = "("+(*r).second+")";
          v.replace(idx,idq-idx+1,rep);
        }
      }
      if ( v.find(']') == string::npos )  {
        if ( v.find("-+") != string::npos || v.find("+-") != string::npos )   {
          while ( (idx=v.find("-+")) != string::npos )
            v.replace(idx,2,"-");
          while ( (idx=v.find("+-")) != string::npos )
            v.replace(idx,2,"-");
        }
        printout(ctx->debug.constants ? ALWAYS : DEBUG,
                 "DDCMS","+++ [%06ld] ----------  %-40s = %s",
                 res->unresolvedConst.size()-1,n.c_str(),res->originalConst[n].c_str());
        _ns.addConstantNS(n, v, "number");
        res->unresolvedConst.erase(i);
        break;
      }
    }
    if ( ++count > 1000 ) break;
  }
  if ( !res->unresolvedConst.empty() )   {
    for(const auto& e : res->unresolvedConst )
      printout(ERROR,"DDCMS","+++ Unresolved constant: %-40s = %s.",e.first.c_str(), e.second.c_str());
    except("DDCMS","++ FAILED to resolve %ld constant entries:",res->unresolvedConst.size());
  }
  res->unresolvedConst.clear();
  res->originalConst.clear();
  res->allConst.clear();
}

template <> void Converter<print_xml_doc>::operator()(xml_h element) const {
  string fname = xml::DocumentHandler::system_path(element);
  printout(_param<ParsingContext>()->debug.includes ? ALWAYS : DEBUG,
           "DDCMS","+++ Processing data from: %s",fname.c_str());
}

/// Converter for <DDDefinition/> tags
static long load_dddefinition(Detector& det, xml_h element) {
  xml_elt_t dddef(element);
  if ( dddef )    {
    static ParsingContext ctxt(&det);
    Namespace _ns(ctxt);
    string fname = xml::DocumentHandler::system_path(element);
    bool open_geometry  = dddef.hasChild(_CMU(open_geometry));
    bool close_geometry = dddef.hasChild(_CMU(close_geometry));

    xml_coll_t(dddef, _U(debug)).for_each(Converter<debug>(det,&ctxt));

    // Here we define the order how XML elements are processed.
    // Be aware of dependencies. This can only defined once.
    // At the end it is a limitation of DOM....
    printout(INFO,"DDCMS","+++ Processing the CMS detector description %s",fname.c_str());

    xml::Document doc;
    Converter<print_xml_doc> print_doc(det,&ctxt);
    try  {
      resolve res;
      print_doc((doc=dddef.document()).root());
      xml_coll_t(dddef, _CMU(DisabledAlgo)).for_each(Converter<disabled_algo>(det,&ctxt,&res));
      xml_coll_t(dddef, _CMU(ConstantsSection)).for_each(Converter<constantssection>(det,&ctxt,&res));
      xml_coll_t(dddef, _CMU(VisSection)).for_each(Converter<vissection>(det,&ctxt));
      xml_coll_t(dddef, _CMU(RotationSection)).for_each(Converter<rotationsection>(det,&ctxt));
      xml_coll_t(dddef, _CMU(MaterialSection)).for_each(Converter<materialsection>(det,&ctxt));

      xml_coll_t(dddef, _CMU(IncludeSection)).for_each(_CMU(Include), Converter<include_load>(det,&ctxt,&res));

      for(xml::Document d : res.includes )   {
        print_doc((doc=d).root());
        Converter<include_constants>(det,&ctxt,&res)((doc=d).root());
      }
      // Before we continue, we have to resolve all constants NOW!
      Converter<resolve>(det,&ctxt,&res)(dddef);
      // Now we can process the include files one by one.....
      for(xml::Document d : res.includes )   {
        print_doc((doc=d).root());
        xml_coll_t(d.root(),_CMU(MaterialSection)).for_each(Converter<materialsection>(det,&ctxt));
      }
      if ( open_geometry )  {
        ctxt.geo_inited = true;
        det.init();
        _ns.addVolume(det.worldVolume());
      }
      for(xml::Document d : res.includes )  {
        print_doc((doc=d).root());
        xml_coll_t(d.root(),_CMU(RotationSection)).for_each(Converter<rotationsection>(det,&ctxt));
      }
      for(xml::Document d : res.includes )  {
        print_doc((doc=d).root());
        xml_coll_t(d.root(), _CMU(SolidSection)).for_each(Converter<solidsection>(det,&ctxt));
      }
      for(xml::Document d : res.includes )  {
        print_doc((doc=d).root());
        xml_coll_t(d.root(), _CMU(LogicalPartSection)).for_each(Converter<logicalpartsection>(det,&ctxt));
      }
      for(xml::Document d : res.includes )  {
        print_doc((doc=d).root());
        xml_coll_t(d.root(), _CMU(Algorithm)).for_each(Converter<algorithm>(det,&ctxt));
      }
      for(xml::Document d : res.includes )  {
        print_doc((doc=d).root());
        xml_coll_t(d.root(), _CMU(PosPartSection)).for_each(Converter<pospartsection>(det,&ctxt));
      }

      /// Unload all XML files after processing
      for(xml::Document d : res.includes ) Converter<include_unload>(det,&ctxt,&res)(d.root());

      print_doc((doc=dddef.document()).root());
      // Now process the actual geometry items
      xml_coll_t(dddef, _CMU(SolidSection)).for_each(Converter<solidsection>(det,&ctxt));
      xml_coll_t(dddef, _CMU(LogicalPartSection)).for_each(Converter<logicalpartsection>(det,&ctxt));
      xml_coll_t(dddef, _CMU(Algorithm)).for_each(Converter<algorithm>(det,&ctxt));
      xml_coll_t(dddef, _CMU(PosPartSection)).for_each(Converter<pospartsection>(det,&ctxt));

    }
    catch(const exception& e)   {
      printout(ERROR,"DDCMS","Exception while processing xml source:%s",doc.uri().c_str());
      printout(ERROR,"DDCMS","----> %s", e.what());
      throw;
    }

    /// This should be the end of all processing....close the geometry
    if ( close_geometry )  {
      det.endDocument();
    }
    printout(INFO,"DDDefinition","+++ Finished processing %s",fname.c_str());
    return 1;
  }
  except("DDDefinition","+++ FAILED to process unknown DOM tree [Invalid Handle]");
  return 0;
}

// Now declare the factory entry for the plugin mechanism
DECLARE_XML_DOC_READER(DDDefinition,load_dddefinition)
