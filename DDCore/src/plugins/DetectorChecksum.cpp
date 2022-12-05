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
#include <TGeoSystemOfUnits.h>
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
  inline double check_null(double d)   {
    if ( abs(d) < 1e-12 )
      return 0e0;
    else
      return d;
  }
  template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf) {
    for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
      (o->*pmf)(*i);
    }
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
  std::string nam = handle->GetName();
  std::size_t idx = nam.find("_0x");
  return idx == std::string::npos ? nam : nam.substr(0, idx);
}
template <> std::string DetectorChecksum::refName(Segmentation handle)  const  {
  std::string nam = handle->name();
  std::size_t idx = nam.find("_0x");
  return idx == std::string::npos ? nam : nam.substr(0, idx);
}
template <typename T> std::string DetectorChecksum::attr_name(T handle)  const  {
  std::string n = " name=\"" + refName(handle)+"\"";
  return n;
}

std::stringstream DetectorChecksum::logger()   const    {
  std::stringstream log;
  log.setf(std::ios::fixed, std::ios::floatfield);
  log << std::setprecision(precision);
  return log;
}

DetectorChecksum::entry_t DetectorChecksum::make_entry(std::stringstream& log)   const {
  std::string data(std::move(log.str()));
  hash_t hash_value = hash64(data.c_str(), data.length());
  return { hash_value, std::move(data) };
}

void DetectorChecksum::configure()   {
  m_len_unit = _toDouble(m_len_unit_nam);
  m_ang_unit = _toDouble(m_ang_unit_nam)/_toDouble("deg/rad");
  m_ene_unit = _toDouble(m_ene_unit_nam);
  m_densunit = _toDouble(m_densunit_nam)/_toDouble("g/cm3");  // g/cm3  is always internal unit of TGeo
  m_atomunit = _toDouble(m_atomunit_nam)/_toDouble("g/mole"); // g/mole is always internal unit of TGeo
  if ( debug > 1 )   {
    printout(INFO,"DetectorChecksum","+++ Float precision: %d", precision);
    printout(INFO,"DetectorChecksum","+++ Unit of length:  %-12s -> conversion factor: %f", m_len_unit_nam.c_str(), m_len_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of angle:   %-12s -> conversion factor: %f", m_ang_unit_nam.c_str(), m_ang_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of energy:  %-12s -> conversion factor: %f", m_ene_unit_nam.c_str(), m_ene_unit);
    printout(INFO,"DetectorChecksum","+++ Unit of density: %-12s -> conversion factor: %f", m_densunit_nam.c_str(), m_densunit);
    printout(INFO,"DetectorChecksum","+++ Unit of density: %-12s -> conversion factor: %f", m_atomunit_nam.c_str(), m_atomunit);
  }
}

/// Dump element in GDML format to output stream
const DetectorChecksum::entry_t& DetectorChecksum::handleElement(Atom element) const {
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
	  << " <atom unit=\""   << m_atomunit_nam 
	  << "\" value=\""      << std::scientific << element->A()/m_atomunit << "\"/>" << newline
	  << "</element>";
      iel = geo.emplace(element, make_entry(log) ).first;
    }
  }
  return iel->second;
}

/// Dump material in GDML format to output stream
const DetectorChecksum::entry_t& DetectorChecksum::handleMaterial(Material medium) const {
  auto& geo = data().mapOfMaterials;
  auto  ima = geo.find(medium);
  if ( ima == geo.end() )   {
    std::stringstream log = logger();
    auto* mat = medium->GetMaterial();
    log << "<material"   << attr_name(medium) << "\"/>" << newline;
    if ( mat->IsMixture() )   {
      std::map<std::string, double>  elts;
      TGeoMixture* mix = (TGeoMixture*)mat;
      int count_elts = mix->GetNelements();
      for ( Int_t idx = 0; idx < count_elts; idx++ )
	elts[refName(mix->GetElement(idx))] = mix->GetWmixt()[idx];
      log << " <elements count=\""<< count_elts << "\">" << newline;
      for( const auto& w : elts )
	log << " <fraction ref=\"" << w.first << "\" n=\"" << w.second << "\">" << newline;
      log << " </elements>" << newline;
    }
    else   {
      log << " A=\""          << mat->GetA()                  << "\""
	  << " Z=\""          << mat->GetZ()                  << "\">" << newline
	  << " <element=\""   << refName(mat->GetElement())   << "\"/>" << newline
	  << " <atom unit=\"" << m_atomunit_nam << "\" value=\"" << mat->GetA()/m_atomunit << "\"/>" << newline;
    }
    log << " <D unit=\"" << m_densunit_nam << "\" value=\"" << mat->GetDensity()/m_densunit << "\"/>" << newline;
    log << "</material>";
    ima = geo.emplace(medium, make_entry(log)).first;
  }
  return ima->second;
}

