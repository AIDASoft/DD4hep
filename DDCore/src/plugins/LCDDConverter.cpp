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
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"
#include "XML/DocumentHandler.h"
#include "LCDDConverter.h"

// ROOT includes
#include "TROOT.h"
#include "TColor.h"
#include "TGeoShape.h"

#include "TGeoArb8.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoEltu.h"
#include "TGeoHype.h"
#include "TGeoMatrix.h"
#include "TGeoParaboloid.h"
#include "TGeoPara.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoShapeAssembly.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TGeoScaledShape.h"
//#include "TGeoEllipsoid.h"

#include "TGeoNode.h"
#include "TClass.h"
#include "TMath.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace dd4hep::detail;
using namespace dd4hep;
using namespace std;
namespace {
  typedef Position XYZRotation;

  XYZRotation getXYZangles(const Double_t* r) {
    Double_t cosb = std::sqrt(r[0]*r[0] + r[1]*r[1]);
    if (cosb > 0.00001) {
      return XYZRotation(atan2(r[5], r[8]), atan2(-r[2], cosb), atan2(r[1], r[0]));
    }
    return XYZRotation(atan2(-r[7], r[4]),atan2(-r[2], cosb),0);
  }

#if 0
  XYZRotation getXYZangles(const Double_t* rotationMatrix) {
    Double_t a, b, c;
    Double_t rad = 1.0;   // RAD by default! 180.0 / TMath::ACos(-1.0);
    const Double_t *r = rotationMatrix;
    Double_t cosb = TMath::Sqrt(r[0] * r[0] + r[1] * r[1]);
    if (cosb > 0.00001) {
      a = TMath::ATan2(r[5], r[8]) * rad;
      b = TMath::ATan2(-r[2], cosb) * rad;
      c = TMath::ATan2(r[1], r[0]) * rad;
    }
    else {
      a = TMath::ATan2(-r[7], r[4]) * rad;
      b = TMath::ATan2(-r[2], cosb) * rad;
      c = 0;
    }
    XYZRotation rr(a, b, c);
    cout << " X:" << a << " " << rr.X() << " Y:" << b << " " << rr.Y() << " Z:" << c << " " << rr.Z()
         << " lx:" << r[0] << " ly:" << r[4] << " lz:" << r[8] << endl;
    return XYZRotation(a, b, c);
  }
#endif

  bool is_volume(const TGeoVolume* volume)  {
    Volume v = Ref_t(volume);
    return v.data() != 0;
  }
  bool is_placement(PlacedVolume node)  {
    PlacedVolume v = Ref_t(node);
    return v.data() != 0;
  }

  string genName(const string& n)  {  return n; }
  string genName(const string& n, const void* ptr)  {
    string nn = genName(n);
    char text[32];
    ::snprintf(text,sizeof(text),"%p",ptr);
    nn += "_";
    nn += text;
    return nn;
  }
}

void LCDDConverter::GeometryInfo::check(const string& name, const TNamed* _n, map<string, const TNamed*>& _m) const {
  map<string, const TNamed*>::const_iterator i = _m.find(name);
  if (i != _m.end()) {
    const char* isa = _n ? _n->IsA()->GetName() : (*i).second ? (*i).second->IsA()->GetName() : "Unknown";
    cout << isa << "(position):  duplicate entry with name:" << name << " " << (void*) _n << " " << (void*) (*i).second << endl;
  }
  _m.insert(make_pair(name, _n));
}

/// Initializing Constructor
LCDDConverter::LCDDConverter(Detector& description)
  : m_detDesc(description), m_dataPtr(0) {
}

LCDDConverter::~LCDDConverter() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

/// Dump element in GDML format to output stream
xml_h LCDDConverter::handleElement(const string& /* name */, Atom element) const {
  GeometryInfo& geo = data();
  xml_h e = geo.xmlElements[element];
  if (!e) {
    int Z = element->Z();
    double A = element->A();
    xml_elt_t atom(geo.doc, _U(atom));
    // If we got an unphysical material (Z<1 or A<1)
    // We pretend it is hydrogen and force Z=1 or A=1.00794 g/mole
    geo.doc_materials.append(e = xml_elt_t(geo.doc, _U(element)));
    e.append(atom);
    e.setAttr(_U(name), element->GetName());
    e.setAttr(_U(formula), element->GetName());
    e.setAttr(_U(Z), Z>0 ? Z : 1);
    atom.setAttr(_U(type), "A");
    atom.setAttr(_U(unit), "g/mol");
    atom.setAttr(_U(value), A>0.99 ? A : 1.00794 /* *(g/mole) */);
    geo.xmlElements[element] = e;
  }
  return e;
}

/// Dump material in GDML format to output stream
xml_h LCDDConverter::handleMaterial(const string& name, Material medium) const {
  GeometryInfo& geo = data();
  xml_h mat = geo.xmlMaterials[medium];
  if (!mat) {
    xml_h obj;
    TGeoMaterial* m = medium->GetMaterial();
    double d = m->GetDensity();   //*(gram/cm3);
    if (d < 1e-10) d = 1e-10;
    mat = xml_elt_t(geo.doc, _U(material));
    mat.setAttr(_U(name), medium->GetName());
    mat.append(obj = xml_elt_t(geo.doc, _U(D)));
    obj.setAttr(_U(value), d /*  *(g/cm3)  */);
    obj.setAttr(_U(unit), "g/cm3");
    obj.setAttr(_U(type), "density");

    geo.checkMaterial(name, medium);

    if (m->IsMixture()) {
      TGeoMixture *mix = (TGeoMixture*) m;
      const double *wmix = mix->GetWmixt();
      const int *nmix = mix->GetNmixt();
      double sum = 0e0;
      for (int i = 0, n = mix->GetNelements(); i < n; i++) {
        TGeoElement *elt = mix->GetElement(i);
        handleElement(elt->GetName(), Atom(elt));
        sum += wmix[i];
      }
      for (int i = 0, n = mix->GetNelements(); i < n; i++) {
        TGeoElement *elt = mix->GetElement(i);
        //string formula = elt->GetTitle() + string("_elm");
        if (nmix) {
          mat.append(obj = xml_elt_t(geo.doc, _U(composite)));
          obj.setAttr(_U(n), nmix[i]);
        }
        else {
          mat.append(obj = xml_elt_t(geo.doc, _U(fraction)));
          obj.setAttr(_U(n), wmix[i] / sum);
        }
        obj.setAttr(_U(ref), elt->GetName());
      }
    }
    else if ( name != "dummy" )   {  
      // Do not exactly know where dummy comes from,
      // but it causes havoc in Geant4 later
      TGeoElement *elt = m->GetElement(0);
      printout(INFO,"++ Converting non mixing material: %s",name.c_str());
      xml_elt_t atom(geo.doc, _U(atom));
      handleElement(elt->GetName(), Atom(elt));
      mat.append(atom);
      mat.setAttr(_U(Z), m->GetZ());
      atom.setAttr(_U(type), "A");
      atom.setAttr(_U(unit), "g/mol");
      atom.setAttr(_U(value), m->GetA() /*  *(g/mole)  */);
    }
    geo.doc_materials.append(mat);
    geo.xmlMaterials[medium] = mat;
  }
  return mat;
}

