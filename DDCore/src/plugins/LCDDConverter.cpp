// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework includes
#include "DD4hep/Plugins.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/FieldTypes.h"
#include "DD4hep/Segmentations.h"
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

#include "TGeoNode.h"
#include "TClass.h"
#include "TMath.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;
namespace {
  typedef Position XYZRotation;
#if 0
  XYZRotation getXYZangles(const Double_t* r) {
    Double_t cosb = sqrt(r[0]*r[0] + r[1]*r[1]);
    if (cosb > 0.00001) {
      return XYZRotation(atan2(r[5], r[8]), atan2(-r[2], cosb), atan2(r[1], r[0]));
    }
    return XYZRotation(atan2(-r[7], r[4]),atan2(-r[2], cosb),0);
  }
#endif
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
    return XYZRotation(a, b, c);
  }
}

void LCDDConverter::GeometryInfo::check(const string& name, const TNamed* n, map<string, const TNamed*>& m) const {
  map<string, const TNamed*>::const_iterator i = m.find(name);
  if (i != m.end()) {
    const char* isa = n ? n->IsA()->GetName() : (*i).second ? (*i).second->IsA()->GetName() : "Unknown";
    cout << isa << "(position):  duplicate entry with name:" << name << " " << (void*) n << " " << (void*) (*i).second << endl;
  }
  m.insert(make_pair(name, n));
}

/// Initializing Constructor
LCDDConverter::LCDDConverter(LCDD& lcdd)
    : m_lcdd(lcdd), m_dataPtr(0) {
}

LCDDConverter::~LCDDConverter() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

/// Dump element in GDML format to output stream
xml_h LCDDConverter::handleElement(const string& name, const TGeoElement* element) const {
  GeometryInfo& geo = data();
  xml_h e = geo.xmlElements[element];
  if (!e) {
    xml_elt_t atom(geo.doc, _U(atom));
    geo.doc_materials.append(e = xml_elt_t(geo.doc, _U(element)));
    e.append(atom);
    e.setAttr(_U(name), element->GetName());
    e.setAttr(_U(formula), element->GetName());
    e.setAttr(_U(Z), element->Z());
    atom.setAttr(_U(type), "A");
    atom.setAttr(_U(unit), "g/mol");
    atom.setAttr(_U(value), element->A() /* *(g/mole) */);
    geo.xmlElements[element] = e;
  }
  return e;
}