/// Dump solid in GDML format to output stream
const DetectorChecksum::entry_t& DetectorChecksum::handleSolid(Solid solid) const {
  auto& geo = data().mapOfSolids;
  auto  iso = geo.find(solid);
  if ( iso == geo.end() )   {
    const TGeoShape* shape = solid.ptr();
    auto  log = logger();

    if ( strncmp(solid.name(), "LumiCal_envelope_shape_0x", strlen("LumiCal_envelope_shape_0x"))==0 )    {
      printout(INFO, "DetectorChecksum","---> %s", solid.name());
    }

    if ( !shape )  {
      log << "<shape type=\"INVALID\"></shape>)";
      iso = geo.emplace(solid, make_entry(log)).first;
      return iso->second;
    }

    // NOTE: We cannot use the name when creating the hash!
    // The name is artificially assigned and contains the shape pointer.
    // This causes havoc.
    TClass*     cl  = shape->IsA();
    std::string nam = "";//attr_name(solid);
    if ( cl == TGeoBBox::Class() )   {
      TGeoBBox* sh = (TGeoBBox*) shape;
      log << "<box" << nam
	  << " lunit=\"" << m_len_unit_nam << "\""
	  << " x=\"" << 2.0*sh->GetDX()/m_len_unit << "\""
	  << " y=\"" << 2.0*sh->GetDY()/m_len_unit << "\""
	  << " z=\"" << 2.0*sh->GetDZ()/m_len_unit << "\""
	  << "/>";
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
	  << "</halfspace>";
    }
    else if ( cl == TGeoTube::Class() || cl == TGeoTubeSeg::Class() ) {
      const TGeoTube* sh = (const TGeoTube*) shape;
      log << "<tube" << nam
	  << " lunit=\""    << m_len_unit_nam           << "\""
	  << " aunit=\""    << m_ang_unit_nam           << "\""
	  << " rmin=\""     << check_null(sh->GetRmin()/m_len_unit) << "\""
	  << " rmax=\""     << check_null(sh->GetRmax()/m_len_unit) << "\""
	  << " dz=\""       << check_null(2*sh->GetDz()/m_len_unit) << "\""
	  << " startphi=\"" << 0.0                 << "\""
	  << " deltaphi=\"" << 360.0/m_ang_unit         << "\""
	  << "/>";
    }
    else if ( cl == TGeoTubeSeg::Class() ) {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      log << "<tube" << nam
	  << " lunit=\""    << m_len_unit_nam             << "\""
	  << " aunit=\""    << m_ang_unit_nam             << "\""
	  << " rmin=\""     << check_null(sh->GetRmin()/m_len_unit) << "\""
	  << " rmax=\""     << check_null(sh->GetRmax()/m_len_unit) << "\""
	  << " dz=\""       << check_null(2*sh->GetDz()/m_len_unit) << "\""
	  << " startphi=\"" << sh->GetPhi1()/m_ang_unit   << "\""
	  << " deltaphi=\"" << (sh->GetPhi2() - sh->GetPhi1())/m_ang_unit << "\""
	  << "/>";
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
	  << "/>";
    }
    else if ( cl == TGeoEltu::Class() ) {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      log << "<eltube" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " dx=\""     << sh->GetA()/m_len_unit      << "\""
	  << " dy=\""     << sh->GetB()/m_len_unit      << "\""
	  << " dz=\""     << sh->GetDz()/m_len_unit     << "\""
	  << "/>";
    }
    else if ( cl == TGeoTrd1::Class() ) {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      log << "<trd" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " x1=\""     << 2*check_null(sh->GetDx1()/m_len_unit)  << "\""
	  << " x2=\""     << 2*check_null(sh->GetDx2()/m_len_unit)  << "\""
	  << " y1=\""     << 2*check_null(sh->GetDy()/m_len_unit)   << "\""
	  << " y2=\""     << 2*check_null(sh->GetDy()/m_len_unit)   << "\""
	  << " z=\""      << 2*check_null(sh->GetDz()/m_len_unit)   << "\""
	  << "/>";
    }
    else if ( cl == TGeoTrd2::Class() ) {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      log << "<trd" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " x1=\""     << 2*check_null(sh->GetDx1()/m_len_unit)  << "\""
	  << " x2=\""     << 2*check_null(sh->GetDx2()/m_len_unit)  << "\""
	  << " y1=\""     << 2*check_null(sh->GetDy1()/m_len_unit)  << "\""
	  << " y2=\""     << 2*check_null(sh->GetDy2()/m_len_unit)  << "\""
	  << " z=\""      << 2*check_null(sh->GetDz()/m_len_unit)   << "\""
	  << "/>";
    }
    else if ( cl == TGeoTrap::Class() )   {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      log << "<trap" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " z=\""      << check_null(2*sh->GetDz()/m_len_unit)   << "\""
	  << " theta=\""  << check_null(sh->GetTheta()/m_ang_unit)  << "\""
	  << " phi=\""    << check_null(sh->GetPhi()/m_ang_unit)    << "\""
	  << " x1=\""     << check_null(2*sh->GetBl1()/m_len_unit)  << "\""
	  << " x2=\""     << check_null(2*sh->GetTl1()/m_len_unit)  << "\""
	  << " x3=\""     << check_null(2*sh->GetBl2()/m_len_unit)  << "\""
	  << " x4=\""     << check_null(2*sh->GetTl2()/m_len_unit)  << "\""
	  << " y1=\""     << check_null(2*sh->GetH1()/m_len_unit)   << "\""
	  << " y2=\""     << check_null(2*sh->GetH2()/m_len_unit)   << "\""
	  << " alpha1=\"" << check_null(sh->GetAlpha1()/m_ang_unit) << "\""
	  << " alpha2=\"" << check_null(sh->GetAlpha2()/m_ang_unit) << "\""
	  << "/>";
    }
    else if ( cl == TGeoHype::Class() )   {
      const TGeoHype* sh = (const TGeoHype*) shape;
      log << "<hype" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin=\""   << check_null(sh->GetRmin()/m_len_unit)   << "\""
	  << " rmax=\""   << check_null(sh->GetRmax()/m_len_unit)   << "\""
	  << " inst=\""   << check_null(sh->GetStIn()/m_ang_unit)   << "\""
	  << " outst=\""  << check_null(sh->GetStOut()/m_ang_unit)  << "\""
	  << " z=\""      << check_null(2*sh->GetDz()/m_len_unit)   << "\""
	  << "/>";
    }
    else if ( cl == TGeoPgon::Class() )   {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      log << "<polyhedra" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " startphi=\"" << check_null(sh->GetPhi1()/m_ang_unit) << "\""
	  << " deltaphi=\"" << check_null(sh->GetDphi()/m_ang_unit) << "\""
	  << " numsides=\"" << sh->GetNedges()     << "\">" << newline;
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        log << " <zplane z=\"" << check_null(sh->GetZ(i)/m_len_unit) 
	    << "\" rmin=\"" << check_null(sh->GetRmin(i)/m_len_unit) << "\""
	    << "\" rmax=\"" << check_null(sh->GetRmax(i)/m_len_unit) << "\"/>" << newline;
      }
      log << "</polyhedra>";
    }
    else if ( cl == TGeoPcon::Class() )  {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      log << "<polycone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " startphi=\"" << check_null(sh->GetPhi1()/m_ang_unit) << "\""
	  << " deltaphi=\"" << check_null(sh->GetDphi()/m_ang_unit) << "\">" << newline;
      for(int i=0, n=sh->GetNz(); i<n; ++i)  {
        log << " <zplane z=\"" << check_null(sh->GetZ(i)/m_len_unit) 
	    << "\" rmin=\"" << check_null(sh->GetRmin(i)/m_len_unit) << "\""
	    << "\" rmax=\"" << check_null(sh->GetRmax(i)/m_len_unit) << "\"/>" << newline;
      }
      log << "</polycone>";
    }
    else if ( cl == TGeoCone::Class() )  {
      const TGeoCone* sh = (const TGeoCone*) shape;
      log << "<cone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin1=\""  << check_null(sh->GetRmin1()/m_len_unit)  << "\""
	  << " rmin2=\""  << check_null(sh->GetRmin2()/m_len_unit)  << "\""
	  << " rmax1=\""  << check_null(sh->GetRmax1()/m_len_unit)  << "\""
	  << " rmax2=\""  << check_null(sh->GetRmax2()/m_len_unit)  << "\""
	  << " z=\""      << check_null(sh->GetDz()/m_len_unit)     << "\""
	  << " startphi=\"" << 0.0/m_ang_unit           << "\""
	  << " deltaphi=\"" << 360.0/m_ang_unit         << "\""
	  << "/>";
    }
    else if ( cl == TGeoConeSeg::Class() )  {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      log << "<cone" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " aunit=\""  << m_ang_unit_nam             << "\""
	  << " rmin1=\""  << check_null(sh->GetRmin1()/m_len_unit)  << "\""
	  << " rmin2=\""  << check_null(sh->GetRmin2()/m_len_unit)  << "\""
	  << " rmax1=\""  << check_null(sh->GetRmax1()/m_len_unit)  << "\""
	  << " rmax2=\""  << check_null(sh->GetRmax2()/m_len_unit)  << "\""
	  << " z=\""      << check_null(sh->GetDz()/m_len_unit)     << "\""
	  << " startphi=\"" << check_null(sh->GetPhi1()/m_ang_unit) << "\""
	  << " deltaphi=\"" << check_null((sh->GetPhi1()-sh->GetPhi1())/m_ang_unit) << "\""
	  << "/>";
    }
    else if ( cl == TGeoParaboloid::Class() )  {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      log << "<paraboloid" << nam
	  << " lunit=\""  << m_len_unit_nam             << "\""
	  << " rlo=\""    << sh->GetRlo()/m_len_unit    << "\""
	  << " rhi=\""    << sh->GetRhi()/m_len_unit    << "\""
	  << " z=\""      << sh->GetDz()/m_len_unit     << "\""
	  << "/>";
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
	  << "/>";
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
	  << "/>";
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
          << "/>";
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
      log << "</xtru>";
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
	  iso = geo.emplace(solid, make_entry(log)).first;
	  return iso->second;
	}
      }
      // The name cannot be used. We hence use the full hash code
      // for the left and right side shapes!
      const entry_t&  ent_left  = handleSolid(Solid(left));
      const entry_t&  ent_right = handleSolid(Solid(right));
      log << "<" << str_oper << nam
	  << " lunit=\"" << m_len_unit_nam << "\""
	  << " aunit=\"" << m_ang_unit_nam << "\">" << newline
	  << " <first ref=\"" << (void*)ent_left.hash << "\""  << ">" << newline
	  << "  " << handlePosition(mat_left).data << newline
	  << "  " << handleRotation(mat_left).data << newline
	  << " </first>" << newline
	  << " <second ref=\"" << (void*)ent_right.hash << "\"" << ">" << newline
	  << "  " << handlePosition(mat_right).data << newline
	  << "  " << handleRotation(mat_right).data << newline
	  << " </second>" << newline
	  << "</" << str_oper << ">";
    }
    else if ( shape->IsA() == TGeoShapeAssembly::Class() )   {
      log << "<shape_assembly " << nam << "\"/>";
    }
    else   {
      except("DetectorChecksum","+++ Unknown shape: %s", solid.name());
    }
    auto ins = geo.emplace(solid, make_entry(log));
    if ( !ins.second )   {
      except("DetectorChecksum", "+++ FAILED to register shape: %s", solid.name());
    }
    iso = ins.first;
  }
  return iso->second;
}