/// Dump solid in GDML format to output stream
xml_h LCDDConverter::handleSolid(const string& name, const TGeoShape* shape) const {
  GeometryInfo& geo = data();
  SolidMap::iterator sit = geo.xmlSolids.find(shape);
  if (!shape) {
    // This is an invalid volume. Let's pray returning nothing will work,
    // and the non-existing solid is also nowhere referenced in the GDML.
    return xml_h(0);
  }
  else if (sit != geo.xmlSolids.end()) {
    // The solidis already registered. Return the reference
    return (*sit).second;
  }
  else if (shape->IsA() == TGeoShapeAssembly::Class()) {
    // Assemblies have no shape in GDML. Hence, return nothing.
    return xml_h(0);
  }
  else {
    xml_h solid(0);
    xml_h zplane(0);
    TClass* isa = shape->IsA();
    string shape_name = shape->GetName(); //genName(shape->GetName(),shape);
    geo.checkShape(name, shape);
    if (isa == TGeoBBox::Class()) {
      const TGeoBBox* sh = (const TGeoBBox*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(box)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(x), 2 * sh->GetDX());
      solid.setAttr(_U(y), 2 * sh->GetDY());
      solid.setAttr(_U(z), 2 * sh->GetDZ());
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTube::Class()) {
      const TGeoTube* sh = (const TGeoTube*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(tube)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rmin), sh->GetRmin());
      solid.setAttr(_U(rmax), sh->GetRmax());
      solid.setAttr(_U(z), 2 * sh->GetDz());
      solid.setAttr(_U(startphi), 0e0);
      solid.setAttr(_U(deltaphi), 360.0);
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(tube)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rmin), sh->GetRmin());
      solid.setAttr(_U(rmax), sh->GetRmax());
      solid.setAttr(_U(z), 2 * sh->GetDz());   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(startphi), sh->GetPhi1());
      solid.setAttr(_U(deltaphi), sh->GetPhi2());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoEltu::Class()) {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(eltube)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(dx), sh->GetA());
      solid.setAttr(_U(dy), sh->GetB());
      solid.setAttr(_U(dz), sh->GetDz());
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTrd1::Class()) {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(trd)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(x1), 2 * sh->GetDx1());
      solid.setAttr(_U(x2), 2 * sh->GetDx2());
      solid.setAttr(_U(y1), 2 * sh->GetDy());
      solid.setAttr(_U(y2), 2 * sh->GetDy());
      solid.setAttr(_U(z),  2 * sh->GetDz());   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTrd2::Class()) {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(trd)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(x1), 2 * sh->GetDx1());
      solid.setAttr(_U(x2), 2 * sh->GetDx2());
      solid.setAttr(_U(y1), 2 * sh->GetDy1());
      solid.setAttr(_U(y2), 2 * sh->GetDy2());
      solid.setAttr(_U(z),  2 * sh->GetDz());   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoHype::Class()) {
      const TGeoHype* sh = (const TGeoHype*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(hype)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rmin),  sh->GetRmin());
      solid.setAttr(_U(rmax),  sh->GetRmax());
      solid.setAttr(Unicode("inst"),  sh->GetStIn());
      solid.setAttr(_U(outst), sh->GetStOut());
      solid.setAttr(_U(z),     sh->GetDz());   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoPgon::Class()) {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(polyhedra)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(startphi), sh->GetPhi1());
      solid.setAttr(_U(deltaphi), sh->GetDphi());
      solid.setAttr(_U(numsides), sh->GetNedges());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm"); 
      for (Int_t i = 0; i < sh->GetNz(); ++i) {
        zplane = xml_elt_t(geo.doc, _U(zplane));
        zplane.setAttr(_U(z), sh->GetZ(i));
        zplane.setAttr(_U(rmin), sh->GetRmin(i));
        zplane.setAttr(_U(rmax), sh->GetRmax(i));
        solid.append(zplane);
      }
    }
    else if (isa == TGeoPcon::Class()) {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(polycone)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(startphi), sh->GetPhi1());
      solid.setAttr(_U(deltaphi), sh->GetDphi());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm"); 
      for (Int_t i = 0; i < sh->GetNz(); ++i) {
        zplane = xml_elt_t(geo.doc, _U(zplane));
        zplane.setAttr(_U(z), sh->GetZ(i));
        zplane.setAttr(_U(rmin), sh->GetRmin(i));
        zplane.setAttr(_U(rmax), sh->GetRmax(i));
        solid.append(zplane);
      }
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoCone::Class()) {
      const TGeoCone* sh = (const TGeoCone*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(cone)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(z),     2 * sh->GetDz());
      solid.setAttr(_U(rmin1), sh->GetRmin1());
      solid.setAttr(_U(rmax1), sh->GetRmax1());
      solid.setAttr(_U(rmin2), sh->GetRmin2());
      solid.setAttr(_U(rmax2), sh->GetRmax2());
      solid.setAttr(_U(startphi), 0e0);
      solid.setAttr(_U(deltaphi), 360.0);
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoConeSeg::Class()) {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(cone)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(z), 2*sh->GetDz());
      solid.setAttr(_U(rmin1), sh->GetRmin1());
      solid.setAttr(_U(rmin2), sh->GetRmin2());
      solid.setAttr(_U(rmax1), sh->GetRmax1());
      solid.setAttr(_U(rmax2), sh->GetRmax2());
      solid.setAttr(_U(startphi), sh->GetPhi1());
      solid.setAttr(_U(deltaphi), sh->GetPhi2() - sh->GetPhi1());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoParaboloid::Class()) {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(paraboloid)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rlo), sh->GetRlo());
      solid.setAttr(_U(rhi), sh->GetRhi());
      solid.setAttr(_U(dz),  sh->GetDz());
      solid.setAttr(_U(lunit), "cm");
    }
#if 0
    else if (isa == TGeoEllipsoid::Class()) {
      const TGeoEllipsoid* sh = (const TGeoEllipsoid*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(ellipsoid)));
      solid.setAttr(_U(lunit), "cm");
    }