/// Dump material in GDML format to output stream
xml_h LCDDConverter::handleMaterial(const string& name, const TGeoMedium* medium) const {
  GeometryInfo& geo = data();
  xml_h mat = geo.xmlMaterials[medium];
  if (!mat) {
    xml_h obj;
    TGeoMaterial* m = medium->GetMaterial();
    double d = m->GetDensity();   //*(gram/cm3);
    if (d < 1e-25)
      d = 1e-25;
    mat = xml_elt_t(geo.doc, _U(material));
    mat.setAttr(_U(name), medium->GetName());
    mat.append(obj = xml_elt_t(geo.doc, _U(D)));
    obj.setAttr(_U(value), d /*  *(g/cm3)  */);
    obj.setAttr(_U(unit), "g/cm3");
    obj.setAttr(_U(type), "density");

    geo.checkMaterial(name, medium);
    if (name == "B") {
      cout << "Converting material:" << name << endl;
    }
    if (m->IsMixture()) {
      TGeoMixture *mix = (TGeoMixture*) m;
      const double *wmix = mix->GetWmixt();
      const int *nmix = mix->GetNmixt();
      double sum = 0e0;
      for (int i = 0, n = mix->GetNelements(); i < n; i++) {
        TGeoElement *elt = mix->GetElement(i);
        handleElement(elt->GetName(), elt);
        sum += wmix[i];
      }
      for (int i = 0, n = mix->GetNelements(); i < n; i++) {
        TGeoElement *elt = mix->GetElement(i);
        string formula = elt->GetTitle() + string("_elm");
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
    else {
      TGeoElement *elt = m->GetElement(0);
      cout << "Converting non mixing material:" << name << endl;
      xml_elt_t atom(geo.doc, _U(atom));
      handleElement(elt->GetName(), elt);
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
    geo.checkShape(name, shape);
    if (shape->IsA() == TGeoBBox::Class()) {
      const TGeoBBox* s = (const TGeoBBox*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(box)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(x), 2 * s->GetDX() * CM_2_MM);
      solid.setAttr(_U(y), 2 * s->GetDY() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDZ() * CM_2_MM);
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTube::Class()) {
      const TGeoTube* s = (const TGeoTube*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(tube)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rmin), s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);
      solid.setAttr(_U(startphi), 0e0);
      solid.setAttr(_U(deltaphi), 2 * M_PI);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoEltu::Class()) {
      const TGeoEltu* s = (const TGeoEltu*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(eltube)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(dx), s->GetA() * CM_2_MM);
      solid.setAttr(_U(dy), s->GetB() * CM_2_MM);
      solid.setAttr(_U(dz), s->GetDz() * CM_2_MM);
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* s = (const TGeoTubeSeg*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(tube)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rmin), s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), s->GetPhi2() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* s = (const TGeoTubeSeg*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(tube)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rmin), s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), s->GetPhi2() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoHype::Class()) {
      const TGeoHype* s = (const TGeoHype*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(hype)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rmin), 2 * s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), 2 * s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(outst), 2 * s->GetStOut() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTrd2::Class()) {
      const TGeoTrd2* s = (const TGeoTrd2*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(trd)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(x1), 2 * s->GetDx1() * CM_2_MM);
      solid.setAttr(_U(x2), 2 * s->GetDx2() * CM_2_MM);
      solid.setAttr(_U(y1), 2 * s->GetDy1() * CM_2_MM);
      solid.setAttr(_U(y2), 2 * s->GetDy2() * CM_2_MM);
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTrap::Class()) {
      const TGeoTrap* s = (const TGeoTrap*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(trap)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);   // Full zlen in GDML, half zlen in TGeo
      solid.setAttr(_U(x1), 2 * s->GetBl1() * CM_2_MM);
      solid.setAttr(_U(x2), 2 * s->GetTl1() * CM_2_MM);
      solid.setAttr(_U(x3), 2 * s->GetBl2() * CM_2_MM);
      solid.setAttr(_U(x4), 2 * s->GetTl2() * CM_2_MM);
      solid.setAttr(_U(y1), 2 * s->GetH1() * CM_2_MM);
      solid.setAttr(_U(y2), 2 * s->GetH2() * CM_2_MM);
      solid.setAttr(_U(alpha1), s->GetAlpha1() * DEGREE_2_RAD);
      solid.setAttr(_U(alpha2), s->GetAlpha2() * DEGREE_2_RAD);
      solid.setAttr(_U(theta), s->GetTheta() * DEGREE_2_RAD);
      solid.setAttr(_U(phi), s->GetPhi() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoPara::Class()) {
      const TGeoPara* s = (const TGeoPara*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(para)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(x), s->GetX() * CM_2_MM);
      solid.setAttr(_U(y), s->GetY() * CM_2_MM);
      solid.setAttr(_U(z), s->GetZ() * CM_2_MM);
      solid.setAttr(_U(alpha), s->GetAlpha() * DEGREE_2_RAD);
      solid.setAttr(_U(theta), s->GetTheta() * DEGREE_2_RAD);
      solid.setAttr(_U(phi), s->GetPhi() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoPgon::Class()) {
      const TGeoPgon* s = (const TGeoPgon*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(polyhedra)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), s->GetDphi() * DEGREE_2_RAD);
      solid.setAttr(_U(numsides), s->GetNedges());
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
      for (size_t i = 0; i < s->GetNz(); ++i) {
        zplane = xml_elt_t(geo.doc, _U(zplane));
        zplane.setAttr(_U(rmin), s->GetRmin(i) * CM_2_MM);
        zplane.setAttr(_U(rmax), s->GetRmax(i) * CM_2_MM);
        zplane.setAttr(_U(z), s->GetZ(i) * CM_2_MM);
        solid.append(zplane);
      }
    }
    else if (shape->IsA() == TGeoPcon::Class()) {
      const TGeoPcon* s = (const TGeoPcon*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(polycone)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), s->GetDphi() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
      for (size_t i = 0; i < s->GetNz(); ++i) {
        zplane = xml_elt_t(geo.doc, _U(zplane));
        zplane.setAttr(_U(rmin), s->GetRmin(i) * CM_2_MM);
        zplane.setAttr(_U(rmax), s->GetRmax(i) * CM_2_MM);
        zplane.setAttr(_U(z), s->GetZ(i) * CM_2_MM);
        solid.append(zplane);
      }
    }
    else if (shape->IsA() == TGeoConeSeg::Class()) {
      const TGeoConeSeg* s = (const TGeoConeSeg*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(polycone)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), (s->GetPhi2() - s->GetPhi1()) * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
      xml_elt_t zplane = xml_elt_t(geo.doc, _U(zplane));
      zplane.setAttr(_U(rmin), s->GetRmin1() * CM_2_MM);
      zplane.setAttr(_U(rmax), s->GetRmax1() * CM_2_MM);
      zplane.setAttr(_U(z), -s->GetDz() * CM_2_MM);
      solid.append(zplane);
      zplane = xml_elt_t(geo.doc, _U(zplane));
      zplane.setAttr(_U(rmin), s->GetRmin2() * CM_2_MM);
      zplane.setAttr(_U(rmax), s->GetRmax2() * CM_2_MM);
      zplane.setAttr(_U(z), s->GetDz() * CM_2_MM);
      solid.append(zplane);
#if 0
      solid.setAttr(_U(z), 2*s->GetDz()*CM_2_MM);
      solid.setAttr(_U(rmin1), s->GetRmin1()*CM_2_MM);
      solid.setAttr(_U(rmin2), s->GetRmin2()*CM_2_MM);
      solid.setAttr(_U(rmax1), s->GetRmax1()*CM_2_MM);
      solid.setAttr(_U(rmax2), s->GetRmax2()*CM_2_MM);
#endif
    }
    else if (shape->IsA() == TGeoCone::Class()) {
      const TGeoCone* s = (const TGeoCone*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(cone)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(z), 2 * s->GetDz() * CM_2_MM);
      solid.setAttr(_U(rmin1), s->GetRmin1() * CM_2_MM);
      solid.setAttr(_U(rmax1), s->GetRmax1() * CM_2_MM);
      solid.setAttr(_U(rmin2), s->GetRmin2() * CM_2_MM);
      solid.setAttr(_U(rmax2), s->GetRmax2() * CM_2_MM);
      solid.setAttr(_U(startphi), 0e0);
      solid.setAttr(_U(deltaphi), 2 * M_PI);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoParaboloid::Class()) {
      const TGeoParaboloid* s = (const TGeoParaboloid*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(paraboloid)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rlo), s->GetRlo() * CM_2_MM);
      solid.setAttr(_U(rhi), s->GetRhi() * CM_2_MM);
      solid.setAttr(_U(dz), s->GetDz() * CM_2_MM);
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoSphere::Class()) {
      const TGeoSphere* s = (const TGeoSphere*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(sphere)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rmin), s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), (s->GetPhi2() - s->GetPhi1()) * DEGREE_2_RAD);
      solid.setAttr(_U(starttheta), s->GetTheta1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltatheta), (s->GetTheta2() - s->GetTheta1()) * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoTorus::Class()) {
      const TGeoTorus* s = (const TGeoTorus*) shape;
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(torus)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(rtor), s->GetR() * CM_2_MM);
      solid.setAttr(_U(rmin), s->GetRmin() * CM_2_MM);
      solid.setAttr(_U(rmax), s->GetRmax() * CM_2_MM);
      solid.setAttr(_U(startphi), s->GetPhi1() * DEGREE_2_RAD);
      solid.setAttr(_U(deltaphi), s->GetDphi() * DEGREE_2_RAD);
      solid.setAttr(_U(aunit), "rad");
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoArb8::Class()) {
      TGeoArb8* s = (TGeoArb8*) shape;
      const double* vtx = s->GetVertices();
      geo.doc_solids.append(solid = xml_elt_t(geo.doc, _U(arb8)));
      solid.setAttr(_U(name), Unicode(name));
      solid.setAttr(_U(v1x), vtx[0] * CM_2_MM);
      solid.setAttr(_U(v1y), vtx[1] * CM_2_MM);
      solid.setAttr(_U(v2x), vtx[2] * CM_2_MM);
      solid.setAttr(_U(v2y), vtx[3] * CM_2_MM);
      solid.setAttr(_U(v3x), vtx[4] * CM_2_MM);
      solid.setAttr(_U(v3y), vtx[5] * CM_2_MM);
      solid.setAttr(_U(v4x), vtx[6] * CM_2_MM);
      solid.setAttr(_U(v4y), vtx[7] * CM_2_MM);
      solid.setAttr(_U(v5x), vtx[8] * CM_2_MM);
      solid.setAttr(_U(v5y), vtx[9] * CM_2_MM);
      solid.setAttr(_U(v6x), vtx[10] * CM_2_MM);
      solid.setAttr(_U(v6y), vtx[11] * CM_2_MM);
      solid.setAttr(_U(v7x), vtx[12] * CM_2_MM);
      solid.setAttr(_U(v7y), vtx[13] * CM_2_MM);
      solid.setAttr(_U(v8x), vtx[14] * CM_2_MM);
      solid.setAttr(_U(v8y), vtx[15] * CM_2_MM);
      solid.setAttr(_U(dz), s->GetDz() * CM_2_MM);
      solid.setAttr(_U(lunit), "mm");
    }
    else if (shape->IsA() == TGeoCompositeShape::Class()) {
      char text[32];
      const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      TGeoMatrix* rm = boolean->GetRightMatrix();
      TGeoMatrix* lm = boolean->GetLeftMatrix();
      TGeoShape* ls = boolean->GetLeftShape();
      TGeoShape* rs = boolean->GetRightShape();
      xml_h left = handleSolid(ls->GetName(), ls);
      xml_h right = handleSolid(rs->GetName(), rs);
      xml_h first(0), second(0);
      if (!left) {
        throw runtime_error("G4Converter: No left LCDD Solid present for composite shape:" + name);
      }
      if (!right) {
        throw runtime_error("G4Converter: No right LCDD Solid present for composite shape:" + name);
      }

      if ( oper == TGeoBoolNode::kGeoSubtraction )
      solid = xml_elt_t(geo.doc,_U(subtraction));
      else if ( oper == TGeoBoolNode::kGeoUnion )
      solid = xml_elt_t(geo.doc,_U(union));
      else if ( oper == TGeoBoolNode::kGeoIntersection )
      solid = xml_elt_t(geo.doc,_U(intersection));

      xml_h obj;
      geo.doc_solids.append(solid);
      solid.append(first = xml_elt_t(geo.doc, _U(first)));
      solid.setAttr(_U(name), Unicode(name));
      first.setAttr(_U(ref), ls->GetName());
      const double *tr = lm->GetTranslation();
      double phi = 0., theta = 0., psi = 0.;

      if ((tr[0] != 0.0) || (tr[1] != 0.0) || (tr[2] != 0.0)) {
        first.append(obj = xml_elt_t(geo.doc, _U(firstposition)));
        obj.setAttr(_U(x), tr[0] * CM_2_MM);
        obj.setAttr(_U(y), tr[1] * CM_2_MM);
        obj.setAttr(_U(z), tr[2] * CM_2_MM);
      }
      if (lm->IsRotation()) {
        TGeoMatrix& linv = lm->Inverse();
        XYZRotation rot = getXYZangles(linv.GetRotationMatrix());
        if ((rot.X() != 0.0) || (rot.Y() != 0.0) || (rot.Z() != 0.0)) {
          first.append(obj = xml_elt_t(geo.doc, _U(firstrotation)));
          obj.setAttr(_U(name), name);
          obj.setAttr(_U(x), rot.X());
          obj.setAttr(_U(y), rot.Y());
          obj.setAttr(_U(z), rot.Z());
          obj.setAttr(_U(unit), "rad");
        }
      }
      tr = rm->GetTranslation();
      solid.append(second = xml_elt_t(geo.doc, _U(second)));
      second.setAttr(_U(ref), rs->GetName());
      ::snprintf(text, sizeof(text), "_%p_", rm);
      string rnam = rs->GetName();
      rnam += text;
      if ((tr[0] != 0.0) || (tr[1] != 0.0) || (tr[2] != 0.0)) {
        xml_ref_t pos = handlePosition(rnam + "pos", rm);
        solid.setRef(_U(positionref), pos.name());
      }
      if (rm->IsRotation()) {
        TGeoMatrix& rinv = rm->Inverse();
        XYZRotation rot = getXYZangles(rinv.GetRotationMatrix());
        if ((rot.X() != 0.0) || (rot.Y() != 0.0) || (rot.Z() != 0.0)) {
          xml_ref_t rot = handleRotation(rnam + "rot", &rinv);
          solid.setRef(_U(rotationref), rot.name());
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
      geo.checkPosition(name, trafo);
      geo.doc_define.append(pos = xml_elt_t(geo.doc, _U(position)));
      pos.setAttr(_U(name), name);
      pos.setAttr(_U(x), tr[0] * CM_2_MM);
      pos.setAttr(_U(y), tr[1] * CM_2_MM);
      pos.setAttr(_U(z), tr[2] * CM_2_MM);
      pos.setAttr(_U(unit), "mm");
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
      geo.identity_pos.setAttr(_U(unit), "mm");
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
      geo.checkRotation(name, trafo);
      geo.doc_define.append(rot = xml_elt_t(geo.doc, _U(rotation)));
      rot.setAttr(_U(name), name);
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
xml_h LCDDConverter::handleVolume(const string& name, const TGeoVolume* volume) const {
  GeometryInfo& geo = data();
  xml_h vol = geo.xmlVolumes[volume];
  if (!vol) {
    const TGeoVolume* v = volume;
    Volume _v = Ref_t(v);
    string n = v->GetName();
    TGeoMedium* m = v->GetMedium();
    TGeoShape* s = v->GetShape();
    xml_ref_t sol = handleSolid(s->GetName(), s);

    geo.checkVolume(name, volume);
    if (v->IsAssembly()) {
      vol = xml_elt_t(geo.doc, _U(assembly));
      vol.setAttr(_U(name), n);
    }
    else {
      if (!sol)
        throw runtime_error("G4Converter: No LCDD Solid present for volume:" + n);
      else if (!m)
        throw runtime_error("G4Converter: No LCDD material present for volume:" + n);

      vol = xml_elt_t(geo.doc, _U(volume));
      vol.setAttr(_U(name), n);
      if (m) {
        string mat_name = m->GetName();
        xml_ref_t med = handleMaterial(mat_name, m);
        vol.setRef(_U(materialref), med.name());
      }
      vol.setRef(_U(solidref), sol.name());
    }
    geo.doc_structure.append(vol);
    geo.xmlVolumes[v] = vol;
    const TObjArray* dau = ((TGeoVolume*) v)->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      for (Int_t i = 0, n = dau->GetEntries(); i < n; ++i) {
        TGeoNode* node = (TGeoNode*) dau->At(i);
        handlePlacement(node->GetName(), node);
      }
    }
    if (geo.doc_header && dynamic_cast<const Volume::Object*>(volume)) {
      Region reg = _v.region();
      LimitSet lim = _v.limitSet();
      VisAttr vis = _v.visAttributes();
      SensitiveDetector det = _v.sensitiveDetector();
      if (det.isValid()) {
        xml_ref_t data = handleSensitive(det.name(), det.ptr());
        vol.setRef(_U(sdref), data.name());
      }
      if (reg.isValid()) {
        xml_ref_t data = handleRegion(reg.name(), reg.ptr());
        vol.setRef(_U(regionref), data.name());
      }
      if (lim.isValid()) {
        xml_ref_t data = handleLimitSet(lim.name(), lim.ptr());
        vol.setRef(_U(limitsetref), data.name());
      }
      if (vis.isValid()) {
        xml_ref_t data = handleVis(vis.name(), vis.ptr());
        vol.setRef(_U(visref), data.name());
      }
    }
  }
  return vol;
}

/// Dump logical volume in GDML format to output stream
xml_h LCDDConverter::handleVolumeVis(const string& name, const TGeoVolume* volume) const {
  GeometryInfo& geo = data();
  xml_h vol = geo.xmlVolumes[volume];
  if (!vol) {
    const TGeoVolume* v = volume;
    Volume _v = Ref_t(v);
    if (dynamic_cast<const Volume::Object*>(volume)) {
      VisAttr vis = _v.visAttributes();
      if (vis.isValid()) {
        geo.doc_structure.append(vol = xml_elt_t(geo.doc, _U(volume)));
        vol.setAttr(_U(name), v->GetName());
        xml_ref_t data = handleVis(vis.name(), vis.ptr());
        vol.setRef(_U(visref), data.name());
        geo.xmlVolumes[v] = vol;
      }
    }
  }
  return vol;
}

/// Dump logical volume in GDML format to output stream
void LCDDConverter::collectVolume(const string& name, const TGeoVolume* volume) const {
  Volume v = Ref_t(volume);
  if (dynamic_cast<const Volume::Object*>(volume)) {
    GeometryInfo& geo = data();
    Region reg = v.region();
    LimitSet lim = v.limitSet();
    SensitiveDetector det = v.sensitiveDetector();
    if (lim.isValid())
      geo.limits.insert(lim.ptr());
    if (reg.isValid())
      geo.regions.insert(reg.ptr());
    if (det.isValid())
      geo.sensitives.insert(det.ptr());
  }
  else {
    cout << "LCDDConverter::collectVolume: Skip volume:" << volume->GetName() << endl;
  }
}

void LCDDConverter::checkVolumes(const string& name, const TGeoVolume* volume) const {
  NameSet::const_iterator i = m_checkNames.find(name);
  if (i != m_checkNames.end()) {
    Volume v = Ref_t(volume);
    cout << "checkVolumes: Volume " << name << " ";
    if (dynamic_cast<const Volume::Object*>(volume)) {
      SensitiveDetector s = v.sensitiveDetector();
      VisAttr vis = v.visAttributes();
      if (s.isValid()) {
        cout << "of " << s.name() << " ";
      }
      else if (vis.isValid()) {
        cout << "with VisAttrs " << vis.name() << " ";
      }
    }
    cout << "has duplicate entries." << endl;
    return;
  }
  m_checkNames.insert(name);
}

/// Dump volume placement in GDML format to output stream
xml_h LCDDConverter::handlePlacement(const string& name, const TGeoNode* node) const {
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
      char text[32];
      ::snprintf(text, sizeof(text), "_%p_pos", node);
      xml_ref_t pos = handlePosition(name + text, m);
      ::snprintf(text, sizeof(text), "_%p_rot", node);
      xml_ref_t rot = handleRotation(name + text, m);
      place.setRef(_U(positionref), pos.name());
      place.setRef(_U(rotationref), rot.name());
    }
    if (geo.doc_root.tag() != "gdml") {
      if (dynamic_cast<const PlacedVolume::Object*>(node)) {
        PlacedVolume p = Ref_t(node);
        const PlacedVolume::VolIDs& ids = p.volIDs();
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

/// Convert the geometry type region into the corresponding LCDD object(s).
xml_h LCDDConverter::handleRegion(const std::string& name, const TNamed* region) const {
  GeometryInfo& geo = data();
  xml_h reg = geo.xmlRegions[region];
  if (!reg) {
    Region r = Ref_t(region);
    geo.doc_regions.append(reg = xml_elt_t(geo.doc, _U(region)));
    reg.setAttr(_U(name), r.name());
    reg.setAttr(_U(cut), r.cut());
    reg.setAttr(_U(eunit), r.energyUnit());
    reg.setAttr(_U(lunit), r.lengthUnit());
    reg.setAttr(_U(store_secondaries), r.storeSecondaries());
    geo.xmlRegions[region] = reg;
  }
  return reg;
}

/// Convert the geometry type LimitSet into the corresponding LCDD object(s)
xml_h LCDDConverter::handleLimitSet(const std::string& name, const TNamed* limitset) const {
  GeometryInfo& geo = data();
  xml_h xml = geo.xmlLimits[limitset];
  if (!xml) {
    LimitSet lim = Ref_t(limitset);
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
    geo.xmlLimits[limitset] = xml;
  }
  return xml;
}

/// Convert the segmentation of a SensitiveDetector into the corresponding LCDD object
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
      xml.setAttr(Unicode(v->name()), v->value());
    }
  }
  return xml;
}

/// Convert the geometry type SensitiveDetector into the corresponding LCDD object(s).
xml_h LCDDConverter::handleSensitive(const string& name, const TNamed* sens_det) const {
  GeometryInfo& geo = data();
  xml_h sensdet = geo.xmlSensDets[sens_det];
  if (!sensdet) {
    SensitiveDetector sd = Ref_t(sens_det);
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
      xml_ref_t ref = handleIdSpec(ro.idSpec().name(), ro.idSpec().ptr());
      sensdet.setRef(_U(idspecref), ref.name());
      xml_h seg = handleSegmentation(ro.segmentation());
      if (seg)
        sensdet.append(seg);
    }
    geo.xmlSensDets[sens_det] = sensdet;
  }
  return sensdet;
}