/// Convert the Position into the corresponding Xml object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handlePosition(const TGeoMatrix* trafo) const {
  auto& geo = data().mapOfPositions;
  auto  ipo = geo.find(trafo);
  if ( ipo == geo.end() )    {
    const double* tr = trafo->GetTranslation();
    std::stringstream log = logger();
    log << "<position"
	<< " unit=\"" << m_len_unit_nam << "\""
	<< " x=\"" << check_null(tr[0]/m_len_unit)  << "\""
	<< " y=\"" << check_null(tr[1]/m_len_unit)  << "\""
	<< " z=\"" << check_null(tr[2]/m_len_unit)  << "\"";
    log << "/>";
    ipo = geo.emplace(trafo, make_entry(log)).first;
  }
  return ipo->second;
}

/// Convert the Rotation into the corresponding Xml object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleRotation(const TGeoMatrix* trafo) const {
  auto& geo = data().mapOfRotations;
  auto  iro = geo.find(trafo);
  if ( iro == geo.end() )    {
    XYZAngles    rot = detail::matrix::_xyzAngles(trafo->GetRotationMatrix());
    std::stringstream log = logger();
    log << "<rotation"
	<< " unit=\"" << m_ang_unit_nam  << "\""
        << " x=\"" << check_null(rot.X()/m_ang_unit) << "\""
	<< " y=\"" << check_null(rot.Y()/m_ang_unit) << "\""
	<< " z=\"" << check_null(rot.Z()/m_ang_unit) << "\""
	<< "/>";
    iro = geo.emplace(trafo, make_entry(log)).first;
  }
  return iro->second;
}