#endif
    else if (isa == TGeoSphere::Class()) {
      const TGeoSphere* sh = (const TGeoSphere*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(sphere)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rmin),        sh->GetRmin());
      solid.setAttr(_U(rmax),        sh->GetRmax());
      solid.setAttr(_U(startphi),    sh->GetPhi1());
      solid.setAttr(_U(deltaphi),   (sh->GetPhi2() - sh->GetPhi1()));
      solid.setAttr(_U(starttheta),  sh->GetTheta1());
      solid.setAttr(_U(deltatheta), (sh->GetTheta2() - sh->GetTheta1()));
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTorus::Class()) {
      const TGeoTorus* sh = (const TGeoTorus*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(torus)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(rtor),     sh->GetR());
      solid.setAttr(_U(rmin),     sh->GetRmin());
      solid.setAttr(_U(rmax),     sh->GetRmax());
      solid.setAttr(_U(startphi), sh->GetPhi1());
      solid.setAttr(_U(deltaphi), sh->GetDphi());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoTrap::Class()) {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(trap)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(z),  2 * sh->GetDz());   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(x1), 2 * sh->GetBl1());
      solid.setAttr(_U(x2), 2 * sh->GetTl1());
      solid.setAttr(_U(x3), 2 * sh->GetBl2());
      solid.setAttr(_U(x4), 2 * sh->GetTl2());
      solid.setAttr(_U(y1), 2 * sh->GetH1());
      solid.setAttr(_U(y2), 2 * sh->GetH2());
      solid.setAttr(_U(alpha1), sh->GetAlpha1());
      solid.setAttr(_U(alpha2), sh->GetAlpha2());
      solid.setAttr(_U(theta),  sh->GetTheta());
      solid.setAttr(_U(phi),    sh->GetPhi());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoPara::Class()) {
      const TGeoPara* sh = (const TGeoPara*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(para)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(x), sh->GetX());
      solid.setAttr(_U(y), sh->GetY());
      solid.setAttr(_U(z), sh->GetZ());
      solid.setAttr(_U(alpha), sh->GetAlpha());
      solid.setAttr(_U(theta), sh->GetTheta());
      solid.setAttr(_U(phi),   sh->GetPhi());
      solid.setAttr(_U(aunit), "deg");
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoArb8::Class()) {
      TGeoArb8* sh = (TGeoArb8*) shape;
      const double* vtx = sh->GetVertices();
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(arb8)));
      solid.setAttr(_U(name), Unicode(shape_name));
      solid.setAttr(_U(v1x), vtx[0]);
      solid.setAttr(_U(v1y), vtx[1]);
      solid.setAttr(_U(v2x), vtx[2]);
      solid.setAttr(_U(v2y), vtx[3]);
      solid.setAttr(_U(v3x), vtx[4]);
      solid.setAttr(_U(v3y), vtx[5]);
      solid.setAttr(_U(v4x), vtx[6]);
      solid.setAttr(_U(v4y), vtx[7]);
      solid.setAttr(_U(v5x), vtx[8]);
      solid.setAttr(_U(v5y), vtx[9]);
      solid.setAttr(_U(v6x), vtx[10]);
      solid.setAttr(_U(v6y), vtx[11]);
      solid.setAttr(_U(v7x), vtx[12]);
      solid.setAttr(_U(v7y), vtx[13]);
      solid.setAttr(_U(v8x), vtx[14]);
      solid.setAttr(_U(v8y), vtx[15]);
      solid.setAttr(_U(dz),  sh->GetDz());
      solid.setAttr(_U(lunit), "cm");
    }
    else if (isa == TGeoCompositeShape::Class() ||
             isa == TGeoUnion::Class() ||
             isa == TGeoIntersection::Class() ||
             isa == TGeoSubtraction::Class() )  {
      const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = sh->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      TGeoMatrix* rm = boolean->GetRightMatrix();
      TGeoMatrix* lm = boolean->GetLeftMatrix();
      TGeoShape* ls = boolean->GetLeftShape();
      TGeoShape* rs = boolean->GetRightShape();
      xml_h left    = handleSolid(ls->GetName(), ls);
      xml_h right   = handleSolid(rs->GetName(), rs);
      xml_h first(0), second(0);
      if (!left) {
        throw runtime_error("G4Converter: No left Detector Solid present for composite shape:" + name);
      }
      if (!right) {
        throw runtime_error("G4Converter: No right Detector Solid present for composite shape:" + name);
      }

      //specific case!
      //Ellipsoid tag preparing
      //if left == TGeoScaledShape AND right  == TGeoBBox
      //   AND if TGeoScaledShape->GetShape == TGeoSphere
      if (strcmp(ls->ClassName(), "TGeoScaledShape") == 0 &&
          strcmp(rs->ClassName(), "TGeoBBox") == 0) {
        if (strcmp(((TGeoScaledShape *)ls)->GetShape()->ClassName(), "TGeoSphere") == 0) {
          if (oper == TGeoBoolNode::kGeoIntersection) {
            TGeoScaledShape* lls = (TGeoScaledShape *)ls;
            TGeoBBox* rrs = (TGeoBBox*)rs;
            solid = xml_elt_t(geo.doc,Unicode("ellipsoid"));
            solid.setAttr(_U(name), Unicode(shape_name));
            double sx = lls->GetScale()->GetScale()[0];
            double sy = lls->GetScale()->GetScale()[1];
            double radius = ((TGeoSphere *)lls->GetShape())->GetRmax();
            double dz = rrs->GetDZ();
            double zorig = rrs->GetOrigin()[2];
            double zcut2 = dz + zorig;
            double zcut1 = 2 * zorig - zcut2;
            solid.setAttr(Unicode("ax"),sx * radius);
            solid.setAttr(Unicode("by"),sy * radius);
            solid.setAttr(Unicode("cz"),radius);
            solid.setAttr(Unicode("zcut1"),zcut1);
            solid.setAttr(Unicode("zcut2"),zcut2);
            solid.setAttr(_U(lunit), "cm");
            return data().xmlSolids[shape] = solid;
          }
        }
      }

      if ( oper == TGeoBoolNode::kGeoSubtraction )
        solid = xml_elt_t(geo.doc,_U(subtraction));
      else if ( oper == TGeoBoolNode::kGeoUnion )
        solid = xml_elt_t(geo.doc,_U(union));
      else if ( oper == TGeoBoolNode::kGeoIntersection )
        solid = xml_elt_t(geo.doc,_U(intersection));

      xml_h obj;
      string lnam = left.attr<string>(_U(name));
      string rnam = right.attr<string>(_U(name));

      geo.doc_solids.append(solid);
      solid.append(first = xml_elt_t(geo.doc, _U(first)));
      solid.setAttr(_U(name), Unicode(shape_name));
      first.setAttr(_U(ref),  lnam);
      const double *tr = lm->GetTranslation();

      if ((tr[0] != 0.0) || (tr[1] != 0.0) || (tr[2] != 0.0)) {
        first.append(obj = xml_elt_t(geo.doc, _U(firstposition)));
        obj.setAttr(_U(name), name+"_"+lnam+"_pos");
        obj.setAttr(_U(x), tr[0]);
        obj.setAttr(_U(y), tr[1]);
        obj.setAttr(_U(z), tr[2]);
        obj.setAttr(_U(unit), "cm");
      }
      if (lm->IsRotation()) {
        TGeoMatrix& linv = lm->Inverse();
        XYZRotation rot = getXYZangles(linv.GetRotationMatrix());
        if ((rot.X() != 0.0) || (rot.Y() != 0.0) || (rot.Z() != 0.0)) {
          first.append(obj = xml_elt_t(geo.doc, _U(firstrotation)));
          obj.setAttr(_U(name), name+"_"+lnam+"_rot");
          obj.setAttr(_U(x), rot.X());
          obj.setAttr(_U(y), rot.Y());
          obj.setAttr(_U(z), rot.Z());
          obj.setAttr(_U(unit), "rad");
        }
      }
      tr = rm->GetTranslation();
      solid.append(second = xml_elt_t(geo.doc, _U(second)));
      second.setAttr(_U(ref), rnam);
      if ((tr[0] != 0.0) || (tr[1] != 0.0) || (tr[2] != 0.0)) {
        xml_ref_t pos = handlePosition(rnam+"_pos", rm);
        solid.setRef(_U(positionref), pos.name());
      }
      if (rm->IsRotation()) {
        TGeoMatrix& rinv = rm->Inverse();
        XYZRotation rot = getXYZangles(rinv.GetRotationMatrix());
        if ((rot.X() != 0.0) || (rot.Y() != 0.0) || (rot.Z() != 0.0)) {
          xml_ref_t xml_rot = handleRotation(rnam+"_rot", &rinv);
          solid.setRef(_U(rotationref), xml_rot.name());
        }
      }
    }
    if (!solid) {
      string err = "Failed to handle unknown solid shape:" + name + " of type " + string(shape->IsA()->GetName());
      throw runtime_error(err);
    }
    return data().xmlSolids[shape] = solid;
  }
}