/// Convert the geometry id dictionary entry to the corresponding Xml object(s).
xml_h LCDDConverter::handleIdSpec(const std::string& name, const TNamed* id_spec) const {
  GeometryInfo& geo = data();
  xml_h id = geo.xmlIdSpecs[id_spec];
  if (!id) {
    int length = 0, start = 0;
    IDDescriptor desc = Ref_t(id_spec);
    geo.doc_idDict.append(id = xml_elt_t(geo.doc, _U(idspec)));
    id.setAttr(_U(name), name);
    const IDDescriptor::FieldMap& m = desc.fields();
    for (IDDescriptor::FieldMap::const_iterator i = m.begin(); i != m.end(); ++i) {
      xml_h idfield = xml_elt_t(geo.doc, _U(idfield));
#if 0
      const IDDescriptor::Field& f = (*i).second;
      start = f.first;
      length = f.second<0 ? -f.second : f.second;
      idfield.setAttr(_U(signed),f.second<0 ? true : false);
      idfield.setAttr(_U(label),(*i).first);
      idfield.setAttr(_U(length),length);
      idfield.setAttr(_U(start),start);
#else
      IDDescriptor::Field f = (*i).second;
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

/// Convert the geometry visualisation attributes to the corresponding LCDD object(s).
xml_h LCDDConverter::handleVis(const string& name, const TNamed* v) const {
  GeometryInfo& geo = data();
  xml_h vis = geo.xmlVis[v];
  if (!vis) {
    float r = 0, g = 0, b = 0;
    VisAttr attr = Ref_t(v);
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
    geo.xmlVis[v] = vis;
  }
  return vis;
}

/// Convert the electric or magnetic fields into the corresponding Xml object(s).
xml_h LCDDConverter::handleField(const std::string& name, const TNamed* f) const {
  GeometryInfo& geo = data();
  xml_h field = geo.xmlFields[f];
  if (!field) {
    Ref_t fld(f);
    string type = f->GetTitle();
    field = xml_elt_t(geo.doc, Unicode(type));
    field.setAttr(_U(name), f->GetName());
    fld = PluginService::Create<TNamed*>(type + "_Convert2LCDD", &m_lcdd, &field, &fld);
    cout << "++ " << (fld.isValid() ? "Converted" : "FAILED    to convert ") << " electromagnetic field:" << f->GetName()
        << " of type " << type << endl;
    if (!fld.isValid()) {
      PluginDebug dbg;
      PluginService::Create<TNamed*>(type + "_Convert2LCDD", &m_lcdd, &field, &fld);
      throw runtime_error(
          "Failed to locate plugin to convert electromagnetic field:" + string(f->GetName()) + " of type " + type + ". "
              + dbg.missingFactory(type));
    }
    geo.doc_fields.append(field);
  }
  return field;
}

/// Handle the geant 4 specific properties
void LCDDConverter::handleProperties(LCDD::Properties& prp) const {
  map<string, string> processors;
  static int s_idd = 9999999;
  string id;
  for (LCDD::Properties::const_iterator i = prp.begin(); i != prp.end(); ++i) {
    const string& nam = (*i).first;
    const LCDD::PropertyValues& vals = (*i).second;
    if (nam.substr(0, 6) == "geant4") {
      LCDD::PropertyValues::const_iterator id_it = vals.find("id");
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
    const LCDDConverter* ptr = this;
    string nam = (*i).second;
    const LCDD::PropertyValues& vals = prp[nam];
    string type = vals.find("type")->second;
    string tag = type + "_Geant4_action";
    long result = PluginService::Create<long>(tag, &m_lcdd, ptr, &vals);
    if (0 == result) {
      PluginDebug dbg;
      result = PluginService::Create<long>(tag, &m_lcdd, ptr, &vals);
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
    cout << "+++++ Executed Successfully LCDD setup module *" << type << "* ." << endl;
  }
}

/// Add header information in LCDD format
void LCDDConverter::handleHeader() const {
  GeometryInfo& geo = data();
  Header hdr = m_lcdd.header();
  xml_h obj;
  geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(detector)));
  obj.setAttr(_U(name), hdr.name());
  geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(generator)));
  obj.setAttr(_U(name), "LCDDConverter");
  obj.setAttr(_U(version), hdr.version());
  obj.setAttr(_U(file), hdr.url());
  obj.setAttr(_U(checksum), Unicode(m_lcdd.constantAsString("compact_checksum")));
  geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(author)));
  obj.setAttr(_U(name), hdr.author());
  geo.doc_header.append(obj = xml_elt_t(geo.doc, _U(comment)));
  obj.setText(hdr.comment());
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
  LCDD& lcdd = m_lcdd;
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createGDML with an invalid geometry!");
  }
  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);

  cout << "++ ==> Converting in memory detector description to GDML format..." << endl;
  const char* comment = "\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
      "      ++++   Linear collider detector description GDML in C++  ++++\n"
      "      ++++   DD4hep Detector description generator.            ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++   Parser:"
  XML_IMPLEMENTATION_TYPE
  "                ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++                              M.Frank CERN/LHCb      ++++\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
  XML::DocumentHandler docH;
  geo.doc = docH.create("gdml", comment);
  geo.doc_root = geo.doc.root();
  geo.doc_root.setAttr(Unicode("xmlns:xs"), "http://www.w3.org/2001/XMLSchema-instance");
  geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"),
      "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd");
  // geo.doc = docH.create("gdml_simple_extension",comment);
  // geo.doc_root.setAttr(Unicode("xmlns:gdml_simple_extension"),"http://www.example.org");
  // geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"),
  //     "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd");
  geo.doc_root.append(geo.doc_define = xml_elt_t(geo.doc, _U(define)));
  geo.doc_root.append(geo.doc_materials = xml_elt_t(geo.doc, _U(materials)));
  geo.doc_root.append(geo.doc_solids = xml_elt_t(geo.doc, _U(solids)));
  geo.doc_root.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));
  geo.doc_root.append(geo.doc_setup = xml_elt_t(geo.doc, _U(setup)));
  geo.doc_setup.setRef(_U(world), lcdd.worldVolume().name());
  geo.doc_setup.setAttr(_U(name), Unicode("Default"));
  geo.doc_setup.setAttr(_U(version), Unicode("1.0"));

  // Ensure that all required materials are present in the LCDD material table