/// Convert the geometry visualisation attributes to the corresponding Detector object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleVis(VisAttr attr) const {
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
    ivi = geo.emplace(attr, make_entry(log)).first;
  }
  return ivi->second;
}

/// Convert the geometry type region into the corresponding Detector object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleRegion(Region region) const {
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
    ire = geo.emplace(region, make_entry(log)).first;
  }
  return ire->second;
}

/// Convert the geometry type LimitSet into the corresponding Detector object(s)
const DetectorChecksum::entry_t& DetectorChecksum::handleLimitSet(LimitSet lim) const {
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
    ili = geo.emplace(lim, make_entry(log)).first;
  }
  return ili->second;
}

const DetectorChecksum::entry_t& DetectorChecksum::handleAlignment(Alignment alignment)  const  {
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
    ial = geo.emplace(alignment, make_entry(log)).first;
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
const DetectorChecksum::entry_t& DetectorChecksum::handleVolume(Volume volume) const {
  auto& geo = data().mapOfVolumes;
  auto  ivo = geo.find(volume);
  if ( ivo == geo.end() ) {
    const TGeoVolume* v = volume;
    std::string       tag;
    std::string       sol;
    std::string       nam = attr_name(v);
    std::stringstream log = logger();
    TGeoShape*  sh  = v->GetShape();
    if ( !sh )
      throw std::runtime_error("DetectorChecksum: No solid present for volume:" + nam);

    if (v->IsAssembly()) {
      const auto& solid_ent = handleSolid(sh);
      tag = "assembly";
      log << "<" << tag << nam
	  << " solid=\""         << refName(sh)           << "\""
	  << " solid_hash=\""    << (void*)solid_ent.hash << "\"";
    }
    else {
      TGeoMedium*   med = v->GetMedium();
      if ( !med )
        throw std::runtime_error("DetectorChecksum: No material present for volume:" + nam);
      const auto& solid_ent = handleSolid(sh);
      tag = "volume";
      log << "<" << tag << nam
	  << " material=\""      << refName(med)          << "\""
	  << " solid=\""         << refName(sh)           << "\""
	  << " solid_hash=\""    << (void*)solid_ent.hash << "\"";
    }
    collectVolume(volume);
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
    const TObjArray* dau = const_cast<TGeoVolume*>(v)->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      log << ">" << newline;
      for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
        TGeoNode* node = reinterpret_cast<TGeoNode*>(dau->At(i));
        const auto& ent = handlePlacement(node);
	log << " <physvol name=\"" << refName(node) << " hash=\"" << (void*)ent.hash << "\"/>" << newline;
      }
      log << "</" << tag << ">";
    }
    else   {
      log << "/>";
    }
    auto ins = geo.emplace(volume, make_entry(log));
    if ( !ins.second )   {
      except("DetectorChecksum", "+++ FAILED to register volume: %s", volume.name());
    }
    ivo = ins.first;
  }
  return ivo->second;
}

/// Dump volume placement in GDML format to output stream
const DetectorChecksum::entry_t& DetectorChecksum::handlePlacement(PlacedVolume node) const {
  auto& geo = data().mapOfPlacements;
  auto  ipl = geo.find(node);
  if ( ipl == geo.end() ) {
    TGeoMatrix* matrix = node->GetMatrix();
    TGeoVolume* volume = node->GetVolume();
    const auto& vol_ent = handleVolume(volume);
    std::stringstream  log = logger();
    log << "<physvol" << attr_name(node)
	<< " volume=\"" << refName(volume) << "\"";
    log << " volume_hash=\"" << (void*)vol_ent.hash << "\"";
    log << ">" << newline;
    if ( matrix )   {
      log << " " << handlePosition(matrix).data << newline;
      if ( matrix->IsRotation() )  {
	log << " " << handleRotation(matrix).data << newline;
      }
    }
    if ( node.data() )   {
      const auto& ids = node.volIDs();
      for (const auto& vid : ids )
	log << " <physvolid"
	    << " name=\""  << vid.first  << "\""
	    << " value=\"" << vid.second << "\""
	    << "/>" << newline;
    }
    log << "</physvol>";
    ipl = geo.emplace(node, make_entry(log)).first;
  }
  return ipl->second;
}

const DetectorChecksum::entry_t& DetectorChecksum::handleDetElement(DetElement det)  const  {
  auto& geo = data().mapOfDetElements;
  auto  dit = geo.find(det);
  if ( dit == geo.end() )   {
    std::stringstream log = logger();
    const auto& place = handlePlacement(det.placement());
    const auto& par = det.parent().isValid() ? handleDetElement(det.parent()) : empty_entry;
    log << "<detelement"
	<< " name=\""          << det.name()        << "\""
	<< " id=\""            << det.id()          << "\""
	<< " type=\""          << det.type()        << "\""
	<< " key=\""           << det.key()         << "\""
	<< " parent=\""        << (void*)par.hash   << "\""
	<< " flag=\""          << det.typeFlag()    << "\""
	<< " combineHits=\""   << det.combineHits() << "\""
	<< " placement=\""     << (void*)place.hash << "\""
	<< "/>";
    dit = geo.emplace(det, make_entry(log)).first;
  }
  return dit->second;
}

