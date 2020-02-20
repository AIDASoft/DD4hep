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

// Framework include files
#include <DDCMS/DDShapes.h>
#include <DD4hep/DD4hepUnits.h>

using namespace dd4hep::cms;
namespace units = dd4hep;


/// Optional: special constructor with object creation
DDBox::DDBox(const std::string& nam, double dx, double dy, double dz)  {
  m_element = dd4hep::Box(nam, dx, dy, dz).ptr();
}

/// Optional: special constructor with object creation
DDPolycone::DDPolycone(const std::string& nam,
                       double start_phi, double delta_phi,
                       const std::vector<double>& z,
                       const std::vector<double>& rmin,
                       const std::vector<double>& rmax)
{
  dd4hep::Polycone pc(nam, start_phi, delta_phi);
  pc.addZPlanes(rmin, rmax, z);
  m_element = pc.ptr();
}

double DDPolycone::startPhi() const   {
  return access()->GetPhi1()*units::deg/units::rad;
}

double DDPolycone::deltaPhi() const   {
  return access()->GetDphi()*units::deg/units::rad;
}

std::vector<double> DDPolycone::zVec() const   {
  const auto*   p = access();
  const double* v = p->GetZ();
  return {v, v+p->GetNz()};
}

std::vector<double> DDPolycone::rMinVec() const   {
  const auto*   p = access();
  const double* v = p->GetRmin();
  return {v, v+p->GetNz()};
}

std::vector<double> DDPolycone::rMaxVec() const   {
  const auto*   p = access();
  const double* v = p->GetRmax();
  return {v, v+p->GetNz()};
}