/// Convert the Position into the corresponding Xml object(s).
xml_h LCDDConverter::handlePosition(const std::string& name, const TGeoMatrix* trafo) const {
  GeometryInfo& geo = data();
  xml_h pos = geo.xmlPositions[trafo];
  if (!pos) {
    const double* tr = trafo->GetTranslation();
    if (tr[0] != 0.0 || tr[1] != 0.0 || tr[2] != 0.0) {
      string gen_name = genName(name,trafo);
      geo.checkPosition(gen_name, trafo);
      geo.doc_define.append(pos = xml_elt_t(geo.doc, _U(position)));
      pos.setAttr(_U(name), gen_name);
      pos.setAttr(_U(x), tr[0]);
      pos.setAttr(_U(y), tr[1]);
      pos.setAttr(_U(z), tr[2]);
      pos.setAttr(_U(unit), "cm");
    }
    else if (geo.identity_pos) {
      pos = geo.identity_pos;
    }
    else {
      geo.doc_define.append(geo.identity_pos = xml_elt_t(geo.doc, _U(position)));
      geo.identity_pos.setAttr(_U(name), "identity_pos");
      geo.identity_pos.setAttr(_U(x), 0);
      geo.identity_pos.setAttr(_U(y), 0);
      geo.identity_pos.setAttr(_U(z), 0);
      geo.identity_pos.setAttr(_U(unit), "cm");
      pos = geo.identity_pos;
      geo.checkPosition("identity_pos", 0);
    }
    geo.xmlPositions[trafo] = pos;
  }
  return pos;
}

/// Convert the Rotation into the corresponding Xml object(s).
xml_h LCDDConverter::handleRotation(const std::string& name, const TGeoMatrix* trafo) const {
  GeometryInfo& geo = data();
  xml_h rot = geo.xmlRotations[trafo];
  if (!rot) {
    XYZRotation r = getXYZangles(trafo->GetRotationMatrix());
    if (!(r.X() == 0.0 && r.Y() == 0.0 && r.Z() == 0.0)) {
      string gen_name = genName(name,trafo);
      geo.checkRotation(gen_name, trafo);
      geo.doc_define.append(rot = xml_elt_t(geo.doc, _U(rotation)));
      rot.setAttr(_U(name), gen_name);
      rot.setAttr(_U(x), r.X());
      rot.setAttr(_U(y), r.Y());
      rot.setAttr(_U(z), r.Z());
      rot.setAttr(_U(unit), "rad");
    }
    else if (geo.identity_rot) {
      rot = geo.identity_rot;
    }
    else {
      geo.doc_define.append(geo.identity_rot = xml_elt_t(geo.doc, _U(rotation)));
      geo.identity_rot.setAttr(_U(name), "identity_rot");
      geo.identity_rot.setAttr(_U(x), 0);
      geo.identity_rot.setAttr(_U(y), 0);
      geo.identity_rot.setAttr(_U(z), 0);
      geo.identity_rot.setAttr(_U(unit), "rad");
      rot = geo.identity_rot;
      geo.checkRotation("identity_rot", 0);
    }
    geo.xmlRotations[trafo] = rot;
  }
  return rot;
}