/// Convert the geometry type SensitiveDetector into the corresponding Detector object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleSensitive(SensitiveDetector sd) const {
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
	  << " combine_hits=\""    << sd.combineHits()             << "\"";
      Readout ro = sd.readout();
      if ( ro.isValid() ) {
	const auto& ro_ent = handleIdSpec(ro.idSpec());
	log << " iddescriptor=\"" << (void*)ro_ent.hash            << "\"";
	const auto& seg_ent = handleSegmentation(ro.segmentation());
	log << " segmentation=\"" << (void*)seg_ent.hash           << "\"";
      }
      log << "/>";
      isi = geo.emplace(sd, make_entry(log)).first;
    }
    return isi->second;
  }
  return empty_entry;
}

/// Convert the segmentation of a SensitiveDetector into the corresponding Detector object
const DetectorChecksum::entry_t& DetectorChecksum::handleSegmentation(Segmentation seg) const {
  if (seg.isValid()) {
    auto& geo = data().mapOfSegmentations;
    auto  ise = geo.find(seg);
    if ( ise == geo.end() ) {
      using param_t = DDSegmentation::SegmentationParameter;
      std::stringstream log = logger();
      const auto& p = seg.parameters();
      log << "<segmentation" << attr_name(seg)
	  << " type=\"" << seg.type() << "\">" << newline;
      log << " <parameters>" << newline;
      for ( const auto& v : p )  {
	log << "  <parameter";
	log << " name=\"" << v->name() << "\""
	    << " type=\"" << v->type() << "\"";
	if ( v->unitType() == param_t::LengthUnit )
	  log << " value=\"" << _toDouble(v->value())/m_len_unit << "\""
	      << " unit=\""  << m_len_unit_nam << "\"";
	else if ( v->unitType() == param_t::AngleUnit )
	  log << " value=\"" << _toDouble(v->value())/m_ang_unit << "\""
	      << " unit=\""  << m_ang_unit_nam << "\"";
	else
	  log << " value=\"" << v->value() << "\"";
	log << "/>" << newline;
      }
      log << " </parameters>" << newline;
      log << "</segmentatoin>";
      ise = geo.emplace(seg, make_entry(log)).first;
    }
    return ise->second;
  }
  return empty_entry;
}

/// Convert the geometry id dictionary entry to the corresponding Xml object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleIdSpec(IDDescriptor id_spec) const {
  if ( id_spec.isValid() )   {
    auto& geo = data().mapOfIdSpecs;
    auto  iid = geo.find(id_spec);
    if ( iid == geo.end() ) {
      const IDDescriptor::FieldMap& fm = id_spec.fields();
      std::stringstream log = logger();
      log << "<id name=\"" << refName(id_spec) << "\">" << newline;
      for (const auto& i : fm )  {
	const BitFieldElement* f = i.second;
	log << " <idfield label=\"" << f->name()       << "\""
	    << " signed=\""         << true_false(f->isSigned()) << "\""
	    << " length=\""         << f->width()      << "\""
	    << " start=\""          << f->offset()     << "\"/>" << newline;
      }
      log << "</id>";
      iid = geo.emplace(id_spec, make_entry(log)).first;
    }
    return iid->second;
  }
  return empty_entry;
}

/// Convert the electric or magnetic fields into the corresponding Xml object(s).
const DetectorChecksum::entry_t& DetectorChecksum::handleField(OverlayedField f) const {
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
    ifd = geo.emplace(f, make_entry(log)).first;
  }
  return ifd->second;
}

/// Add header information in Detector format
const DetectorChecksum::entry_t& DetectorChecksum::handleHeader() const {
  GeometryInfo& geo = data();
  Header hdr = m_detDesc.header();
  if ( hdr.isValid() && geo.header.data.empty() )  {
    std::stringstream log = logger();
    log << "<header name=\"" << hdr.name() << "\">"
	<< "<autor name=\""  << hdr.author() << "\"/>"
	<< "<generator version=\"" << hdr.version() << "\"" << " url=\"" << hdr.url() << "\"/>"
	<< "<comment>" << hdr.comment() << "</comment>"
	<< "</header>";
    geo.header = make_entry(log);
    return geo.header;
  }
  printout(WARNING,"DetectorChecksum","+++ No Detector header information availible from the geometry description.");
  return empty_entry;
}

