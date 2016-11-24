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

// Framework include files
#include "DD4hep/LCDD.h"
#include "SimpleGDMLWriter.h"

// ROOT includes
#include "TROOT.h"
#include "TColor.h"
#include "TGeoShape.h"
#include "TGeoCone.h"
#include "TGeoParaboloid.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoTube.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TClass.h"
#include "TMath.h"

// C/C++ include files
#include <iostream>

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

namespace {
  string indent = "";

  void getAngles(const Double_t* m, Double_t &phi, Double_t &theta, Double_t &psi) {
    // Retreive Euler angles.
    // Check if theta is 0 or 180.
    if (TMath::Abs(1. - TMath::Abs(m[8])) < 1.e-9) {
      theta = TMath::ACos(m[8]) * RAD_2_DEGREE;
      phi = TMath::ATan2(-m[8] * m[1], m[0]) * RAD_2_DEGREE;
      psi = 0.;   // convention, phi+psi matters
      return;
    }
    // sin(theta) != 0
    phi = TMath::ATan2(m[2], -m[5]);
    Double_t sphi = TMath::Sin(phi);
    if (TMath::Abs(sphi) < 1.e-9)
      theta = -TMath::ASin(m[5] / TMath::Cos(phi)) * RAD_2_DEGREE;
    else
      theta = TMath::ASin(m[2] / sphi) * RAD_2_DEGREE;
    phi *= RAD_2_DEGREE;
    psi = TMath::ATan2(m[6], m[7]) * RAD_2_DEGREE;
  }
}

/// Dump logical volume in GDML format to output stream
void* SimpleGDMLWriter::handleVolume(const string& name, const TGeoVolume* volume) const {
  Volume vol(volume);
  VisAttr vis = vol.visAttributes();
  TGeoShape* shape = volume->GetShape();
  TGeoMedium* medium = volume->GetMedium();
  int num = volume->GetNdaughters();

  m_output << "\t\t<volume name=\"" << name << "\">" << endl;
  m_output << "\t\t\t<solidref ref=\"" << shape->GetName() << "\"/>" << endl;
  m_output << "\t\t\t<materialref ref=\""
           << (medium ? medium->GetName() : "UnknownMaterial") << "\"/>" << endl;
  if (vis.isValid()) {
    m_output << "\t\t\t<visref ref=\"" << vis.name() << "\"/>" << endl;
  }
  if (num > 0) {
    for (int i = 0; i < num; ++i) {
      //TGeoNode* n = volume->GetNode(i);
      TGeoNode* n   = volume->GetNode(volume->GetNode(i)->GetName());
      TGeoVolume* v = n->GetVolume();
      TGeoMatrix* m = n->GetMatrix();
      m_output << "\t\t\t<physvol>" << endl;
      m_output << "\t\t\t\t<volumeref ref=\"" << v->GetName() << "\"/>" << endl;
      if (m) {
        if (m->IsTranslation()) {
          m_output << "\t\t\t\t<positionref ref=\"" << n->GetName() << "_pos\"/>" << endl;
        }
        if (m->IsRotation()) {
          m_output << "\t\t\t\t<rotationref ref=\"" << n->GetName() << "_rot\"/>" << endl;
        }
      }
      m_output << "\t\t\t</physvol>" << endl;
    }
  }
  m_output << "\t\t</volume>" << endl;
  return 0;
}