/// Dump logical volume in GDML format to output stream
xml_h LCDDConverter::handleVolume(const string& /* name */, Volume volume) const {
  GeometryInfo& geo = data();
  xml_h vol = geo.xmlVolumes[volume];
  if (!vol) {
    const TGeoVolume* v = volume;
    Volume _v = Ref_t(v);
    string  n = genName(v->GetName(),v);
    TGeoMedium* medium = v->GetMedium();
    TGeoShape*  sh     = v->GetShape();
    xml_ref_t   sol    = handleSolid(sh->GetName(), sh);

    geo.checkVolume(n, volume);
    if (v->IsAssembly()) {
      vol = xml_elt_t(geo.doc, _U(assembly));
      vol.setAttr(_U(name), n);
    }
    else {
      if (!sol)
        throw runtime_error("G4Converter: No Detector Solid present for volume:" + n);
      else if (!m)
        throw runtime_error("G4Converter: No Detector material present for volume:" + n);

      vol = xml_elt_t(geo.doc, _U(volume));
      vol.setAttr(_U(name), n);
      if (m) {
        string    mat_name = medium->GetName();
        xml_ref_t med      = handleMaterial(mat_name, Material(medium));
        vol.setRef(_U(materialref), med.name());
      }
      vol.setRef(_U(solidref), sol.name());
    }
    geo.doc_structure.append(vol);
    geo.xmlVolumes[v] = vol;
    const TObjArray* dau = const_cast<TGeoVolume*>(v)->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
        TGeoNode* node = reinterpret_cast<TGeoNode*>(dau->At(i));
        handlePlacement(node->GetName(), node);
      }
    }
    if (geo.doc_header && is_volume(volume)) {
      Region   reg = _v.region();
      LimitSet lim = _v.limitSet();
      VisAttr  vis = _v.visAttributes();
      SensitiveDetector det = _v.sensitiveDetector();
      if (det.isValid()) {
        xml_ref_t xml_data = handleSensitive(det.name(), det);
        vol.setRef(_U(sdref), xml_data.name());
      }
      if (reg.isValid()) {
        xml_ref_t xml_data = handleRegion(reg.name(), reg);
        vol.setRef(_U(regionref), xml_data.name());
      }
      if (lim.isValid()) {
        xml_ref_t xml_data = handleLimitSet(lim.name(), lim);
        vol.setRef(_U(limitsetref), xml_data.name());
      }
      if (vis.isValid()) {
        xml_ref_t xml_data = handleVis(vis.name(), vis);
        vol.setRef(_U(visref), xml_data.name());
      }
    }
  }
  return vol;
}