void DetectorChecksum::collect_det_elements(DetElement top)  const  {
  auto& geo = data().mapOfDetElements;
  auto it = geo.find(top);
  if ( it == geo.end() )    {
    handleDetElement(top);
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
  if ( debug > 1 )   {
    printout(ALWAYS, "DetectorChecksum", "++ ==> Computing checksum for tree: %s", top.path().c_str());
    printout(ALWAYS, "DetectorChecksum", "++ Handled %ld materials.",      geo.mapOfMaterials.size());
    printout(ALWAYS, "DetectorChecksum", "++ Handled %ld solids.",         geo.mapOfSolids.size());
    printout(ALWAYS, "DetectorChecksum", "++ Handled %ld volumes.",        geo.mapOfVolumes.size());
    printout(ALWAYS, "DetectorChecksum", "++ Handled %ld vis.attributes.", geo.mapOfVis.size());
    printout(ALWAYS, "DetectorChecksum", "++ Handled %ld fields.",         geo.mapOfFields.size());
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

void DetectorChecksum::hash_debug(const std::string& prefix, const entry_t& ent, int flg)  const   {
  if ( debug > 2 )    {
    if ( flg == 1 )    {
      debug_hash << std::setw(16) << std::left << prefix << "." << ent.data << std::endl;
      return;
    }
    debug_hash << std::setw(16) << std::left << (prefix+".hash64:  ") << (void*)ent.hash << std::endl;
    if ( debug > 3 )    {
      debug_hash << std::setw(16) << std::left << prefix << ": |" << ent.data << "|" << std::endl;
    }
  }
}

void DetectorChecksum::checksumDetElement(int lvl, DetElement det, hashes_t& hashes, bool recursive)  const  {
  auto& dat = data();
  auto& geo = dat.mapOfDetElements;
  auto it = geo.find(det);
  if ( it != geo.end() )    {
    std::set<PlacedVolume> child_places;
    std::set<PlacedVolume> hashed_places;
    auto det_pv = det.placement();
    std::size_t hash_idx_de = hashes.size();

    /// Hash DetElement and placement
    hashes.push_back(it->second.hash);
    hash_debug(det.name(), it->second);
    std::size_t hash_idx_ro  = hashes.size();
    std::size_t hash_idx_id  = 0;
    std::size_t hash_idx_seg = 0;
    if ( hash_readout )   {
      SensitiveDetector sd = m_detDesc.sensitiveDetector(det.name());
      if ( sd.isValid() )   {
	Readout ro = sd.readout();
	const auto& sens_ent = handleSensitive(sd);
	hashes.push_back(sens_ent.hash);
	hash_debug(" .sensitive", sens_ent);
	if ( ro.isValid() ) {
	  const auto& id_ent = handleIdSpec(ro.idSpec());
	  const auto& seg_ent = handleSegmentation(ro.segmentation());

	  hash_idx_id = hashes.size();
	  hashes.push_back(id_ent.hash);
	  hash_idx_seg = hashes.size();
	  hashes.push_back(seg_ent.hash);

	  hash_debug(" .iddesc",  id_ent);
	  hash_debug(" .readout", seg_ent);
	}
      }
    }

    std::size_t hash_idx_pv = hashes.size();
    checksumPlacement(det_pv, hashes, false);
    for ( const auto& c : det.children() )
      child_places.emplace(c.second.placement());

    /// Hash all daughters with a DetElement child (but excluding the child)
    /// Note: We only take into account the placements to the next DetElement (if any)
    /// On the fly we remember all placements already taken into account!
    std::size_t hash_idx_daughters = hashes.size();
    for( auto pv : child_places )   {
      auto chain = _get_path(pv, child_places);
      for( std::size_t i=0; i < chain.size()-1; ++i )   {
	checksumPlacement(det_pv, hashes, false);
	hashed_places.insert(chain[i]);
      }
      if ( !chain.empty() ) hashed_places.insert(chain[chain.size()-1]);
    }
    /// Now hash all daughter volumes, which are not linked to a DetElement
    /// in the structural hierarchy.
    const TObjArray* dau = det_pv.volume()->GetNodes();
    if (dau && dau->GetEntries() > 0) {
      for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
	PlacedVolume pv = reinterpret_cast<TGeoNode*>(dau->At(i));
	if ( hashed_places.find(pv) == hashed_places.end() )  {
	  checksumPlacement(det_pv, hashes, true);
	}
      }
    }

    /// Finally: Hash recursively the structural children
    std::size_t hash_idx_children = hashes.size();
    if ( recursive )   {
      for ( const auto& c : det.children() )
	checksumDetElement(lvl+1, c.second, hashes, recursive);
    }

    /// All done: Some debugging printout
    if ( debug > 0 || lvl <= max_level )  {
      std::stringstream str;
      hash_t code = detail::hash64(&hashes[hash_idx_de], (hash_idx_ro-hash_idx_de)*sizeof(hash_t));
      str << "+++ " << std::setw(4) << std::left << lvl
	  << " " << std::setw(36) << std::left << det.name() << " de"
	  << " " << std::setfill('0') << std::setw(16) << std::hex << code;
      code = detail::hash64(&hashes[hash_idx_pv], sizeof(hash_t));
      str << " " << std::setfill(' ') << std::setw(9) << std::left << "+place"
	  << " " << std::setfill('0') << std::setw(16) << std::hex << code;
      code = detail::hash64(&hashes[hash_idx_daughters], (hash_idx_children-hash_idx_daughters)*sizeof(hash_t));
      if ( !(child_places.empty() && hashed_places.empty()) )
	str << " " << std::setfill(' ') << std::setw(10) << std::left << "+daughters"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << code;
      code = detail::hash64(&hashes[hash_idx_children], (hashes.size()-hash_idx_children)*sizeof(hash_t));
      if ( !det.children().empty() )
	str << " " << std::setfill(' ') << std::setw(9) << std::left << "+children"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << code;
      std::cout << str.str() << std::endl;
      if ( hash_idx_pv-hash_idx_ro > 0 )  {
	str.str("");
	str << std::setfill(' ') << "+++ " << std::setw(4) << std::left << lvl
	    << " " << std::setw(56) << std::left << " ";
	code = detail::hash64(&hashes[hash_idx_ro], (hash_idx_pv-hash_idx_ro)*sizeof(hash_t));
	str << " " << std::setfill(' ') << std::setw(9) << std::left << "+readout"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << code;
	if ( hash_idx_id > 0 )  {
	  code = detail::hash64(&hashes[hash_idx_id], sizeof(hash_t));
	  str << " " << std::setfill(' ') << std::setw(10) << std::left << "+iddesc"
	      << " " << std::setfill('0') << std::setw(16) << std::hex << code;
	}
	if ( hash_idx_seg > 0 )  {
	  code = detail::hash64(&hashes[hash_idx_seg], sizeof(hash_t));
	  str << " " << std::setfill(' ') << std::setw(9) << std::left << "+segment"
	      << " " << std::setfill('0') << std::setw(16) << std::hex << code;
	}
	std::cout << str.str() << std::endl;
      }
      if ( lvl == 0 )   {
	str.str("");
	code = detail::hash64(&hashes[0], hashes.size()*sizeof(hash_t));
	str << std::setfill(' ') << "+++ " << std::setw(4) << std::left << lvl
	    << " " << std::setw(39) << std::left << "Combined hash code"
	    << " " << std::setfill('0') << std::setw(16) << std::hex << code
	    << "  (" << std::dec << hashes.size() << " sub-codes)";
	std::cout << str.str() << std::endl;
      }
    }
    return;
  }
  except("DetectorChecksum","ERROR: Cannot checksum invalid DetElement");
}

void DetectorChecksum::checksumPlacement(PlacedVolume pv, hashes_t& hashes, bool recursive)  const  {
  handlePlacement(pv);
  auto& geo = data().mapOfPlacements;
  auto it = geo.find(pv);
  if ( it != geo.end() )    {
    Volume v = pv.volume();
    const auto& vol = handleVolume(v);
    entry_t name_entry {0, pv.name()};

    hash_debug(" .place.name", name_entry, 1);
    hash_debug(" .place", it->second);
    hashes.push_back(it->second.hash);
    hashes.push_back(vol.hash);
    hash_debug(" .place.vol", vol);
    if ( !v.isAssembly() )  {
      const auto& mat = handleMaterial(v.material());
      hashes.push_back(mat.hash);
      hash_debug(" .place.mat", mat);
    }
    if ( recursive )   {
      const TObjArray* dau = v->GetNodes();
      if (dau && dau->GetEntries() > 0)   {
	for (Int_t i = 0, n_dau = dau->GetEntries(); i < n_dau; ++i) {
	  PlacedVolume node = reinterpret_cast<TGeoNode*>(dau->At(i));
	  checksumPlacement(node, hashes, recursive);
	}
      }
    }
    return;
  }
  except("DetectorChecksum","ERROR: Cannot checksum invalid PlacedVolume");
}

/// Dump elements used in this apparatus
void DetectorChecksum::dump_elements()   const   {
  const auto& geo = data().mapOfElements;
  for(const auto& e : geo)   {
    Atom a = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Element   %-32s    0x%016lx%s",
	     a.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump materials used in this apparatus
void DetectorChecksum::dump_materials()   const   {
  const auto& geo = data().mapOfMaterials;
  for(const auto& e : geo)   {
    Material m = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Material  %-32s    0x%016lx%s",
	     m.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump solids used in this apparatus
void DetectorChecksum::dump_solids()   const   {
  const auto& geo = data().mapOfSolids;
  for(const auto& e : geo)   {
    Solid s = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Solid     %-32s    0x%016lx%s",
	     s.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump volumes used in this apparatus
void DetectorChecksum::dump_volumes()   const   {
  const auto& geo = data().mapOfVolumes;
  for(const auto& e : geo)   {
    Volume v = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Volume    %-32s    0x%016lx%s",
	     v.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump placements used in this apparatus
void DetectorChecksum::dump_placements()   const   {
  const auto& geo = data().mapOfPlacements;
  for(const auto& e : geo)   {
    PlacedVolume pv = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Placement %-32s    0x%016lx%s",
	     pv.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump detelements used in this apparatus
void DetectorChecksum::dump_detelements()   const   {
  const auto& geo = data().mapOfDetElements;
  for(const auto& e : geo)   {
    DetElement de = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Detelement %-32s    0x%016lx%s",
	     de.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
    if ( de.path() == "/world" )   {
      PlacedVolume pv = de.placement();
      Volume        v = pv.volume();
      Solid         s = v.solid();
      const auto&  es = handleSolid(s);
      const auto&  ev = handleVolume(v);
      const auto&  ep = handlePlacement(pv);

      printout(ALWAYS, "DetectorChecksum", "    Solid     %-32s    0x%016lx%s",
	       s.name(),  es.hash, debug > 2 ? ("\n"+es.data).c_str() : "");
      printout(ALWAYS, "DetectorChecksum", "    Volume    %-32s    0x%016lx%s",
	       v.name(),  ev.hash, debug > 2 ? ("\n"+ev.data).c_str() : "");
      printout(ALWAYS, "DetectorChecksum", "    Placement %-32s    0x%016lx%s",
	       pv.name(), ep.hash, debug > 2 ? ("\n"+ep.data).c_str() : "");
    }
  }
}

/// Dump iddescriptors used in this apparatus
void DetectorChecksum::dump_iddescriptors()   const   {
  const auto& geo = data().mapOfIdSpecs;
  for(const auto& e : geo)   {
    IDDescriptor v = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ ID desc   %-32s    0x%016lx%s",
	     v.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump segmentations used in this apparatus
void DetectorChecksum::dump_segmentations()   const   {
  const auto& geo = data().mapOfSegmentations;
  for(const auto& e : geo)   {
    Segmentation s = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Segment   %-32s    0x%016lx%s",
	     s.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

/// Dump sensitives used in this apparatus
void DetectorChecksum::dump_sensitives()   const   {
  const auto& geo = data().mapOfSensDets;
  for(const auto& e : geo)   {
    SensitiveDetector sd = e.first;
    printout(ALWAYS, "DetectorChecksum", "+++ Sens.Det. %-32s    0x%016lx%s",
	     sd.name(), e.second.hash, debug > 2 ? ("\n"+e.second.data).c_str() : "");
  }
}

static long create_checksum(Detector& description, int argc, char** argv) {
  std::vector<std::string> detectors;
  int precision = 6, newline = 0, level = 1, readout = 0, debug = 0;
  int dump_elements = 0, dump_materials = 0, dump_solids = 0, dump_volumes = 0;
  int dump_placements = 0, dump_detelements = 0, dump_sensitives = 0;
  int dump_iddesc = 0, dump_segmentations = 0;
  std::string len_unit, ang_unit, ene_unit, dens_unit, atom_unit;

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
    else if ( 0 == ::strncmp("-atomic_unit",argv[i],10) && (i+1)<argc )
      atom_unit = argv[++i];
    else if ( 0 == ::strncmp("-level", argv[i],5) && (i+1)<argc )
      level = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-debug", argv[i],5) && (i+1)<argc )
      debug = ::atol(argv[++i]);
    else if ( 0 == ::strncmp("-newline",argv[i],5) )
      newline = 1;
    else if ( 0 == ::strncmp("-readout",argv[i],5) )
      readout = 1;
    else if ( 0 == ::strncmp("-dump_elements",argv[i],10) )
      dump_elements = 1;
    else if ( 0 == ::strncmp("-dump_materials",argv[i],10) )
      dump_materials = 1;
    else if ( 0 == ::strncmp("-dump_solids",argv[i],10) )
      dump_solids = 1;
    else if ( 0 == ::strncmp("-dump_volumes",argv[i],10) )
      dump_volumes = 1;
    else if ( 0 == ::strncmp("-dump_placements",argv[i],10) )
      dump_placements = 1;
    else if ( 0 == ::strncmp("-dump_detelements",argv[i],10) )
      dump_detelements = 1;
    else if ( 0 == ::strncmp("-dump_sensitives",argv[i],10) )
      dump_sensitives = 1;
    else if ( 0 == ::strncmp("-dump_segmentations",argv[i],10) )
      dump_segmentations = 1;
    else if ( 0 == ::strncmp("-dump_iddescriptors",argv[i],10) )
      dump_iddesc = 1;
    else  {
      std::cout <<
        "Usage: -plugin DD4hepDetectorChecksum -arg [-arg]                             \n\n"
        "     -detector <string>     Top level DetElement path. Default: '/world'        \n"
	"     -readout               also hash the detector's readout properties         \n"
	"                            (sensitive det, id desc, segmentation)              \n"
	"                            default: false                                      \n"
	"                                                                                \n"
	"   Debugging: Dump individual hash codes (debug>=1)                             \n"
	"   Debugging: and the hashed string (debug>2)                                   \n"
	"     -dump_elements         Dump hashes of used elements                        \n"
	"     -dump_materials        Dump hashes of used materials                       \n"
	"     -dump_solids           Dump hashes of used solids                          \n"
	"     -dump_volumes          Dump hashes of used volumes                         \n"
	"     -dump_placements       Dump hashes of used placements                      \n"
	"     -dump_detelements      Dump hashes of used detelements                     \n"
	"     -dump_sensitive        Dump hashes of sensitive detectors                  \n"
	"     -dump_readout          Dump hashes of readout entities                     \n"
	"     -dump_iddescriptors    Dump hashes of ID descriptors                       \n"
	"     -dump_segmentations    Dump hashes of readout segmentations                \n"
	"                                                                                \n"
	"   Modify units in the created hash strings (deprecated):                       \n"
	"     -length_unit  <value>  Unit of length  as literal. default: mm             \n"
	"     -angle_unit   <value>  Unit of angle   as literal. default: deg            \n"
	"     -energy_unit  <value>  Unit of energy  as literal. default: GeV            \n"
	"     -density_unit <value>  Unit of density as literal. default: g/cm3          \n"
	"     -atomic_unit <value>   Unit of atomic weight as literal. default: g/mole   \n"
	"                                                                                \n"
	"     -debug <number>        Enable debug printouts.                             \n"
        "     -help                  Print this help output                              \n"
        "     Arguments given: " << arguments(argc, argv) << std::endl << std::flush;
      ::exit(EINVAL);
    }
  }
  DetectorChecksum wr(description);
  DetElement de = description.world();
  wr.precision = precision;
  if ( !len_unit.empty()  ) wr.m_len_unit_nam = len_unit;
  if ( !ang_unit.empty()  ) wr.m_ang_unit_nam = ang_unit;
  if ( !ene_unit.empty()  ) wr.m_ene_unit_nam = ene_unit;
  if ( !dens_unit.empty() ) wr.m_densunit_nam = dens_unit;
  if ( !atom_unit.empty() ) wr.m_atomunit_nam = atom_unit;
  if ( newline ) wr.newline = "\n";
  wr.hash_readout = readout;
  wr.max_level = level;
  wr.debug = debug;
  wr.configure();

  bool make_dump = false;
  if ( dump_elements   || dump_materials  || dump_solids || 
       dump_volumes    || dump_placements || dump_detelements ||
       dump_sensitives || dump_iddesc     || dump_segmentations )   {
    make_dump = true;
    wr.debug = 1;
  }
  DetectorChecksum::hashes_t hash_vec;
  DetectorChecksum::hash_t checksum = 0;
  if ( !detectors.empty() )  {
    for (const auto& det : detectors )   {
      de = detail::tools::findElement(description,det);
      wr.analyzeDetector(de);
      hash_vec.clear();
      wr.checksumDetElement(0, de, hash_vec, true);
      if ( wr.debug > 2 )   {
	std::cout << wr.debug_hash.str() << std::endl;
	wr.debug_hash.str("");
      }
      checksum = detail::hash64(&hash_vec[0], hash_vec.size()*sizeof(DetectorChecksum::hash_t));
      printout(ALWAYS,"DetectorChecksum","+++ Checksum for %s 0x%016lx",
	       de.path().c_str(), checksum);
      if ( make_dump ) goto MakeDump;
    }
    return 1;
  }
  

  wr.analyzeDetector(de);
  hash_vec.push_back(wr.handleHeader().hash);
  wr.checksumDetElement(0, description.world(), hash_vec, true);
  checksum = detail::hash64(&hash_vec[0], hash_vec.size()*sizeof(DetectorChecksum::hash_t));
  if ( wr.debug > 2 ) std::cout << wr.debug_hash.str() << std::endl;
  printout(ALWAYS,"DetectorChecksum","+++ Checksum for %s 0x%016lx",
	   de.path().c_str(), checksum);

 MakeDump:
  if ( make_dump )   {
    wr.debug = debug;
    if ( dump_elements      ) wr.dump_elements();
    if ( dump_materials     ) wr.dump_materials();
    if ( dump_detelements   ) wr.dump_detelements();
    if ( dump_placements    ) wr.dump_placements();
    if ( dump_volumes       ) wr.dump_volumes();
    if ( dump_solids        ) wr.dump_solids();
    if ( dump_sensitives    ) wr.dump_sensitives();
    if ( dump_segmentations ) wr.dump_segmentations();
    if ( dump_iddesc        ) wr.dump_iddescriptors();
  }
  return 1;
}

DECLARE_APPLY(DD4hepDetectorChecksum, create_checksum)