/// Dump solid in GDML format to output stream
void* SimpleGDMLWriter::handleSolid(const string& name, const TGeoShape* shape) const {
  if (shape) {
    if (shape->IsA() == TGeoBBox::Class()) {
      const TGeoBBox* s = (const TGeoBBox*) shape;
      m_output << "\t\t<box name=\"" << name << "_shape\" x=\"" << s->GetDX() << "\" y=\"" << s->GetDY() << "\" z=\""
               << s->GetDZ() << "\" lunit=\"cm\"/>" << endl;
    }
    else if (shape->IsA() == TGeoTube::Class()) {
      const TGeoTube* s = (const TGeoTube*) shape;
      m_output << "\t\t<tube name=\"" << name << "_shape\" rmin=\"" << s->GetRmin() << "\" rmax=\"" << s->GetRmax() << "\" z=\""
               << s->GetDz() << "\" startphi=\"0.0\" deltaphi=\"360.0\" aunit=\"deg\" lunit=\"cm\"/>" << endl;
    }
    else if (shape->IsA() == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* s = (const TGeoTubeSeg*) shape;
      m_output << "\t\t<tube name=\"" << name << "_shape\" rmin=\"" << s->GetRmin() << "\" rmax=\"" << s->GetRmax() << "\" z=\""
               << s->GetDz() << "\" startphi=\"" << s->GetPhi1() << "\" deltaphi=\"" << s->GetPhi2()
               << "\" aunit=\"deg\" lunit=\"cm\"/>" << endl;
    }
    else if (shape->IsA() == TGeoTrd1::Class()) {
      const TGeoTrd1* s = (const TGeoTrd1*) shape;
      m_output << "\t\t<tube name=\"" << name << "_shape\" x1=\"" << s->GetDx1() << "\" x2=\"" << s->GetDx2() << "\" y1=\""
               << s->GetDy() << "\" y2=\"" << s->GetDy() << "\" z=\"" << s->GetDz() << "\" lunit=\"cm\"/>" << endl;
    }
    else if (shape->IsA() == TGeoTrd2::Class()) {
      const TGeoTrd2* s = (const TGeoTrd2*) shape;
      m_output << "\t\t<tube name=\"" << name << "_shape\" x1=\"" << s->GetDx1() << "\" x2=\"" << s->GetDx2() << "\" y1=\""
               << s->GetDy1() << "\" y2=\"" << s->GetDy2() << "\" z=\"" << s->GetDz() << "\" lunit=\"cm\"/>" << endl;
    }
    else if (shape->IsA() == TGeoPgon::Class()) {
      const TGeoPgon* s = (const TGeoPgon*) shape;
      m_output << "\t\t<polyhedra name=\"" << name << "_shape\" startphi=\"" << s->GetPhi1() << "\" deltaphi=\"" << s->GetDphi()
               << "\" numsides=\"" << s->GetNedges() << "\" aunit=\"deg\" lunit=\"cm\">" << endl;
      for (int i = 0; i < s->GetNz(); ++i) {
        m_output << "\t\t\t<zplane z=\"" << s->GetZ(i) << "\" rmin=\"" << s->GetRmin(i) << "\" rmax=\"" << s->GetRmax(i)
                 << "\" lunit=\"cm\"/>" << endl;
      }
      m_output << "\t\t</polyhedra>" << endl;
    }
    else if (shape->IsA() == TGeoPcon::Class()) {
      const TGeoPcon* s = (const TGeoPcon*) shape;
      m_output << "\t\t<polycone name=\"" << name << "_shape\" startphi=\"" << s->GetPhi1() << "\" deltaphi=\"" << s->GetDphi()
               << "\" aunit=\"deg\" lunit=\"cm\">" << endl;
      for (int i = 0; i < s->GetNz(); ++i) {
        m_output << "\t\t\t<zplane z=\"" << s->GetZ(i) << "\" rmin=\"" << s->GetRmin(i) << "\" rmax=\"" << s->GetRmax(i)
                 << "\" lunit=\"cm\"/>" << endl;
      }
      m_output << "\t\t</polycone>" << endl;
    }
    else if (shape->IsA() == TGeoCompositeShape::Class()) {
      string nn = name;
      const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();

      handleSolid(name + "_left", boolean->GetLeftShape());
      handleSolid(name + "_right", boolean->GetRightShape());

      if (oper == TGeoBoolNode::kGeoSubtraction)
        m_output << "\t\t<subtraction name=\"" << nn << "\">" << endl;
      else if (oper == TGeoBoolNode::kGeoUnion)
        m_output << "\t\t<union name=\"" << nn << "\">" << endl;
      else if (oper == TGeoBoolNode::kGeoIntersection)
        m_output << "\t\t<intersection name=\"" << nn << "\">" << endl;

      m_output << "\t\t\t<first ref=\"" << nn << "_left\"/>" << endl;
      m_output << "\t\t\t<second ref=\"" << nn << "_right\"/>" << endl;
      indent = "\t";
      handleTransformation("", boolean->GetRightMatrix());
      indent = "";

      if (oper == TGeoBoolNode::kGeoSubtraction)
        m_output << "\t\t</subtraction>" << endl;
      else if (oper == TGeoBoolNode::kGeoUnion)
        m_output << "\t\t</union>" << endl;
      else if (oper == TGeoBoolNode::kGeoIntersection)
        m_output << "\t\t</intersection>" << endl;
    }
    else {
      cerr << "Failed to handle unknwon solid shape:" << shape->IsA()->GetName() << endl;
    }
  }
  return 0;
}

/// Dump structure information in GDML format to output stream
void SimpleGDMLWriter::handleStructure(const VolumeVector& volset) const {
  m_output << "\t<structure>" << endl;
  for (VolumeVector::const_iterator i = volset.begin(); i != volset.end(); ++i)
    handleVolume((*i)->GetName(), (*i));
  m_output << "\t</structure>" << endl;
}

