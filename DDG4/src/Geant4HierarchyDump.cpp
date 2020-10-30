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

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4HierarchyDump.h"

// Geant4 include files
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4ProductionCuts.hh"
#include "G4VUserRegionInformation.hh"
#include "G4Element.hh"
#include "G4SDManager.hh"

#include "G4AssemblyVolume.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4UnionSolid.hh"
#include "G4Paraboloid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4Region.hh"
#include "G4UserLimits.hh"
#include "G4VSensitiveDetector.hh"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4ElectroMagneticField.hh"
#include "G4FieldManager.hh"

// C/C++ include files
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace dd4hep::sim;
using namespace dd4hep;
using namespace std;

static const char* _T(const std::string& str) {
  return str.c_str();
}

/// Initializing Constructor
Geant4HierarchyDump::Geant4HierarchyDump(Detector& description, unsigned long flags)
  : m_detDesc(description), m_flags(flags)
{
  m_flags &= ~G4DUMP_SOLID;
}

/// Standard destructor
Geant4HierarchyDump::~Geant4HierarchyDump() {
}

void Geant4HierarchyDump::dump(const string& indent, const G4VPhysicalVolume* v) const {
  G4LogicalVolume*      lv   = v->GetLogicalVolume();
  G4VSensitiveDetector* sd   = lv->GetSensitiveDetector();
  G4RotationMatrix*     rot  = v->GetObjectRotation();
  G4Material*           mat  = lv->GetMaterial();
  G4VSolid*             sol  = lv->GetSolid();
  G4Region*             rg   = lv->GetRegion();
  G4UserLimits*         ul   = lv->GetUserLimits();
  G4int                 ndau = lv->GetNoDaughters();
  stringstream str;
  char text[32];

  printout(INFO, "Geant4Hierarchy", "%s -> Placement:%s LV:%s Material:%s Solid:%s # of Daughters:%d CopyNo:%d",
	   _T(indent), _T(v->GetName()), _T(lv->GetName()), _T(mat->GetName()),
	   _T(sol->GetName()), ndau, v->GetCopyNo());

  if (sd && (m_flags & G4DUMP_SOLID)) {
    str.str("");
    sol->StreamInfo(str);
    printout(INFO, "Geant4Hierarchy", "%s    Solid:%s", _T(indent), str.str().c_str());
  }
  if (rg && (m_flags & G4DUMP_LIMITS)) {
    G4UserLimits* rg_limits = rg->GetUserLimits();
    str.str("");
    str << indent << "    Region:" << rg->GetName() << " #Materials:" << rg->GetNumberOfMaterials() << " #Volumes:"
        << rg->GetNumberOfRootVolumes();
    if (rg_limits)
      str << " Limits:" << rg_limits->GetType();
    printout(INFO, "Geant4Hierarchy", str.str().c_str());
  }
  if (sd && (m_flags & G4DUMP_SENSDET)) {
    printout(INFO, "Geant4Hierarchy", "%s    Sens.det:%p %s path:%s Active:%-3s #Coll:%d", _T(indent), sd,
             _T(sd->GetName()), _T(sd->GetFullPathName()), yes_no(sd->isActive()), sd->GetNumberOfCollections());
  }
  if (ul && (m_flags & G4DUMP_LIMITS)) {
    printout(INFO, "Geant4Hierarchy", "%s    Limits:%s ", _T(indent), _T(ul->GetType()));
  }
  if (rot && (m_flags & G4DUMP_MATRIX)) {
    const G4ThreeVector     t = v->GetTranslation();
    const G4RotationMatrix& r = *rot;
    double det =
      r.xx()*r.yy()*r.zz() + r.xy()*r.yz()*r.zx() + r.xz()*r.yx()*r.zy() -
      r.zx()*r.yy()*r.xz() - r.zy()*r.yz()*r.xx() - r.zz()*r.yx()*r.xy();
    printout(INFO, "Geant4Hierarchy", "%s    Matrix: %sREFLECTED Tr: %8.3g %8.3g %8.3g [mm]",
	     _T(indent), det > 0e0 ? "NOT " : "", t.x(), t.y(), t.z());
  }
  for (G4int idau = 0; idau < ndau; ++idau) {
    ::snprintf(text, sizeof(text), "  %-3d", idau);
    dump(indent + text, lv->GetDaughter(idau));
  }
}