/// Dump logical volume in GDML format to output stream
xml_h LCDDConverter::handleVolumeVis(const string& /* name */, const TGeoVolume* volume) const {
  GeometryInfo& geo = data();
  xml_h vol = geo.xmlVolumes[volume];
  if (!vol) {
    const TGeoVolume* v = volume;
    Volume _v = Ref_t(v);
    if (is_volume(volume)) {
      VisAttr vis = _v.visAttributes();
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

/// Dump logical volume in GDML format to output stream
void LCDDConverter::collectVolume(const string& /* name */, const TGeoVolume* volume) const {
  Volume v = Ref_t(volume);
  if ( is_volume(volume) )     {
    GeometryInfo& geo = data();
    Region reg = v.region();
    LimitSet lim = v.limitSet();
    SensitiveDetector det = v.sensitiveDetector();
    if (lim.isValid())
      geo.limits.insert(lim);
    if (reg.isValid())
      geo.regions.insert(reg);
    if (det.isValid())
      geo.sensitives.insert(det);
  }
  else {
    printout(WARNING,"LCDDConverter","++ CollectVolume: Skip volume: %s",volume->GetName());
  }
}

void LCDDConverter::checkVolumes(const string& /* name */, Volume v) const {
  string n = v.name()+_toString(v.ptr(),"_%p");
  NameSet::const_iterator i = m_checkNames.find(n);
  if (i != m_checkNames.end()) {
    stringstream str;
    str << "++ CheckVolumes: Volume " << n << " ";
    if (is_volume(v.ptr()))     {
      SensitiveDetector sd = v.sensitiveDetector();
      VisAttr vis = v.visAttributes();
      if (sd.isValid()) {
        str << "of " << sd.name() << " ";
      }
      else if (vis.isValid()) {
        str << "with VisAttrs " << vis.name() << " ";
      }
    }
    str << "has duplicate entries." << endl;
    printout(ERROR,"LCDDConverter",str.str().c_str());
    return;
  }
  m_checkNames.insert(n);
}

/// Dump volume placement in GDML format to output stream
xml_h LCDDConverter::handlePlacement(const string& name,PlacedVolume node) const {
  GeometryInfo& geo = data();
  xml_h place = geo.xmlPlacements[node];
  if (!place) {
    TGeoMatrix* m = node->GetMatrix();
    TGeoVolume* v = node->GetVolume();
    xml_ref_t vol = xml_h(geo.xmlVolumes[v]);
    xml_h mot = geo.xmlVolumes[node->GetMotherVolume()];

    place = xml_elt_t(geo.doc, _U(physvol));
    if (mot) {   // Beware of top level volume!
      mot.append(place);
    }
    place.setRef(_U(volumeref), vol.name());
    if (m) {
      xml_ref_t pos = handlePosition(name+"_pos", m);
      place.setRef(_U(positionref), pos.name());
      if ( m->IsRotation() )  {
        xml_ref_t rot = handleRotation(name+"_rot", m);
        place.setRef(_U(rotationref), rot.name());
      }
    }
    if (geo.doc_root.tag() != "gdml") {
      if (is_placement(node)) {
        const PlacedVolume::VolIDs& ids = node.volIDs();
        for (PlacedVolume::VolIDs::const_iterator i = ids.begin(); i != ids.end(); ++i) {
          xml_h pvid = xml_elt_t(geo.doc, _U(physvolid));
          pvid.setAttr(_U(field_name), (*i).first);
          pvid.setAttr(_U(value), (*i).second);
          place.append(pvid);
        }
      }
    }
    geo.xmlPlacements[node] = place;
  }
  else {
    cout << "Attempt to DOUBLE-place physical volume:" << name << " No:" << node->GetNumber() << endl;
  }
  return place;
}

/// Convert the geometry type region into the corresponding Detector object(s).
xml_h LCDDConverter::handleRegion(const std::string& /* name */, Region region) const {
  GeometryInfo& geo = data();
  xml_h reg = geo.xmlRegions[region];
  if (!reg) {
    geo.doc_regions.append(reg = xml_elt_t(geo.doc, _U(region)));
    reg.setAttr(_U(name), region.name());
    reg.setAttr(_U(cut), region.cut());
    reg.setAttr(_U(eunit), "GeV");  // TGeo has energy in GeV
    reg.setAttr(_U(lunit), "cm");   // TGeo has lengths in cm
    reg.setAttr(_U(store_secondaries), region.storeSecondaries());
    geo.xmlRegions[region] = reg;
  }
  return reg;
}

/// Convert the geometry type LimitSet into the corresponding Detector object(s)
xml_h LCDDConverter::handleLimitSet(const std::string& /* name */, LimitSet lim) const {
  GeometryInfo& geo = data();
  xml_h xml = geo.xmlLimits[lim];
  if (!xml) {
    geo.doc_limits.append(xml = xml_elt_t(geo.doc, _U(limitset)));
    xml.setAttr(_U(name), lim.name());
    const set<Limit>& obj = lim.limits();
    for (set<Limit>::const_iterator i = obj.begin(); i != obj.end(); ++i) {
      xml_h x = xml_elt_t(geo.doc, _U(limit));
      const Limit& l = *i;
      xml.append(x);
      x.setAttr(_U(name), l.name);
      x.setAttr(_U(unit), l.unit);
      x.setAttr(_U(value), l.value);
      x.setAttr(_U(particles), l.particles);
    }
    geo.xmlLimits[lim] = xml;
  }
  return xml;
}

/// Convert the segmentation of a SensitiveDetector into the corresponding Detector object
xml_h LCDDConverter::handleSegmentation(Segmentation seg) const {
  xml_h xml;
  if (seg.isValid()) {
    typedef DDSegmentation::Parameters _P;
    string typ = seg.type();
    _P p = seg.parameters();
    xml = xml_elt_t(data().doc, Unicode(typ));
    for (_P::const_iterator i = p.begin(); i != p.end(); ++i) {
      const _P::value_type& v = *i;
      if (v->name() == "lunit") {
        string val = v->value() == _toDouble("mm") ? "mm" : v->value() == _toDouble("cm") ? "cm" :
          v->value() == _toDouble("m") ? "m" : v->value() == _toDouble("micron") ? "micron" :
          v->value() == _toDouble("nanometer") ? "namometer" : "??";
        xml.setAttr(Unicode(v->name()), Unicode(val));
        continue;
      }
      // translate from TGeo units to Geant4 units if necessary
      if (v->unitType() == DDSegmentation::SegmentationParameter::LengthUnit) {
        double value = _toDouble(v->value()) * CM_2_MM;
        xml.setAttr(Unicode(v->name()), value);
      } else if (v->unitType() == DDSegmentation::SegmentationParameter::AngleUnit) {
        double value = _toDouble(v->value()) * DEGREE_2_RAD;
        xml.setAttr(Unicode(v->name()), value);
      } else {
        xml.setAttr(Unicode(v->name()), v->value());
      }
    }
  }
  return xml;
}

/// Convert the geometry type SensitiveDetector into the corresponding Detector object(s).
xml_h LCDDConverter::handleSensitive(const string& /* name */, SensitiveDetector sd) const {
  GeometryInfo& geo = data();
  xml_h sensdet = geo.xmlSensDets[sd];
  if (!sensdet) {
    geo.doc_detectors.append(sensdet = xml_elt_t(geo.doc, Unicode(sd.type())));
    sensdet.setAttr(_U(name), sd.name());
    sensdet.setAttr(_U(ecut), sd.energyCutoff());
    sensdet.setAttr(_U(eunit), "MeV");
    sensdet.setAttr(_U(verbose), int(sd.verbose() ? 1 : 0));
    sensdet.setAttr(_U(hits_collection), sd.hitsCollection());
    if (sd.combineHits())
      sensdet.setAttr(_U(combine_hits), sd.combineHits());
    Readout ro = sd.readout();
    if (ro.isValid()) {
      xml_ref_t ref = handleIdSpec(ro.idSpec().name(), ro.idSpec());
      sensdet.setRef(_U(idspecref), ref.name());
      xml_h seg = handleSegmentation(ro.segmentation());
      if (seg)
        sensdet.append(seg);
    }
    geo.xmlSensDets[sd] = sensdet;
  }
  return sensdet;
}

/// Convert the geometry id dictionary entry to the corresponding Xml object(s).
xml_h LCDDConverter::handleIdSpec(const std::string& name, IDDescriptor id_spec) const {
  GeometryInfo& geo = data();
  xml_h id = geo.xmlIdSpecs[id_spec];
  if (!id) {
    int length = 0, start = 0;
    IDDescriptor desc = Ref_t(id_spec);
    geo.doc_idDict.append(id = xml_elt_t(geo.doc, _U(idspec)));
    id.setAttr(_U(name), name);
    const IDDescriptor::FieldMap& fm = desc.fields();
    for (const auto& i : fm )  {
      xml_h idfield = xml_elt_t(geo.doc, _U(idfield));
#if 0
      const BitFieldValue* f = i.second;
      start = f.first;
      length = f.second<0 ? -f.second : f.second;
      idfield.setAttr(_U(signed),f.second<0 ? true : false);
      idfield.setAttr(_U(label),(*i).first);
      idfield.setAttr(_U(length),length);
      idfield.setAttr(_U(start),start);
#else
      const BitFieldValue* f = i.second;
      idfield.setAttr(_U(signed),f->isSigned() ? true : false);
      idfield.setAttr(_U(label), f->name());
      idfield.setAttr(_U(length), (int) f->width());
      idfield.setAttr(_U(start), (int) f->offset());
#endif
      id.append(idfield);
    }
    id.setAttr(_U(length), length + start);
    geo.xmlIdSpecs[id_spec] = id;
  }
  return id;
}

/// Convert the geometry visualisation attributes to the corresponding Detector object(s).
xml_h LCDDConverter::handleVis(const string& /* name */, VisAttr attr) const {
  GeometryInfo& geo = data();
  xml_h vis = geo.xmlVis[attr];
  if (!vis) {
    float r = 0, g = 0, b = 0;
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
    attr.rgb(r, g, b);
    col.setAttr(_U(alpha), attr.alpha());
    col.setAttr(_U(R), r);
    col.setAttr(_U(G), g);
    col.setAttr(_U(B), b);
    vis.append(col);
    geo.xmlVis[attr] = vis;
  }
  return vis;
}

/// Convert the electric or magnetic fields into the corresponding Xml object(s).
xml_h LCDDConverter::handleField(const std::string& /* name */, OverlayedField f) const {
  GeometryInfo& geo = data();
  xml_h field = geo.xmlFields[f];
  if (!field) {
    Ref_t fld(f);
    string type = f->GetTitle();
    field = xml_elt_t(geo.doc, Unicode(type));
    field.setAttr(_U(name), f->GetName());
    fld = PluginService::Create<NamedObject*>(type + "_Convert2Detector", &m_detDesc, &field, &fld);
    printout(ALWAYS,"LCDDConverter","++ %s electromagnetic field:%s of type %s",
             (fld.isValid() ? "Converted" : "FAILED    to convert "), f->GetName(), type.c_str());
    if (!fld.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type + "_Convert2Detector", &m_detDesc, &field, &fld);
      throw runtime_error("Failed to locate plugin to convert electromagnetic field:"
                          + string(f->GetName()) + " of type " + type + ". "
                          + dbg.missingFactory(type));
    }
    geo.doc_fields.append(field);
  }
  return field;
}

/// Handle the geant 4 specific properties
void LCDDConverter::handleProperties(Detector::Properties& prp) const {
  map<string, string> processors;
  static int s_idd = 9999999;
  string id;
  for (Detector::Properties::const_iterator i = prp.begin(); i != prp.end(); ++i) {
    const string& nam = (*i).first;
    const Detector::PropertyValues& vals = (*i).second;
    if (nam.substr(0, 6) == "geant4") {
      Detector::PropertyValues::const_iterator id_it = vals.find("id");
      if (id_it != vals.end()) {
        id = (*id_it).second;
      }
      else {
        char text[32];
        ::snprintf(text, sizeof(text), "%d", ++s_idd);
        id = text;
      }
      processors.insert(make_pair(id, nam));
    }
  }
  for (map<string, string>::const_iterator i = processors.begin(); i != processors.end(); ++i) {
    const GeoHandler* ptr = this;
    string nam = (*i).second;
    const Detector::PropertyValues& vals = prp[nam];
    string type = vals.find("type")->second;
    string tag = type + "_Geant4_action";
    long result = PluginService::Create<long>(tag, &m_detDesc, ptr, &vals);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(tag, &m_detDesc, ptr, &vals);
      if (0 == result) {
        throw runtime_error("Failed to locate plugin to interprete files of type"
                            " \"" + tag + "\" - no factory:" + type + ". " +
                            dbg.missingFactory(tag));
      }
    }
    result = *(long*) result;
    if (result != 1) {
      throw runtime_error("Failed to invoke the plugin " + tag + " of type " + type);
    }
    printout(INFO,"","+++ Executed Successfully Detector setup module %s.",type.c_str());
  }
}

/// Add header information in Detector format
void LCDDConverter::handleHeader() const {
  GeometryInfo& geo = data();
  Header hdr = m_detDesc.header();
  if ( hdr.isValid() )  {
    xml_h obj;
    geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(detector)));
    obj.setAttr(_U(name), hdr.name());
    geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(generator)));
    obj.setAttr(_U(name), "LCDDConverter");
    obj.setAttr(_U(version), hdr.version());
    obj.setAttr(_U(file), hdr.url());
    obj.setAttr(_U(checksum), Unicode(m_detDesc.constantAsString("compact_checksum")));
    geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(author)));
    obj.setAttr(_U(name), hdr.author());
    geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(comment)));
    obj.setText(hdr.comment());
    return;
  }
  printout(WARNING,"LCDDConverter","+++ No Detector header information availible from the geometry description.");
}

