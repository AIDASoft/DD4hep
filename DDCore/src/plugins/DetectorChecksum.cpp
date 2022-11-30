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
#include <DD4hep/Plugins.h>
#include <DD4hep/Printout.h>
#include <DD4hep/FieldTypes.h>
#include <DD4hep/DetectorTools.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/AlignmentData.h>
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/detail/ObjectsInterna.h>
#include <DD4hep/detail/DetectorInterna.h>
#include "DetectorChecksum.h"

// ROOT includes
#include <TROOT.h>
#include <TClass.h>
#include <TColor.h>
#include <TGeoBoolNode.h>

// C/C++ include files
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace dd4hep;
using namespace dd4hep::detail;

namespace {
  bool is_volume(const TGeoVolume* volume)  {
    Volume v(volume);
    return v.data() != 0;
  }
}

/// Initializing Constructor
DetectorChecksum::DetectorChecksum(Detector& description)
  : m_detDesc(description), m_dataPtr(0) {
}

DetectorChecksum::~DetectorChecksum() {
  if (m_dataPtr)
    delete m_dataPtr;
  m_dataPtr = 0;
}

template <typename T> std::string DetectorChecksum::refName(T handle)  const  {
  return handle->GetName();
}
template <> std::string DetectorChecksum::refName(Segmentation handle)  const  {
  return handle->name();
}
template <typename T> std::string DetectorChecksum::attr_name(T handle)  const  {
  std::string n = " name=\"" + refName(handle)+"\"";
  return n;
}