#if 0
  const LCDD::HandleMap& mat = lcdd.materials();
  for(LCDD::HandleMap::const_iterator i=mat.begin(); i!=mat.end(); ++i)
  geo.materials.insert(dynamic_cast<TGeoMedium*>((*i).second.ptr()));
#endif

  // Start creating the objects for materials, solids and log volumes.
  handle(this, geo.materials, &LCDDConverter::handleMaterial);
  cout << "++ Handled " << geo.materials.size() << " materials." << endl;

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;

  handle(this, geo.solids, &LCDDConverter::handleSolid);
  cout << "++ Handled " << geo.solids.size() << " solids." << endl;

  handle(this, geo.volumes, &LCDDConverter::handleVolume);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;

  m_checkNames.clear();
  handle(this, geo.volumes, &LCDDConverter::checkVolumes);
  return geo.doc;
}

/// Create geometry conversion
xml_doc_t LCDDConverter::createVis(DetElement top) {
  LCDD& lcdd = m_lcdd;
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createLCDD with an invalid geometry!");
  }

  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);
  cout << "++ ==> Dump visualisation attributes from in memory detector description..." << endl;
  const char comment[] = "\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
      "      ++++   Linear collider detector description LCDD in C++  ++++\n"
      "      ++++   DD4hep Detector description generator.            ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++   Parser:"
  XML_IMPLEMENTATION_TYPE
  "                ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++                              M.Frank CERN/LHCb      ++++\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
  XML::DocumentHandler docH;
  xml_elt_t elt(0);
  geo.doc = docH.create("visualization", comment);
  geo.doc_root = geo.doc.root();
  geo.doc_root.append(geo.doc_display = xml_elt_t(geo.doc, _U(display)));
  geo.doc_root.append(geo.doc_structure = xml_elt_t(geo.doc, _U(structure)));

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  handle(this, geo.volumes, &LCDDConverter::handleVolumeVis);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;
  return geo.doc;
}