template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
    string n = (*i)->GetName();
    (o->*pmf)(n, *i);
  }
}

template <typename O, typename C, typename F> void handleMap(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i)
    (o->*pmf)((*i).first, (*i).second);
}

template <typename O, typename C, typename F> void handleRMap(const O* o, const C& c, F pmf) {
  for (typename C::const_reverse_iterator i = c.rbegin(); i != c.rend(); ++i)
    handle(o, (*i).second, pmf);
}

/// Create geometry conversion
xml_doc_t LCDDConverter::createGDML(DetElement top) {
  Detector& description = m_detDesc;
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createGDML with an invalid geometry!");
  }
  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);

  printout(ALWAYS,"LCDDConverter","++ ==> Converting in memory detector description to GDML format...");
  xml::DocumentHandler docH;
  geo.doc = docH.create("gdml", docH.defaultComment());
  geo.doc_root = geo.doc.root();
  geo.doc_root.setAttr(Unicode("xmlns:xs"), "http://www.w3.org/2001/XMLSchema-instance");
  geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"),
                       "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd");
  // geo.doc = docH.create("gdml_simple_extension",comment);
  // geo.doc_root.setAttr(Unicode("xmlns:gdml_simple_extension"),"http://www.example.org");
  // geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"),
  //     "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd");
  Volume world_vol = description.worldVolume();
  geo.doc_root.append(geo.doc_define = xml_elt_t(geo.doc, _U(define)));
  geo.doc_root.append(geo.doc_materials = xml_elt_t(geo.doc, _U(materials)));
  geo.doc_root.append(geo.doc_solids = xml_elt_t(geo.doc, _U(solids)));
  geo.doc_root.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));
  geo.doc_root.append(geo.doc_setup = xml_elt_t(geo.doc, _U(setup)));
  geo.doc_setup.setRef(_U(world), genName(world_vol.name(),world_vol.ptr()));
  geo.doc_setup.setAttr(_U(name), Unicode("default"));
  geo.doc_setup.setAttr(_U(version), Unicode("1.0"));

  // Ensure that all required materials are present in the Detector material table
#if 0
  const Detector::HandleMap& mat = description.materials();
  for(Detector::HandleMap::const_iterator i=mat.begin(); i!=mat.end(); ++i)
    geo.materials.insert(dynamic_cast<TGeoMedium*>((*i).second.ptr()));
#endif

  // Start creating the objects for materials, solids and log volumes.
  handle(this, geo.materials, &LCDDConverter::handleMaterial);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld materials.",geo.materials.size());

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  printout(ALWAYS,"LCDDConverter","++ Collected %ld volumes.",geo.volumes.size());

  handle(this, geo.solids, &LCDDConverter::handleSolid);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld solids.",geo.solids.size());

  handle(this, geo.volumes, &LCDDConverter::handleVolume);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld volumes.",geo.volumes.size());

  m_checkNames.clear();
  handle(this, geo.volumes, &LCDDConverter::checkVolumes);
  return geo.doc;
}

/// Create geometry conversion
xml_doc_t LCDDConverter::createVis(DetElement top) {
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createDetector with an invalid geometry!");
  }

  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);
  printout(ALWAYS,"LCDDConverter","++ ==> Dump visualisation attributes "
           "from in memory detector description...");
  xml::DocumentHandler docH;
  xml_elt_t elt(0);
  geo.doc = docH.create("visualization", docH.defaultComment());
  geo.doc_root = geo.doc.root();
  geo.doc_root.append(geo.doc_display = xml_elt_t(geo.doc, _U(display)));
  geo.doc_root.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  handle(this, geo.volumes, &LCDDConverter::handleVolumeVis);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld volumes.",geo.volumes.size());
  return geo.doc;
}