/// Dump single volume transformation in GDML format to output stream
void* SimpleGDMLWriter::handleTransformation(const string& name, const TGeoMatrix* m) const {
  if (m) {
    if (m->IsTranslation()) {
      const Double_t* f = m->GetTranslation();
      m_output << indent << "\t\t<position ";
      if (!name.empty())
        m_output << "name=\"" << name << "_pos\" ";
      m_output << "x=\"" << f[0] << "\" " << "y=\"" << f[1] << "\" " << "z=\"" << f[2] << "\" unit=\"cm\"/>" << endl;
    }
    if (m->IsRotation()) {
      const Double_t* mat = m->GetRotationMatrix();
      Double_t theta = 0.0, phi = 0.0, psi = 0.0;
      getAngles(mat, theta, phi, psi);
      m_output << indent << "\t\t<rotation ";
      if (!name.empty())
        m_output << "name=\"" << name << "_rot\" ";
      m_output << "x=\"" << theta << "\" " << "y=\"" << psi << "\" " << "z=\"" << phi << "\" unit=\"deg\"/>" << endl;
    }
  }
  return 0;
}

/// Dump Transformations in GDML format to output stream
void SimpleGDMLWriter::handleTransformations(const TransformSet& trafos) const {
  m_output << "\t<define>" << endl;
  for (TransformSet::const_iterator i = trafos.begin(); i != trafos.end(); ++i)
    handleTransformation((*i).first, (*i).second);
  m_output << "\t</define>" << endl;
}

/// Dump all solids in GDML format to output stream
void SimpleGDMLWriter::handleSolids(const SolidSet& solids) const {
  m_output << "\t<solids>" << endl;
  for (SolidSet::const_iterator i = solids.begin(); i != solids.end(); ++i)
    handleSolid((*i)->GetName(), (*i));
  m_output << "\t</solids>" << endl;
}

/// Dump all constants in GDML format to output stream
void SimpleGDMLWriter::handleDefines(const LCDD::HandleMap& defs) const {
  m_output << "\t<define>" << endl;
  for (LCDD::HandleMap::const_iterator i = defs.begin(); i != defs.end(); ++i)
    m_output << "\t\t<constant name=\"" << (*i).second->GetName() << "\" value=\"" << (*i).second->GetTitle() << "\" />"
             << endl;
  m_output << "\t</define>" << endl;
}

/// Dump all visualisation specs in LCDD format to output stream
void SimpleGDMLWriter::handleVisualisation(const VisRefs& vis) const {
  m_output << "\t<display>" << endl;
  for (VisRefs::const_iterator i = vis.begin(); i != vis.end(); ++i) {
    VisAttr v = Ref_t(*i);
    if (v.isValid()) {
      float r = 1., g = 1., b = 1., alpha = 1.;
      TColor *color = gROOT->GetColor(v.color());
      if (color) {
        color->GetRGB(r, g, b);
        alpha = color->GetAlpha();
      }
      const char* line_style = 0, *draw_style = 0;
      switch (v.lineStyle()) {
      case VisAttr::DASHED:
        line_style = "broken";
        break;
      case VisAttr::SOLID:
      default:
        line_style = "unbroken";
        break;
      }
      switch (v.drawingStyle()) {
      case VisAttr::WIREFRAME:
        draw_style = "wireframe";
        break;
      default:
        break;
      }

      m_output << "\t\t<vis name=\"" << v.name() << "\" ";
      if (line_style)
        m_output << "linestyle=\"" << line_style << "\" ";
      if (draw_style)
        m_output << "drawingStyle=\"" << draw_style << "\" ";
      m_output << "show_daughters=\"" << (const char*) (v.showDaughters() ? "true" : "false") << "\" " << "visible=\""
               << (const char*) (v.visible() ? "true" : "false") << "\" >" << endl << "\t\t\t<color R=\"" << r << "\" G=\"" << g
               << "\" B=\"" << b << "\" alpha=\"" << alpha << "\" />" << endl << "\t\t</vis>" << endl;
    }
  }
  m_output << "\t</display>" << endl;
}

void SimpleGDMLWriter::create(DetElement top) {
  GeometryInfo geo;
  collect(top, geo);
  //handleSetup(LCDD::getInstance().header());
  handleDefines(LCDD::getInstance().constants());
  handleVisualisation(geo.vis);
  handleTransformations(geo.trafos);
  handleSolids(geo.solids);
  handleStructure(geo.volumes);
}