/// Create geometry conversion
xml_doc_t LCDDConverter::createLCDD(DetElement top) {
  LCDD& lcdd = m_lcdd;
  if (!top.isValid()) {
    throw runtime_error("Attempt to call createLCDD with an invalid geometry!");
  }

  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  collect(top, geo);
  const char comment[] = "\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
      "      ++++   Linear collider detector description LCDD in C++  ++++\n"
      "      ++++   DD4hep Detector description generator.            ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++   Parser:"
  XML_IMPLEMENTATION_TYPE
  "                ++++\n"
      "      ++++                                                     ++++\n"
      "      ++++                              M.Frank CERN/LHCb      ++++\n"
      "      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n  ";
  XML::DocumentHandler docH;
  xml_elt_t elt(0);
  geo.doc = docH.create("lcdd", comment);
  geo.doc_root = geo.doc.root();
  geo.doc_root.setAttr(Unicode("xmlns:lcdd"), "http://www.lcsim.org/schemas/lcdd/1.0");
  geo.doc_root.setAttr(Unicode("xmlns:xs"), "http://www.w3.org/2001/XMLSchema-instance");
  geo.doc_root.setAttr(Unicode("xs:noNamespaceSchemaLocation"), "http://www.lcsim.org/schemas/lcdd/1.0/lcdd.xsd");

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
  geo.doc_setup.setRef(_U(world), lcdd.worldVolume().name());
  geo.doc_setup.setAttr(_U(name), Unicode("Default"));
  geo.doc_setup.setAttr(_U(version), Unicode("1.0"));

  // Ensure that all required materials are present in the LCDD material table
  const LCDD::HandleMap& fld = lcdd.fields();
  for (LCDD::HandleMap::const_iterator i = fld.begin(); i != fld.end(); ++i)
    geo.fields.insert((*i).second.ptr());

  cout << "++ ==> Converting in memory detector description to LCDD format..." << endl;
  handleHeader();
  // Start creating the objects for materials, solids and log volumes.
  handle(this, geo.materials, &LCDDConverter::handleMaterial);
  cout << "++ Handled " << geo.materials.size() << " materials." << endl;

  handle(this, geo.volumes, &LCDDConverter::collectVolume);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;

  handle(this, geo.solids, &LCDDConverter::handleSolid);
  cout << "++ Handled " << geo.solids.size() << " solids." << endl;

  handle(this, geo.vis, &LCDDConverter::handleVis);
  cout << "++ Handled " << geo.solids.size() << " visualization attributes." << endl;

  handle(this, geo.sensitives, &LCDDConverter::handleSensitive);
  cout << "++ Handled " << geo.sensitives.size() << " sensitive detectors." << endl;

  handle(this, geo.limits, &LCDDConverter::handleLimitSet);
  cout << "++ Handled " << geo.limits.size() << " limit sets." << endl;

  handle(this, geo.regions, &LCDDConverter::handleRegion);
  cout << "++ Handled " << geo.regions.size() << " regions." << endl;

  handle(this, geo.volumes, &LCDDConverter::handleVolume);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;

  handle(this, geo.fields, &LCDDConverter::handleField);
  cout << "++ Handled " << geo.fields.size() << " fields." << endl;

  m_checkNames.clear();
  handle(this, geo.volumes, &LCDDConverter::checkVolumes);
#if 0
  //==================== Fields
  handleProperties(m_lcdd.properties());
#endif
  return geo.doc;
}