/// Create geometry conversion
xml_doc_t LCDDConverter::createDetector(DetElement top) {
  Detector& description = m_detDesc;
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createDetector with an invalid geometry!");
  }

  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);
  xml::DocumentHandler docH;
  xml_elt_t elt(0);
  Volume world_vol = description.worldVolume();
  geo.doc = docH.create("description", docH.defaultComment());
  geo.doc_root = geo.doc.root();
  geo.doc_root.setAttr(Unicode("xmlns:description"), "http://www.lcsim.org/schemas/description/1.0");
  geo.doc_root.setAttr(Unicode("xmlns:xs"), "http://www.w3.org/2001/XMLSchema-instance");
  geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"), 
                       "http://www.lcsim.org/schemas/description/1.0/description.xsd");

  geo.doc_root.append(geo.doc_header = xml_elt_t(geo.doc, _U(header)));
  geo.doc_root.append(geo.doc_idDict = xml_elt_t(geo.doc, _U(iddict)));
  geo.doc_root.append(geo.doc_detectors = xml_elt_t(geo.doc, _U(sensitive_detectors)));
  geo.doc_root.append(geo.doc_limits = xml_elt_t(geo.doc, _U(limits)));
  geo.doc_root.append(geo.doc_regions = xml_elt_t(geo.doc, _U(regions)));
  geo.doc_root.append(geo.doc_display = xml_elt_t(geo.doc, _U(display)));
  geo.doc_root.append(geo.doc_gdml = xml_elt_t(geo.doc, _U(gdml)));
  geo.doc_root.append(geo.doc_fields = xml_elt_t(geo.doc, _U(fields)));

  geo.doc_gdml.append(geo.doc_define = xml_elt_t(geo.doc, _U(define)));
  geo.doc_gdml.append(geo.doc_materials = xml_elt_t(geo.doc, _U(materials)));
  geo.doc_gdml.append(geo.doc_solids = xml_elt_t(geo.doc, _U(solids)));
  geo.doc_gdml.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));
  geo.doc_gdml.append(geo.doc_setup = xml_elt_t(geo.doc, _U(setup)));
  geo.doc_setup.setRef(_U(world), genName(world_vol.name(),world_vol.ptr()));
  geo.doc_setup.setAttr(_U(name), Unicode("default"));
  geo.doc_setup.setAttr(_U(version), Unicode("1.0"));

  // Ensure that all required materials are present in the Detector material table
  const Detector::HandleMap& fld = description.fields();
  for (Detector::HandleMap::const_iterator i = fld.begin(); i != fld.end(); ++i)
    geo.fields.insert((*i).second);

  printout(ALWAYS,"LCDDConverter","++ ==> Converting in memory detector description to Detector format...");
  handleHeader();
  // Start creating the objects for materials, solids and log volumes.
  handle(this, geo.materials, &LCDDConverter::handleMaterial);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld materials.",geo.materials.size());

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  printout(ALWAYS,"LCDDConverter","++ Collected %ld volumes.",geo.volumes.size());

  handle(this, geo.solids, &LCDDConverter::handleSolid);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld solids.",geo.solids.size());

  handle(this, geo.vis, &LCDDConverter::handleVis);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld visualization attributes.",geo.vis.size());

  handle(this, geo.sensitives, &LCDDConverter::handleSensitive);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld sensitive detectors.",geo.sensitives.size());

  handle(this, geo.limits, &LCDDConverter::handleLimitSet);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld limit sets.",geo.limits.size());

  handle(this, geo.regions, &LCDDConverter::handleRegion);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld regions.",geo.regions.size());

  handle(this, geo.volumes, &LCDDConverter::handleVolume);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld volumes.",geo.volumes.size());

  handle(this, geo.fields, &LCDDConverter::handleField);
  printout(ALWAYS,"LCDDConverter","++ Handled %ld fields.",geo.fields.size());

  m_checkNames.clear();
  handle(this, geo.volumes, &LCDDConverter::checkVolumes);
#if 0
  //==================== Fields
  handleProperties(m_detDesc.properties());
#endif
  return geo.doc;
}

/// Helper constructor
LCDDConverter::GeometryInfo::GeometryInfo()
  : doc(0), doc_root(0), doc_header(0), doc_idDict(0), doc_detectors(0), doc_limits(0), 
    doc_regions(0), doc_display(0), doc_gdml(0), doc_fields(0), doc_define(0),
    doc_materials(0), doc_solids(0), doc_structure(0), doc_setup(0)
{
}

static long dump_output(xml_doc_t doc, int argc, char** argv) {
  xml::DocumentHandler docH;
  return docH.output(doc, argc > 0 ? argv[0] : "");
}

static long create_gdml(Detector& description, int argc, char** argv) {
  LCDDConverter wr(description);
  return dump_output(wr.createGDML(description.world()), argc, argv);
}

static long create_description(Detector& description, int argc, char** argv) {
  LCDDConverter wr(description);
  return dump_output(wr.createDetector(description.world()), argc, argv);
}

static long create_vis(Detector& description, int argc, char** argv) {
  LCDDConverter wr(description);
  return dump_output(wr.createVis(description.world()), argc, argv);
}

static long create_visASCII(Detector& description, int /* argc */, char** argv) {
  LCDDConverter wr(description);
  /* xml_doc_t doc = */ wr.createVis(description.world());
  LCDDConverter::GeometryInfo& geo = wr.data();
  map<string, xml_comp_t> vis_map;
  for (xml_coll_t c(geo.doc_display, _U(vis)); c; ++c)
    vis_map.insert(make_pair(xml_comp_t(c).nameStr(), xml_comp_t(c)));

  const char* sep = ";";
  ofstream os(argv[0]);
  for (xml_coll_t c(geo.doc_structure, _U(volume)); c; ++c) {
    xml_comp_t vol = c;
    xml_comp_t ref = c.child(_U(visref));
    xml_comp_t vis = (*vis_map.find(ref.refStr())).second;
    xml_comp_t col = vis.child(_U(color));
    os << "vol:" << vol.nameStr() << sep << "vis:" << vis.nameStr() << sep 
       << "visible:" << vis.visible() << sep << "r:"
       << col.R() << sep << "g:" << col.G() << sep << "b:" << col.B() << sep 
       << "alpha:" << col.alpha() << sep << "line_style:"
       << vis.attr < string > (_U(line_style)) << sep 
       << "drawing_style:" << vis.attr < string> (_U(drawing_style)) << sep 
       << "show_daughters:" << vis.show_daughters() << sep << endl;
  }
  os.close();
  return 1;
}

DECLARE_APPLY(dd4hepGeometry2VIS, create_vis)
DECLARE_APPLY(dd4hepGeometry2VISASCII, create_visASCII)
DECLARE_APPLY(dd4hepGeometry2GDML, create_gdml)
DECLARE_APPLY(dd4hepGeometry2Detector, create_description)