void DetectorChecksum::configure()   {
  m_len_unit = _toDouble(m_len_unit_nam)/m_len__def;
  m_ang_unit = _toDouble(m_ang_unit_nam)/m_ang__def;
  m_ene_unit = _toDouble(m_ene_unit_nam)/m_ene__def;
  m_densunit = _toDouble(m_densunit_nam)/m_dens_def;
  m_atomunit = _toDouble(m_atomunit_nam)/m_atom_def;
  if ( debug > 1 )   {
    printout(INFO,"DetectorChecksum","+++ Float precision: %d", precision);
    printout(INFO,"DetectorChecksum","+++ Unit of length:  %-12s -> %f", m_len_unit_nam.c_str(), m_len_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of angle:   %-12s -> %f", m_ang_unit_nam.c_str(), m_ang_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of energy:  %-12s -> %f", m_ene_unit_nam.c_str(), m_ene_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of density: %-12s -> %f", m_densunit_nam.c_str(), m_densunit);
    printout(INFO,"DetectorChecksum","+++ Unit of density: %-12s -> %f", m_atomunit_nam.c_str(), m_atomunit);
  }
}

std::stringstream DetectorChecksum::logger()   const    {
  std::stringstream log;
  log.setf(std::ios::fixed, std::ios::floatfield);
  log << std::setprecision(precision);
  return log;
}

/// Dump element in GDML format to output stream
const std::string& DetectorChecksum::handleElement(Atom element) const {
  auto& geo = data().mapOfElements;
  auto  iel = geo.find(element);
  if ( iel == geo.end() )   {
    if ( element->HasIsotopes() )   {
      except("DetectorChecksum","%s: Atoms with isotopes not implemented", element.name());
    }
    else   {
      std::stringstream log = logger();
      log << "<element"         << attr_name(element)
	  << " Z=\""            << element->Z()   << "\""
	  << " formula=\""      << element.name() << "\""
	  << ">" << newline
	  << " <atom unit=\"" << m_atomunit_nam 
	  << "\" value=\""    << std::scientific << element->A()/m_atomunit << "\"/>" << newline
	  << "</element>";
      iel = geo.emplace(element, log.str()).first;
      if ( debug > 3 ) std::cout << log.str() << std::endl;
    }
  }
  return iel->second;
}

/// Dump material in GDML format to output stream
const std::string& DetectorChecksum::handleMaterial(Material medium) const {
  auto& geo = data().mapOfMaterials;
  auto  ima = geo.find(medium);
  if ( ima == geo.end() )   {
    std::stringstream log = logger();
    auto* mat = medium->GetMaterial();
    log << "<material"   << attr_name(medium) << newline;
    if ( mat->IsMixture() )   {
      std::map<std::string, double>  elts;
      TGeoMixture* mix = (TGeoMixture*)mat;
      for (Int_t idx = 0; idx < mix->GetNelements(); idx++) {
	TGeoElement *e = mix->GetElement(idx);
	handleElement(e);
	elts[refName(e)] = mix->GetWmixt()[idx];
      }
      log << "/>" << newline;
      for( const auto& w : elts )  {
	log << " <fraction ref=\"" << w.first << "\" n=\"" << w.second << "\">" << newline;
      }
    }
    else   {
      log << " A=\""          << mat->GetA()    << "\""
	  << " Z=\""          << mat->GetZ()    << "\">" << newline
	  << " <atom unit=\"" << m_atomunit_nam << "\" value=\"" << mat->GetA()/m_atomunit << "\"/>" << newline;
    }
    log << " <D unit=\"" << m_densunit_nam << "\" value=\"" << mat->GetDensity()/m_densunit << "\"/>" << newline;
    log << "</material>";
    ima = geo.emplace(medium, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ima->second;
}

/// Dump solid in GDML format to output stream
const std::string& DetectorChecksum::handleSolid(Solid solid) const {
  auto& geo = data().mapOfSolids;
  auto  iso = geo.find(solid);
  if ( iso == geo.end() )   {
    const TGeoShape* shape = solid.ptr();
    if ( !shape )  {
      iso = geo.emplace(solid, std::string("<shape type=\"INVALID\"></shape>)")).first;
      return iso->second;
    }

    TClass*     cl  = shape->IsA();
    std::string nam = attr_name(solid);
    auto        log = logger();
    if ( cl == TGeoBBox::Class() )   {
      TGeoBBox* sh = (TGeoBBox*) shape;
      log << "<box" << nam
	  << " lunit=\"" << m_len_unit_nam << "\""
	  << " x=\"" << 2.0*sh->GetDX()/m_len_unit << "\""
	  << " y=\"" << 2.0*sh->GetDY()/m_len_unit << "\""
	  << " z=\"" << 2.0*sh->GetDZ()/m_len_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoHalfSpace::Class() ) {
      TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
      const auto& pnt = sh->GetPoint();
      const auto& nrm = sh->GetNorm();
      log << "<halfspace" << nam
	  << " lunit=\"" << m_len_unit_nam << "\">" << newline
	  << " <point x=\""        << pnt[0]/m_len_unit << "\""
	  << " y=\""               << pnt[1]/m_len_unit << "\""
	  << " z=\""               << pnt[2]/m_len_unit << "\"/>" << newline
	  << " <normal x=\""       << nrm[0]/m_len_unit << "\""
	  << " y=\""               << nrm[1]/m_len_unit << "\""
	  << " z=\""               << nrm[2]/m_len_unit << "\"/>" << newline
	  << "</halfspace> << newline";
    }
    else if ( cl == TGeoTube::Class() || cl == TGeoTubeSeg::Class() ) {
      const TGeoTube* sh = (const TGeoTube*) shape;
      log << "<tube" << nam
	  << " lunit=\""    << m_len_unit_nam           << "\""
	  << " aunit=\""    << m_ang_unit_nam           << "\""
	  << " rmin=\""     << sh->GetRmin()/m_len_unit << "\""
	  << " rmax=\""     << sh->GetRmax()/m_len_unit << "\""
	  << " dz=\""       << 2*sh->GetDz()/m_len_unit << "\""
	  << " startphi=\"" << 0.0                 << "\""
	  << " deltaphi=\"" << 360.0/m_ang_unit         << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoTubeSeg::Class() ) {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      log << "<tube" << nam
	  << " lunit=\""    << m_len_unit_nam             << "\""
	  << " aunit=\""    << m_ang_unit_nam             << "\""
	  << " rmin=\""     << sh->GetRmin()/m_len_unit   << "\""
	  << " rmax=\""     << sh->GetRmax()/m_len_unit   << "\""
	  << " dz=\""       << 2*sh->GetDz()/m_len_unit   << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit   << "\""
	  << " deltaphi=\"" << (sh->GetPhi2() - sh->GetPhi1())/m_ang_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoCtub::Class() ) {
      const TGeoCtub* sh = (const TGeoCtub*) shape;
      const Double_t* hi = sh->GetNhigh();
      const Double_t* lo = sh->GetNlow();
      log << "<cutTube" << nam
	  << " lunit=\""    << m_len_unit_nam             << "\""
	  << " aunit=\""    << m_ang_unit_nam             << "\""
	  << " rmin=\""     << sh->GetRmin()/m_len_unit   << "\""
	  << " rmax=\""     << sh->GetRmax()/m_len_unit   << "\""
	  << " dz=\""       << 2*sh->GetDz()/m_len_unit   << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit   << "\""
	  << " deltaphi=\"" << (sh->GetPhi2() - sh->GetPhi1())/m_ang_unit << "\""
	  << " lowX=\""     << lo[0]/m_len_unit           << "\""
	  << " lowY=\""     << lo[1]/m_len_unit           << "\""
	  << " lowZ=\""     << lo[2]/m_len_unit           << "\""
	  << " highX=\""    << hi[0]/m_len_unit           << "\""
	  << " highY=\""    << hi[1]/m_len_unit           << "\""
	  << " highZ=\""    << hi[2]/m_len_unit           << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoEltu::Class() ) {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      log << "<eltube" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " dx=\""     << sh->GetA()/m_len_unit      << "\""
	  << " dy=\""     << sh->GetB()/m_len_unit      << "\""
	  << " dz=\""     << sh->GetDz()/m_len_unit     << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoTrd1::Class() ) {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      log << "<trd" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " x1=\""     << 2*sh->GetDx1()/m_len_unit  << "\""
	  << " x2=\""     << 2*sh->GetDx2()/m_len_unit  << "\""
	  << " y1=\""     << 2*sh->GetDy()/m_len_unit   << "\""
	  << " y2=\""     << 2*sh->GetDy()/m_len_unit   << "\""
	  << " z=\""      << 2*sh->GetDz()/m_len_unit   << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoTrd2::Class() ) {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      log << "<trd" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " x1=\""     << 2*sh->GetDx1()/m_len_unit  << "\""
	  << " x2=\""     << 2*sh->GetDx2()/m_len_unit  << "\""
	  << " y1=\""     << 2*sh->GetDy1()/m_len_unit  << "\""
	  << " y2=\""     << 2*sh->GetDy2()/m_len_unit  << "\""
	  << " z=\""      << 2*sh->GetDz()/m_len_unit   << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoTrap::Class() )   {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      log << "<trap" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " z=\""      << 2*sh->GetDz()/m_len_unit   << "\""
	  << " theta=\""  << sh->GetTheta()/m_ang_unit  << "\""
	  << " phi=\""    << sh->GetPhi()/m_ang_unit    << "\""
	  << " x1=\""     << 2*sh->GetBl1()/m_len_unit  << "\""
	  << " x2=\""     << 2*sh->GetTl1()/m_len_unit  << "\""
	  << " x3=\""     << 2*sh->GetBl2()/m_len_unit  << "\""
	  << " x4=\""     << 2*sh->GetTl2()/m_len_unit  << "\""
	  << " y1=\""     << 2*sh->GetH1()/m_len_unit   << "\""
	  << " y2=\""     << 2*sh->GetH2()/m_len_unit   << "\""
	  << " alpha1=\"" << sh->GetAlpha1()/m_ang_unit << "\""
	  << " alpha2=\"" << sh->GetAlpha2()/m_ang_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoHype::Class() )   {
      const TGeoHype* sh = (const TGeoHype*) shape;
      log << "<hype" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin=\""   << sh->GetRmin()/m_len_unit   << "\""
	  << " rmax=\""   << sh->GetRmax()/m_len_unit   << "\""
	  << " inst=\""   << sh->GetStIn()/m_ang_unit   << "\""
	  << " outst=\""  << sh->GetStOut()/m_ang_unit  << "\""
	  << " z=\""      << 2*sh->GetDz()/m_len_unit   << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoPgon::Class() )   {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      log << "<polyhedra" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit << "\""
	  << " deltaphi=\"" << sh->GetDphi()/m_ang_unit << "\""
	  << " numsides=\"" << sh->GetNedges()     << "\">" << newline;
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        log << " <zplane z=\"" << sh->GetZ(i)/m_len_unit 
	    << "\" rmin=\"" << sh->GetRmin(i) << "\""
	    << "\" rmax=\"" << sh->GetRmax(i) << "\"/>" << newline;
      }
      log << "</polyhedra>" << newline;
    }
    else if ( cl == TGeoPcon::Class() )  {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      log << "<polycone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit << "\""
	  << " deltaphi=\"" << sh->GetDphi()/m_ang_unit << "\">" << newline;
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        log << " <zplane z=\"" << sh->GetZ(i)/m_len_unit 
	    << "\" rmin=\"" << sh->GetRmin(i) << "\""
	    << "\" rmax=\"" << sh->GetRmax(i) << "\"/>" << newline;
      }
      log << "</polycone>" << newline;
    }
    else if ( cl == TGeoCone::Class() )  {
      const TGeoCone* sh = (const TGeoCone*) shape;
      log << "<cone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin1=\""  << sh->GetRmin1()/m_len_unit  << "\""
	  << " rmin2=\""  << sh->GetRmin2()/m_len_unit  << "\""
	  << " rmax1=\""  << sh->GetRmax1()/m_len_unit  << "\""
	  << " rmax2=\""  << sh->GetRmax2()/m_len_unit  << "\""
	  << " z=\""      << sh->GetDz()/m_len_unit     << "\""
	  << " startphi=\"" << 0.0/m_ang_unit           << "\""
	  << " deltaphi=\"" << 360.0/m_ang_unit         << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoConeSeg::Class() )  {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      log << "<cone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin1=\""  << sh->GetRmin1()/m_len_unit  << "\""
	  << " rmin2=\""  << sh->GetRmin2()/m_len_unit  << "\""
	  << " rmax1=\""  << sh->GetRmax1()/m_len_unit  << "\""
	  << " rmax2=\""  << sh->GetRmax2()/m_len_unit  << "\""
	  << " z=\""      << sh->GetDz()/m_len_unit     << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit << "\""
	  << " deltaphi=\"" << (sh->GetPhi1()-sh->GetPhi1())/m_ang_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoParaboloid::Class() )  {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      log << "<paraboloid" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " rlo=\""    << sh->GetRlo()/m_len_unit    << "\""
	  << " rhi=\""    << sh->GetRhi()/m_len_unit    << "\""
	  << " z=\""      << sh->GetDz()/m_len_unit     << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoSphere::Class() )   {
      const TGeoSphere* sh = (const TGeoSphere*) shape;
      log << "<sphere" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin=\""   << sh->GetRmin()/m_len_unit   << "\""
	  << " rmax=\""   << sh->GetRmax()/m_len_unit   << "\""
	  << " startphi=\""   << sh->GetPhi1()/m_ang_unit << "\""
	  << " deltaphi=\""   << (sh->GetPhi1()-sh->GetPhi1())/m_ang_unit << "\""
	  << " starttheta=\"" << sh->GetTheta1()/m_ang_unit << "\""
	  << " deltatheta=\"" << (sh->GetTheta1()-sh->GetTheta1())/m_ang_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoTorus::Class() )   {
      const TGeoTorus* sh = (const TGeoTorus*) shape;
      log << "<torus" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rtor=\""   << sh->GetR()/m_len_unit      << "\""
	  << " rmin=\""   << sh->GetRmin()/m_len_unit   << "\""
	  << " rmax=\""   << sh->GetRmax()/m_len_unit   << "\""
	  << " startphi=\""   << sh->GetPhi1()/m_ang_unit << "\""
	  << " deltaphi=\""   << sh->GetDphi()/m_ang_unit << "\""
	  << "/>" << newline;
    }
    else if ( cl == TGeoArb8::Class() )   {
      TGeoArb8* sh = (TGeoArb8*) shape;
      const Double_t* v = sh->GetVertices();
      log << "<arb8" << nam
          << " v1x=\""    << v[0]/m_len_unit            << "\""
          << " v1y=\""    << v[1]/m_len_unit            << "\""
          << " v2x=\""    << v[2]/m_len_unit            << "\""
          << " v2y=\""    << v[3]/m_len_unit            << "\""
          << " v3x=\""    << v[4]/m_len_unit            << "\""
          << " v3y=\""    << v[5]/m_len_unit            << "\""
          << " v4x=\""    << v[6]/m_len_unit            << "\""
          << " v4y=\""    << v[7]/m_len_unit            << "\""
          << " v5x=\""    << v[8]/m_len_unit            << "\""
          << " v5y=\""    << v[9]/m_len_unit            << "\""
          << " v6x=\""    << v[10]/m_len_unit           << "\""
          << " v6y=\""    << v[11]/m_len_unit           << "\""
          << " v7x=\""    << v[12]/m_len_unit           << "\""
          << " v7y=\""    << v[13]/m_len_unit           << "\""
          << " v8x=\""    << v[14]/m_len_unit           << "\""
          << " v8y=\""    << v[15]/m_len_unit           << "\""
          << " dz=\""     << sh->GetDz()/m_len_unit     << "\""
          << "/>" << newline;
    }
    else if ( cl == TGeoXtru::Class() )   {
      const TGeoXtru* sh = (const TGeoXtru*) shape;
      log << "<xtru" << nam << ">" << newline;
      for (int i = 0; i < sh->GetNvert(); i++) {
	log << " <twoDimVertex x=\"" << sh->GetX(i)/m_len_unit << "\""
	    << "\" y=\"" << sh->GetY(i)/m_len_unit << "\"/>" << newline;
      }
      for (int i = 0; i < sh->GetNz(); i++) {
	log << " <section zOrder=\"" << i << "\""
	    << " scalingFactor=\"" << sh->GetScale(i) << "\""
	    << " zPosition=\"" << sh->GetZ(i)/m_len_unit << "\""
	    << " xOffset=\""   << sh->GetXOffset(i)/m_len_unit << "\""
	    << " yOffset=\""   << sh->GetYOffset(i)/m_len_unit << "\"/>" << newline;
      }
      log << "</xtru>" << newline;
    }
    else if (shape->IsA() == TGeoCompositeShape::Class() )   {
      const TGeoCompositeShape* sh  = (const TGeoCompositeShape*)shape;
      const TGeoBoolNode* boolean   = sh->GetBoolNode();
      const TGeoShape*    left      = boolean->GetLeftShape();
      const TGeoShape*    right     = boolean->GetRightShape();
      const TGeoMatrix*   mat_left  = boolean->GetLeftMatrix();
      const TGeoMatrix*   mat_right = boolean->GetRightMatrix();
      std::string         str_oper;

      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      if (oper == TGeoBoolNode::kGeoSubtraction)
        str_oper = "subtraction";
      else if (oper == TGeoBoolNode::kGeoUnion)
        str_oper = "union";
      else if (oper == TGeoBoolNode::kGeoIntersection)
        str_oper = "intersection";

      if ( left->IsA() == TGeoScaledShape::Class() && right->IsA() == TGeoBBox::Class() )   {
	const auto* scaled = (TGeoScaledShape*)left;
	const auto* sphere = (TGeoSphere*)scaled->GetShape();
	const auto* box    = (TGeoBBox*)right;
	if ( scaled->IsA() == TGeoSphere::Class() && oper == TGeoBoolNode::kGeoIntersection )   {
	  Double_t sx    = scaled->GetScale()->GetScale()[0];
	  Double_t sy    = scaled->GetScale()->GetScale()[1];
	  Double_t ax    = sx * sphere->GetRmax();
	  Double_t by    = sy * sphere->GetRmax();
	  Double_t cz    = sphere->GetRmax();
	  Double_t dz    = box->GetDZ();
	  Double_t zorig = box->GetOrigin()[2];
	  Double_t zcut2 = dz + zorig;
	  Double_t zcut1 = 2 * zorig - zcut2;
	  log << "<ellipsoid" << nam
	      << " lunit=\""  << m_len_unit_nam     << "\""
	      << " ax=\""     << ax/m_len_unit      << "\""
	      << " by=\""     << by/m_len_unit      << "\""
	      << " cz=\""     << cz/m_len_unit      << "\""
	      << " zcut1=\""  << zcut1/m_len_unit   << "\""
	      << " zcut2=\""  << zcut2/m_len_unit   << "\"/>";
	  iso = geo.emplace(solid, log.str()).first;
	  return iso->second;
	}
      }
      XYZAngles       angles;
      const Double_t *trans;
      std::string     str_left  = handleSolid(Solid(left));
      std::string     str_right = handleSolid(Solid(right));
      trans = mat_left->GetTranslation();
      angles = detail::matrix::_xyzAngles(mat_left);

      log << "<" << str_oper << nam
	  << " lunit=\"" << m_len_unit_nam << "\""
	  << " aunit=\"" << m_ang_unit_nam << "\">" << newline
	  << "<first>"   << newline
	  << str_left    << newline
	  << "</first>"  << newline
	  << "<second>"  << newline
	  << str_right   << newline
	  << "</second>" << newline;

      log << "<firstposition"
	  << " name=\""  << shape->GetName()  << "." << left->GetName() << ".position\"" << newline
	  << " unit=\""  << m_len_unit_nam << "\""
	  << " x=\""     << trans[0]/m_len_unit    << "\""
	  << " y=\""     << trans[1]/m_len_unit    << "\""
	  << " z=\""     << trans[2]/m_len_unit    << "\""
	  << "/>" << newline;
      log << "<firstrotation"
	  << " name=\""  << shape->GetName()  << "." << left->GetName() << ".rotation\"" << newline
	  << " unit=\""  << m_ang_unit_nam << "\""
	  << " x=\""     << angles.X()/m_ang_unit  << "\""
	  << " y=\""     << angles.Y()/m_ang_unit  << "\""
	  << " z=\""     << angles.Z()/m_ang_unit  << "\""
	  << "/>" << newline;
      trans = mat_right->GetTranslation();
      angles = detail::matrix::_xyzAngles(mat_right);
      log << "<position"
	  << " name=\""  << shape->GetName()  << "." << right->GetName() << ".position\"" << newline
	  << " unit=\""  << m_len_unit_nam << "\""
	  << " x=\""     << trans[0]/m_len_unit    << "\""
	  << " y=\""     << trans[1]/m_len_unit    << "\""
	  << " z=\""     << trans[2]/m_len_unit    << "\""
	  << "/>" << newline;
      log << "<rotation"
	  << " name=\""  << shape->GetName()  << "." << right->GetName() << ".rotation\"" << newline
	  << " unit=\""  << m_ang_unit_nam << "\""
	  << " x=\""     << angles.X()/m_ang_unit  << "\""
	  << " y=\""     << angles.Y()/m_ang_unit  << "\""
	  << " z=\""     << angles.Z()/m_ang_unit  << "\""
	  << "/>" << newline;
 
      log << "</" << str_oper << ">" << newline;
    }
    iso = geo.emplace(solid, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return iso->second;
}

/// Convert the Position into the corresponding Xml object(s).
const std::string& DetectorChecksum::handlePosition(const TGeoMatrix* trafo) const {
  auto& geo = data().mapOfPositions;
  auto  ipo = geo.find(trafo);
  if ( ipo == geo.end() )    {
    const double* tr = trafo->GetTranslation();
    std::stringstream log = logger();
    log << "<position"
	<< " unit=\"" << m_len_unit_nam << "\""
	<< " x=\"" << tr[0]/m_len_unit  << "\""
	<< " y=\"" << tr[1]/m_len_unit  << "\""
	<< " z=\"" << tr[2]/m_len_unit  << "\"/>";
    ipo = geo.emplace(trafo, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ipo->second;
}

/// Convert the Rotation into the corresponding Xml object(s).
const std::string& DetectorChecksum::handleRotation(const TGeoMatrix* trafo) const {
  auto& geo = data().mapOfRotations;
  auto  iro = geo.find(trafo);
  if ( iro == geo.end() )    {
    XYZAngles    rot = detail::matrix::_xyzAngles(trafo->GetRotationMatrix());
    std::stringstream log = logger();
    log << "<rotation"
	<< " unit=\"" << m_ang_unit_nam  << "\""
        << " x=\"" << rot.X()/m_ang_unit << "\""
	<< " y=\"" << rot.Y()/m_ang_unit << "\""
	<< " z=\"" << rot.Z()/m_ang_unit << "\"/>";
    iro = geo.emplace(trafo, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return iro->second;
}

/// Convert the geometry visualisation attributes to the corresponding Detector object(s).
const std::string& DetectorChecksum::handleVis(VisAttr attr) const {
  auto& geo = data().mapOfVis;
  auto  ivi = geo.find(attr);
  if ( ivi == geo.end() ) {
    std::stringstream log = logger();
    float red = 0, green = 0, blue = 0;
    int style = attr.lineStyle();
    int draw  = attr.drawingStyle();
    attr.rgb(red, green, blue);

    log << "<vis"
	<< " name=\""           << attr.name()          << "\""
	<< " visible=\""        << attr.visible()       << "\""
	<< " show_daughters=\"" << attr.showDaughters() << "\"";
    if (style == VisAttr::SOLID)
      log << " line_style=\"unbroken\"";
    else if (style == VisAttr::DASHED)
      log << " line_style=\"broken\"";
    if (draw == VisAttr::SOLID)
      log << " line_style=\"solid\"";
    else if (draw == VisAttr::WIREFRAME)
      log << " line_style=\"wireframe\"";
    log << "<color"
	<< " alpha=\"" << attr.alpha()
	<< " R=\""     << red    << "\""
	<< " B=\""     << blue   << "\""
	<< " G=\""     << green  << "\"/>" << newline;
    log << "</vis>";
    ivi = geo.emplace(attr, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ivi->second;
}

/// Convert the geometry type region into the corresponding Detector object(s).
const std::string& DetectorChecksum::handleRegion(Region region) const {
  auto& geo = data().mapOfRegions;
  auto  ire = geo.find(region);
  if ( ire == geo.end() )   {
    std::stringstream log = logger();
    log << "<region name=\""       << region.name() << "\""
	<< " store_secondaries=\"" << (region.storeSecondaries() ? 1 : 0) << "\""
	<< " cut=\""               << region.cut() << "\""
	<< " eunit=\""             << m_ene_unit_nam << "\""
	<< " lunit=\""             << m_len_unit_nam << "\""
	<< "/>";
    ire = geo.emplace(region, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ire->second;
}

/// Convert the geometry type LimitSet into the corresponding Detector object(s)
const std::string& DetectorChecksum::handleLimitSet(LimitSet lim) const {
  auto& geo = data().mapOfLimits;
  auto  ili = geo.find(lim);
  if ( ili == geo.end() )    {
    std::stringstream log = logger();
    const std::set<Limit>& obj = lim.limits();
    log << "<limitset name=\""     << lim.name() << "\">" << newline;
    for (const auto& limit : obj)  {
      log << "<limit name=\""      << limit.name   << "\""
	  << "  unit=\""           << limit.unit   << "\""
	  << "  value=\""          << limit.value  << "\""
	  << " particles=\""       << limit.particles << "\""
	  << "/>" << newline;
    }
    log << "</limitSet>";
    ili = geo.emplace(lim, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ili->second;
}

const std::string& DetectorChecksum::handleAlignment(Alignment alignment)  const  {
  auto& geo = data().mapOfAlignments;
  auto  ial = geo.find(alignment);
  if ( ial == geo.end() ) {
    std::stringstream log = logger();
    const auto& data = alignment.data();
    double x, y, z;
    data.delta.pivot.GetComponents(x, y, z);
    log << "<nominal>" << newline;
    log << "<translation"
	<< " unit=\""  << m_len_unit_nam << "\""
	<< " x=\""     << data.delta.translation.X()/m_len_unit  << "\""
	<< " y=\""     << data.delta.translation.Y()/m_len_unit  << "\""
	<< " z=\""     << data.delta.translation.Z()/m_len_unit  << "\"/>" << newline;
    log << "<pivot"
	<< " unit=\""  << m_len_unit_nam << "\""
   	<< " x=\""     << x / m_len_unit << "\""
	<< " y=\""     << y / m_len_unit << "\""
	<< " z=\""     << z / m_len_unit << "\"/>" << newline;
    log << "<rotation"
	<< " unit=\""  << m_len_unit_nam << "\""
	<< " theta=\"" << data.delta.rotation.Theta()/m_ang_unit  << "\""
	<< " phi=\""   << data.delta.rotation.Phi()/m_len_unit  << "\""
	<< " psi=\""   << data.delta.rotation.Psi()/m_len_unit  << "\"/>" << newline;
    log << "</nominal>";
    ial = geo.emplace(alignment, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ial->second;
}

/// Dump logical volume in GDML format to output stream
void DetectorChecksum::collectVolume(Volume volume) const {
  Volume v(volume);
  if ( is_volume(volume) )     {
    Region            reg = v.region();
    LimitSet          lim = v.limitSet();
    VisAttr           vis = v.visAttributes();
    SensitiveDetector det = v.sensitiveDetector();
    if ( lim.isValid() ) handleLimitSet(lim);
    if ( reg.isValid() ) handleRegion(reg);
    if ( vis.isValid() ) handleVis(vis);
    if ( det.isValid() ) handleSensitive(det);
  }
  else {
    printout(WARNING,"DetectorChecksum","++ CollectVolume: Skip volume: %s",volume.name());
  }
}

/// Dump logical volume in GDML format to output stream
const std::string& DetectorChecksum::handleVolume(Volume volume) const {
  auto& geo = data().mapOfVolumes;
  auto  ivo = geo.find(volume);
  if ( ivo == geo.end() ) {
    const TGeoVolume* v = volume;
    std::string        tag;
    std::string        n   = attr_name(v);
    std::stringstream  log = logger();

    if (v->IsAssembly()) {
      tag = "assembly";
      log << "<" << tag << n;
    }
    else {
      TGeoMedium*   med = v->GetMedium();
      TGeoShape*    sh  = v->GetShape();
      if ( !sh )
        throw std::runtime_error("DetectorChecksum: No Detector Solid present for volume:" + n);
      else if ( !med )
        throw std::runtime_error("DetectorChecksum: No Detector material present for volume:" + n);
      tag = "volume";
      handleSolid(sh);
      handleMaterial(med);
      log << "<" << tag << n
	  << " material=\"" << refName(med) << "\""
	  << " solid=\""    << refName(sh)   << "\"";
    }
    auto reg = volume.region();
    auto lim = volume.limitSet();
    auto vis = volume.visAttributes();
    auto det = volume.sensitiveDetector();
    if ( lim.isValid() )
      log << " limits=\""    << refName(lim) << "\"";
    if ( reg.isValid() )
      log << " region=\""    << refName(reg) << "\"";
    if ( vis.isValid() )
      log << " vis=\""       << refName(vis) << "\"";
    if ( det.isValid() )
      log << " sensitive=\"" << refName(det) << "\"";
    log << ">";
    collectVolume(volume);
    const TObjArray* dau = const_cast<TGeoVolume*>(v)->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
        TGeoNode* node = reinterpret_cast<TGeoNode*>(dau->At(i));
	log << "<physvol name=\"" << refName(node) << "\"/>";
        handlePlacement(node);
      }
    }
    log << "</" << tag << ">";
    ivo = geo.emplace(volume, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ivo->second;
}

/// Dump volume placement in GDML format to output stream
const std::string& DetectorChecksum::handlePlacement(PlacedVolume node) const {
  auto& geo = data().mapOfPlacements;
  auto  ipl = geo.find(node);
  if ( ipl == geo.end() ) {
    TGeoMatrix* matrix = node->GetMatrix();
    TGeoVolume* volume = node->GetVolume();
    std::stringstream  log = logger();

    log << "<physvol" << attr_name(node)
	<< " volumeref=\"" << refName(volume) << "\"";
    log << ">" << newline;
    log << handleVolume(node.volume()) << newline;
    if ( matrix )   {
      log << handlePosition(matrix) << newline;
      if ( matrix->IsRotation() )  {
	log << handleRotation(matrix) << newline;
      }
    }
    if ( node.data() )   {
      const PlacedVolume::VolIDs& ids = node.volIDs();
      for (const auto& vid : ids )
	log << "<physvolid field_name=\"" << vid.first << "\" value=\"" << vid.second << "\"/>" << newline;
    }
    log << "</physvol>";
    ipl = geo.emplace(node, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ipl->second;
}

const std::string& DetectorChecksum::handleDetElement(DetElement det)  const  {
  auto& geo = data().mapOfDetElements;
  auto  dit = geo.find(det);
  if ( dit == geo.end() )   {
    std::stringstream log = logger();
    std::string  par = det.parent().isValid() ? det.parent().path() : std::string("");
    log << "<detelement"
	<< " name=\""          << det.name()             << "\""
	<< " id=\""            << det.id()               << "\""
	<< " type=\""          << det.type()             << "\""
	<< " key=\""           << det.key()              << "\""
	<< " parent=\""        << par                    << "\""
	<< " flag=\""          << det.typeFlag()         << "\""
	<< " combineHits=\""   << det.combineHits()      << "\""
	<< " volume_id=\""     << det.volumeID()         << "\""
	<< " placement=\""     << det.placementPath()    << "\""
	<< "/>";
    dit = geo.emplace(det, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return dit->second;
}

/// Convert the geometry type SensitiveDetector into the corresponding Detector object(s).
const std::string& DetectorChecksum::handleSensitive(SensitiveDetector sd) const {
  if ( sd.isValid() )   {
    auto& geo = data().mapOfSensDets;
    auto  isi = geo.find(sd);
    if ( isi == geo.end() ) {
      std::stringstream log = logger();
      log << "<sensitive_detector"
	  << " name=\""            << refName(sd)                  << "\""
	  << " type=\""            << sd.type()                    << "\""
	  << " ecut=\""            << sd.energyCutoff()/m_ene_unit << "\""
	  << " eunit=\""           << m_ene_unit_nam               << "\""
	  << " hits_collection=\"" << sd.hitsCollection()          << "\""
	  << " combine_hits=\""    << sd.combineHits()             << "\"/>";
      Readout ro = sd.readout();
      if ( ro.isValid() ) {
	handleIdSpec(ro.idSpec());
	handleSegmentation(ro.segmentation());
      }
      isi = geo.emplace(sd, log.str()).first;
      if ( debug > 3 ) std::cout << log.str() << std::endl;
    }
    return isi->second;
  }
  return empty_string;
}

/// Convert the segmentation of a SensitiveDetector into the corresponding Detector object
const std::string& DetectorChecksum::handleSegmentation(Segmentation seg) const {
  if (seg.isValid()) {
    auto& geo = data().mapOfSegmentations;
    auto  ise = geo.find(seg);
    if ( ise == geo.end() ) {
      std::stringstream log = logger();
      const auto& p = seg.parameters();
      log << "<segmentation" << attr_name(seg)
	  << " type=\"" << seg.type() << "\">" << newline;
      log << "<parameters>" << newline;
      for ( const auto& v : p )  {
	log << "<parameter";
	if (v->name() == "lunit")   {
	  log << " name=\"" << v->name() << "\" value=\"" << _toDouble(v->value())/m_len_unit << "\"";
	  continue;
	}
	// translate from TGeo units to Geant4 units if necessary
	if (v->unitType() == DDSegmentation::SegmentationParameter::LengthUnit) {
	  double value = _toDouble(v->value());
	  log << " name=\"" << v->unitType() << "\" value=\"" << value/m_len_unit << "\"";
	}
	else if (v->unitType() == DDSegmentation::SegmentationParameter::AngleUnit) {
	  double value = _toDouble(v->value());
	  log << " name=\"" << v->unitType() << "\" value=\"" << value/m_ang_unit << "\"";
	}
	else {
	  log << " name=\"" << v->name() << "\" value=\"" << v->value() << "\"";
	}
	log << "/>" << newline;
      }
      log << "</parameters>" << newline;
      log << "</segmentatoin>";
      ise = geo.emplace(seg, log.str()).first;
      if ( debug > 3 ) std::cout << log.str() << std::endl;
    }
    return ise->second;
  }
  return empty_string;
}

/// Convert the geometry id dictionary entry to the corresponding Xml object(s).
const std::string& DetectorChecksum::handleIdSpec(IDDescriptor id_spec) const {
  if ( id_spec.isValid() )   {
    auto& geo = data().mapOfIdSpecs;
    auto  iid = geo.find(id_spec);
    if ( iid == geo.end() ) {
      std::stringstream log = logger();
      const IDDescriptor::FieldMap& fm = id_spec.fields();
      log << "<id name=\"" << refName(id_spec) << "\">";
      for (const auto& i : fm )  {
	const BitFieldElement* f = i.second;
	log << "<idfield label=\"" << f->name()       << "\""
	    << " signed=\""        << true_false(f->isSigned()) << "\""
	    << " length=\""        << f->width()      << "\""
	    << " start=\""         << f->offset()     << "\"/>" << newline;
      }
      log << "</id>";
      iid = geo.emplace(id_spec, log.str()).first;
      if ( debug > 3 ) std::cout << log.str() << std::endl;
    }
    return iid->second;
  }
  return empty_string;
}

/// Convert the electric or magnetic fields into the corresponding Xml object(s).
const std::string& DetectorChecksum::handleField(OverlayedField f) const {
  auto& geo = data().mapOfFields;
  auto  ifd = geo.find(f);
  if ( ifd == geo.end() ) {
    std::string type = f->GetTitle();
    std::stringstream log = logger();
    log << "<field name=\"" << f->GetName() << "\" type=\"" << f->GetTitle() << "\">";
#if 0
    field = xml_elt_t(geo.doc, Unicode(type));
    field.setAttr(_U(name), f->GetName());
    fld = PluginService::Create<NamedObject*>(type + "_Convert2Detector", &m_detDesc, &field, &fld);
    printout(ALWAYS,"DetectorChecksum","++ %s electromagnetic field:%s of type %s",
             (fld.isValid() ? "Converted" : "FAILED    to convert "), f->GetName(), type.c_str());
    if (!fld.isValid()) {
      PluginDebug dbg;
      PluginService::Create<NamedObject*>(type + "_Convert2Detector", &m_detDesc, &field, &fld);
      throw std::runtime_error("Failed to locate plugin to convert electromagnetic field:"
			       + std::string(f->GetName()) + " of type " + type + ". "
			       + dbg.missingFactory(type));
    }
#endif
    log << "</field>";
    ifd = geo.emplace(f, log.str()).first;
    if ( debug > 3 ) std::cout << log.str() << std::endl;
  }
  return ifd->second;
}

/// Add header information in Detector format
const std::string& DetectorChecksum::handleHeader() const {
  GeometryInfo& geo = data();
  Header hdr = m_detDesc.header();
  if ( hdr.isValid() && geo.header.empty() )  {
    std::stringstream log = logger();
    log << "<header name=\"" << hdr.name() << "\">"
	<< "<autor name=\""  << hdr.author() << "\"/>"
	<< "<generator version=\"" << hdr.version() << "\"" << " url=\"" << hdr.url() << "\"/>"
	<< "<comment>" << hdr.comment() << "</comment>"
	<< "</header>";
    geo.header = log.str();
    return geo.header;
  }
  printout(WARNING,"DetectorChecksum","+++ No Detector header information availible from the geometry description.");
  return empty_string;
}

template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
    (o->*pmf)(*i);
  }
}

void DetectorChecksum::collect_det_elements(DetElement top)  const  {
  auto& geo = data().mapOfDetElements;
  auto it = geo.find(top);
  if ( it == geo.end() )    {
    std::string str = handleDetElement(top);
    geo.emplace(top, std::move(str));
    handlePlacement(top.placement());
    for( const auto& c : top.children() )   {
      collect_det_elements(c.second);
    }
  }
}

/// Create geometry conversion
void DetectorChecksum::analyzeDetector(DetElement top)      {
  Detector& description = m_detDesc;
  if (!top.isValid()) {
    throw std::runtime_error("Attempt to call analyzeDetector with an invalid geometry!");
  }
  GeometryInfo& geo = *(m_dataPtr = new GeometryInfo);
  m_data->clear();
  handleHeader();
  collect_det_elements(top);
  for (const auto& fld : description.fields() )
    handleField(fld.second);
  if ( debug > 1 )  {
    printout(ALWAYS,"DetectorChecksum","++ ==> Computing checksum for tree: %s", top.path().c_str());
    printout(ALWAYS,"DetectorChecksum","++ Handled %ld materials.", geo.mapOfMaterials.size());
    printout(ALWAYS,"DetectorChecksum","++ Handled %ld solids.", geo.mapOfSolids.size());
    printout(ALWAYS,"DetectorChecksum","++ Handled %ld volumes.", geo.mapOfVolumes.size());
    printout(ALWAYS,"DetectorChecksum","++ Handled %ld visualization attributes.", geo.mapOfVis.size());
    printout(ALWAYS,"DetectorChecksum","++ Handled %ld fields.", geo.mapOfFields.size());
  }
}

std::vector<PlacedVolume> _get_path(PlacedVolume node, const std::set<PlacedVolume>& match)   {
  if ( match.end() == match.find(node) )   {
    const TObjArray* dau = node.volume()->GetNodes();
    for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
      TGeoNode* n = reinterpret_cast<TGeoNode*>(dau->At(i));
      auto cont = _get_path(n, match);
      if ( !cont.empty() )   {
	cont.emplace_back(node);
	return cont;
      }
    }
    return { };
  }
  return { node };
}

DetectorChecksum::checksum_t
DetectorChecksum::checksumDetElement(int lvl, DetElement top, unsigned long long int hash, bool recursive)  const  {
  auto& dat = data();
  auto& geo = dat.mapOfDetElements;
  auto it = geo.find(top);
  if ( it != geo.end() )    {
    std::set<PlacedVolume> child_places;
    std::set<PlacedVolume> hashed_places;
    auto det_pv = top.placement();

    /// Hash DetElement and placement
    auto de_hash = detail::update_hash64(hash, it->second.c_str());
    if ( hash_readout )   {
      SensitiveDetector sd = m_detDesc.sensitiveDetector(top.name());
      if ( sd.isValid() )   {
	Readout ro = sd.readout();
	de_hash = detail::update_hash64(de_hash, handleSensitive(sd).c_str());
	if ( ro.isValid() ) {
	  de_hash = detail::update_hash64(de_hash, handleIdSpec(ro.idSpec()).c_str());
	  de_hash = detail::update_hash64(de_hash, handleSegmentation(ro.segmentation()).c_str());
	}
      }
    }
    auto pv_hash = checksumPlacement(det_pv, de_hash, false);

    for ( const auto& c : top.children() )
      child_places.emplace(c.second.placement());

    /// Hash all daughters with a DetElement child (but only excluding child)
    /// Note: We only take into account the placements to the next DetElement (if any)
    /// On the fly we remember all placements already taken into account!
    auto dau_hash = pv_hash;
    for( auto pv : child_places )   {
      auto chain = _get_path(pv, child_places);
      for( std::size_t i=0; i < chain.size()-1; ++i )   {
	auto entry = chain[i];
	dau_hash = checksumPlacement(entry, dau_hash, false);
	hashed_places.insert(entry);
      }
    }
    /// Now hash all daughter volumes, which are not linked to a DetElement
    /// in the structural hierarchy.
    const TObjArray* dau = det_pv.volume()->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
	PlacedVolume pv = reinterpret_cast<TGeoNode*>(dau->At(i));
	if ( hashed_places.find(pv) == hashed_places.end() )  {
	  dau_hash = checksumPlacement(pv, dau_hash, true);
	}
      }
    }

    /// Finally: Hash recursively the structural children
    auto child_hash = dau_hash;
    if ( recursive )   {
      for ( const auto& c : top.children() )
	child_hash = checksumDetElement(lvl+1, c.second, child_hash, recursive);
    }

    /// All done: Some debugging printout
    if ( debug > 0 || lvl <= max_level )  {
      std::stringstream str;
      str << "+++ " << std::setw(4) << std::left << lvl
	  << " " << std::setw(24) << std::left << top.name()
	  << " " << std::setfill('0') << std::setw(16) << std::hex << de_hash;
      str << " " << std::setfill(' ') << std::setw(7) << std::left << "+place"
	  << " " << std::setfill('0') << std::setw(16) << std::hex << pv_hash;
      if ( !(child_places.empty() && hashed_places.empty()) )
	str << " " << std::setfill(' ') << std::setw(10) << std::left << "+daughters"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << dau_hash;
      if ( !top.children().empty() )
	str << " " << std::setfill(' ') << std::setw(9) << std::left << "+children"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << child_hash;
      std::cout << str.str() << std::endl;
    }
    return child_hash;
  }
  except("DetectorChecksum","ERROR: Cannot checksum invalid DetElement");
  return hash;
}

DetectorChecksum::checksum_t
DetectorChecksum::checksumPlacement(PlacedVolume pv, unsigned long long int hash, bool recursive)  const  {
  auto& geo = data().mapOfPlacements;
  auto it = geo.find(pv);
  if ( it != geo.end() )    {
    hash = detail::update_hash64(hash, it->second.c_str());
    if ( recursive )   {
      const TObjArray* dau = pv.volume()->GetNodes();
      if (dau && dau->GetEntries() > 0)   {
	for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
	  PlacedVolume node = reinterpret_cast<TGeoNode*>(dau->At(i));
	  hash = checksumPlacement(node, hash, recursive);
	}
      }
    }
    return hash;
  }
  except("DetectorChecksum","ERROR: Cannot checksum invalid PlacedVolume");
  return hash;
}

static long create_checksum(Detector& description, int argc, char** argv) {
  std::vector<std::string> detectors;
  int precision = 6, newline = 0, level = 1, readout = 0, debug = 0;
  std::string len_unit, ang_unit, ene_unit, dens_unit;
  for(int i = 0; i < argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-detector",argv[i],4) && (i+1)<argc )
      detectors.emplace_back(argv[++i]);
    else if ( 0 == ::strncmp("-precision",argv[i],4) && (i+1)<argc )
      precision = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-length_unit",argv[i],10) && (i+1)<argc )
      len_unit = argv[++i];
    else if ( 0 == ::strncmp("-angle_unit",argv[i],10) && (i+1)<argc )
      ang_unit = argv[++i];
    else if ( 0 == ::strncmp("-energy_unit",argv[i],10) && (i+1)<argc )
      ene_unit = argv[++i];
    else if ( 0 == ::strncmp("-density_unit",argv[i],10) && (i+1)<argc )
      dens_unit = argv[++i];
    else if ( 0 == ::strncmp("-level", argv[i],5) && (i+1)<argc )
      level = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-debug", argv[i],5) && (i+1)<argc )
      debug = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-newline",argv[i],5) )
      newline = 1;
    else if ( 0 == ::strncmp("-readout",argv[i],5) )
      readout = 1;
    else  {
      std::cout <<
        "Usage: -plugin DD4hep_GeometryDisplay  -arg [-arg]                      \n\n"
        "     -detector <string>     Top level DetElement path. Default: '/world'  \n"
	"     -readout               also hash the detector's readout properties   \n"
	"                            (sensitive det, id desc, segmentation)        \n"
	"                            default: false                                \n"
	"     -length_unit  <value>  Unit of length  as literal. default: mm       \n"
	"     -angle_unit   <value>  Unit of angle   as literal. default: deg      \n"
	"     -energy_unit  <value>  Unit of energy  as literal. default: GeV      \n"
	"     -density_unit <value>  Unit of density as literal. default: g/cm3    \n"
	"     -debug <number>        Enable debug printouts.                       \n"
        "     -help                  Print this help output                        \n"
        "     Arguments given: " << arguments(argc,argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
  }
  DetectorChecksum wr(description);
  wr.precision = precision;
  if ( !len_unit.empty()  ) wr.m_len_unit_nam = len_unit;
  if ( !ang_unit.empty()  ) wr.m_ang_unit_nam = ang_unit;
  if ( !ene_unit.empty()  ) wr.m_ene_unit_nam = ene_unit;
  if ( !dens_unit.empty() ) wr.m_densunit_nam = dens_unit;
  if ( newline ) wr.newline = "\n";
  wr.hash_readout = readout;
  wr.max_level = level;
  wr.debug = debug;
  wr.configure();

  if ( !detectors.empty() )  {
    for (const auto& det : detectors )   {
      DetElement de = detail::tools::findElement(description,det);
      auto hash = detail::hash64("DD4hepGeometryChecksum");
      wr.analyzeDetector(de);
      wr.checksumDetElement(0, de, hash, true);
    }
    return 1;
  }
  auto hash = detail::hash64("DD4hepGeometryChecksum");
  wr.analyzeDetector(description.world());
  wr.checksumDetElement(0, description.world(), hash, true);
  return 1;
}

DECLARE_APPLY(DD4hepGeometryChecksum, create_checksum)