/// Helper constructor
LCDDConverter::GeometryInfo::GeometryInfo()
    : doc(0), doc_root(0), doc_header(0), doc_idDict(0), doc_detectors(0), doc_limits(0), doc_regions(0), doc_display(0), doc_gdml(
        0), doc_fields(0), doc_define(0), doc_materials(0), doc_solids(0), doc_structure(0), doc_setup(0) {
}

static long dump_output(xml_doc_t doc, int argc, char** argv) {
  XML::DocumentHandler docH;
  return docH.output(doc, argc > 0 ? argv[0] : "");
}

static long create_gdml(LCDD& lcdd, int argc, char** argv) {
  LCDDConverter wr(lcdd);
  return dump_output(wr.createGDML(lcdd.world()), argc, argv);
}

static long create_lcdd(LCDD& lcdd, int argc, char** argv) {
  LCDDConverter wr(lcdd);
  return dump_output(wr.createLCDD(lcdd.world()), argc, argv);
}

static long create_vis(LCDD& lcdd, int argc, char** argv) {
  LCDDConverter wr(lcdd);
  return dump_output(wr.createVis(lcdd.world()), argc, argv);
}

static long create_visASCII(LCDD& lcdd, int argc, char** argv) {
  LCDDConverter wr(lcdd);
  xml_doc_t doc = wr.createVis(lcdd.world());
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
    os << "vol:" << vol.nameStr() << sep << "vis:" << vis.nameStr() << sep << "visible:" << vis.visible() << sep << "r:"
        << col.R() << sep << "g:" << col.G() << sep << "b:" << col.B() << sep << "alpha:" << col.alpha() << sep << "line_style:"
        << vis.attr < string > (_U(line_style)) << sep << "drawing_style:" << vis.attr < string
        > (_U(drawing_style)) << sep << "show_daughters:" << vis.show_daughters() << sep << endl;
  }
  os.close();
  return 1;
}

DECLARE_APPLY(DD4hepGeometry2VIS, create_vis);
DECLARE_APPLY(DD4hepGeometry2VISASCII, create_visASCII);
DECLARE_APPLY(DD4hepGeometry2GDML, create_gdml);
DECLARE_APPLY(DD4hepGeometry2LCDD, create_lcdd);
