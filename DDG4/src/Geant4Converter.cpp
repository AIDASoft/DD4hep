// $Id$
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
#include "DD4hep/Plugins.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/objects/DetectorInterna.h"

#include "DDG4/Geant4Field.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4SensitiveDetector.h"

// ROOT includes
#include "TROOT.h"
#include "TColor.h"
#include "TGeoNode.h"
#include "TGeoShape.h"
#include "TGeoCone.h"
#include "TGeoHype.h"
#include "TGeoPcon.h"
#include "TGeoEltu.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoTube.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoParaboloid.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include "TGeoScaledShape.h"
#include "TGeoManager.h"
#include "TClass.h"
#include "TMath.h"

// Geant4 include files
#include "G4VisAttributes.hh"
#include "G4ProductionCuts.hh"
#include "G4VUserRegionInformation.hh"

#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4Cons.hh"
#include "G4Hype.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4UnionSolid.hh"
#include "G4Paraboloid.hh"
#include "G4Ellipsoid.hh"
#include "G4EllipticalTube.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Region.hh"
#include "G4UserLimits.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4ElectroMagneticField.hh"
#include "G4FieldManager.hh"
#include "G4ReflectionFactory.hh"
#include "CLHEP/Units/SystemOfUnits.h"

// C/C++ include files
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace DD4hep::Simulation;
//using namespace DD4hep::Simulation::Geant4GeometryMaps;
using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

#include "DDG4/Geant4AssemblyVolume.h"
#include "DD4hep/DetectorTools.h"
#include "G4RotationMatrix.hh"
#include "G4AffineTransform.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ReflectionFactory.hh"

void Geant4AssemblyVolume::imprint(Geant4GeometryInfo& info,
                                   const TGeoNode*   parent,
                                   Chain chain,
                                   Geant4AssemblyVolume* pAssembly,
                                   G4LogicalVolume*  pMotherLV,
                                   G4Transform3D&    transformation,
                                   G4int copyNumBase,
                                   G4bool surfCheck )
{
  TGeoVolume* vol = parent->GetVolume();
  static int level=0;
  ++level;


  unsigned int  numberOfDaughters;

  if( copyNumBase == 0 )
  {
    numberOfDaughters = pMotherLV->GetNoDaughters();
  }
  else
  {
    numberOfDaughters = copyNumBase;
  }

  // We start from the first available index
  //
  numberOfDaughters++;

  ImprintsCountPlus();

  std::vector<G4AssemblyTriplet> triplets = pAssembly->fTriplets;
  //cout << " Assembly:" << DetectorTools::placementPath(chain) << endl;

  for( unsigned int i = 0; i < triplets.size(); i++ )  {
    const TGeoNode* node = pAssembly->m_entries[i];
    Chain new_chain = chain;
    new_chain.push_back(node);
    //cout << " Assembly: Entry: " << DetectorTools::placementPath(new_chain) << endl;

    G4Transform3D Ta( *(triplets[i].GetRotation()),
                      triplets[i].GetTranslation() );
    if ( triplets[i].IsReflection() )  { Ta = Ta * G4ReflectZ3D(); }

    G4Transform3D Tfinal = transformation * Ta;

    if ( triplets[i].GetVolume() )
    {
      // Generate the unique name for the next PV instance
      // The name has format:
      //
      // av_WWW_impr_XXX_YYY_ZZZ
      // where the fields mean:
      // WWW - assembly volume instance number
      // XXX - assembly volume imprint number
      // YYY - the name of a log. volume we want to make a placement of
      // ZZZ - the log. volume index inside the assembly volume
      //
      std::stringstream pvName;
      pvName << "av_"
             << GetAssemblyID()
             << "_impr_"
             << GetImprintsCount()
             << "_"
             << triplets[i].GetVolume()->GetName().c_str()
             << "_pv_"
             << i
             << std::ends;

      // Generate a new physical volume instance inside a mother
      // (as we allow 3D transformation use G4ReflectionFactory to
      //  take into account eventual reflection)
      //
      G4PhysicalVolumesPair pvPlaced
        = G4ReflectionFactory::Instance()->Place( Tfinal,
                                                  pvName.str().c_str(),
                                                  triplets[i].GetVolume(),
                                                  pMotherLV,
                                                  false,
                                                  numberOfDaughters + i,
                                                  surfCheck );

      // Register the physical volume created by us so we can delete it later
      //
      fPVStore.push_back( pvPlaced.first );
      info.g4VolumeImprints[vol].push_back(make_pair(new_chain,pvPlaced.first));
#if 0
      cout << " Assembly:Parent:" << parent->GetName() << " " << node->GetName()
           << " " <<  (void*)node << " G4:" << pvName.str() << " Daughter:"
           << DetectorTools::placementPath(new_chain) << endl;
      cout << endl;
#endif

      if ( pvPlaced.second )  {
        G4Exception("G4AssemblyVolume::MakeImprint(..)", "GeomVol0003", FatalException,
                    "Fancy construct popping new mother from the stack!");
        //fPVStore.push_back( pvPlaced.second );
      }
    }
    else if ( triplets[i].GetAssembly() )  {
      // Place volumes in this assembly with composed transformation
      imprint(info, parent, new_chain, (Geant4AssemblyVolume*)triplets[i].GetAssembly(), pMotherLV, Tfinal, i*100+copyNumBase, surfCheck );
    }
    else   {
      --level;
      G4Exception("G4AssemblyVolume::MakeImprint(..)",
                  "GeomVol0003", FatalException,
                  "Triplet has no volume and no assembly");
    }
  }
  //cout << "Imprinted assembly level:" << level << " in mother:" << pMotherLV->GetName() << endl;
  --level;
}

namespace {
  static string indent = "";
  static Double_t s_identity_rot[] = { 1., 0., 0., 0., 1., 0., 0., 0., 1. };
  struct MyTransform3D : public G4Transform3D {
    MyTransform3D(double XX, double XY, double XZ, double DX, double YX, double YY, double YZ, double DY, double ZX, double ZY,
                  double ZZ, double DZ)
      : G4Transform3D(XX, XY, XZ, DX, YX, YY, YZ, DY, ZX, ZY, ZZ, DZ) {
    }
    MyTransform3D(const double* t, const double* r = s_identity_rot)
      : G4Transform3D(r[0],r[1],r[2],t[0]*CM_2_MM,r[3],r[4],r[5],t[1]*CM_2_MM,r[6],r[7],r[8],t[2]*CM_2_MM)  {
    }
  };

#if 0  // warning: unused function 'handleName' [-Wunused-function]
  void handleName(const TGeoNode* n) {
    TGeoVolume* v = n->GetVolume();
    TGeoMedium* m = v->GetMedium();
    TGeoShape* s = v->GetShape();
    string nam;
    printout(DEBUG, "G4", "TGeoNode:'%s' Vol:'%s' Shape:'%s' Medium:'%s'", n->GetName(), v->GetName(), s->GetName(),
             m->GetName());
  }
#endif

  class G4UserRegionInformation : public G4VUserRegionInformation {
  public:
    Region region;
    double threshold;
    bool storeSecondaries;
    G4UserRegionInformation()
      : threshold(0.0), storeSecondaries(false) {
    }
    virtual ~G4UserRegionInformation() {
    }
    virtual void Print() const {
      if (region.isValid())
        printout(DEBUG, "Region", "Name:%s", region.name());
    }
  };
}

/// Initializing Constructor
Geant4Converter::Geant4Converter(LCDD& lcdd_ref)
  : Geant4Mapping(lcdd_ref), m_checkOverlaps(true) {
  this->Geant4Mapping::init();
  m_propagateRegions = true;
  m_outputLevel = PrintLevel(printLevel() - 1);
}

/// Initializing Constructor
Geant4Converter::Geant4Converter(LCDD& lcdd_ref, PrintLevel level)
  : Geant4Mapping(lcdd_ref), m_checkOverlaps(true) {
  this->Geant4Mapping::init();
  m_propagateRegions = true;
  m_outputLevel = level;
}

/// Standard destructor
Geant4Converter::~Geant4Converter() {
}

/// Dump element in GDML format to output stream
void* Geant4Converter::handleElement(const string& name, const Atom element) const {
  G4Element* g4e = data().g4Elements[element];
  if (!g4e) {
    g4e = G4Element::GetElement(name, false);
    if (!g4e) {
      if (element->GetNisotopes() > 1) {
        g4e = new G4Element(name, element->GetTitle(), element->GetNisotopes());
        for (int i = 0, n = element->GetNisotopes(); i < n; ++i) {
          TGeoIsotope* iso = element->GetIsotope(i);
          G4Isotope* g4iso = G4Isotope::GetIsotope(iso->GetName(), false);
          if (!g4iso) {
            g4iso = new G4Isotope(iso->GetName(), iso->GetZ(), iso->GetN(), iso->GetA());
          }
          g4e->AddIsotope(g4iso, element->GetRelativeAbundance(i));
        }
      }
      else {
        g4e = new G4Element(element->GetTitle(), name, element->Z(), element->A() * (CLHEP::g / CLHEP::mole));
      }
      stringstream str;
      str << (*g4e);
      printout(m_outputLevel, "Geant4Converter", "++ Created G4 %s", str.str().c_str());
    }
    data().g4Elements[element] = g4e;
  }
  return g4e;
}

/// Dump material in GDML format to output stream
void* Geant4Converter::handleMaterial(const string& name, Material medium) const {
  G4Material* mat = data().g4Materials[medium];
  if (!mat) {
    mat = G4Material::GetMaterial(name, false);
    if (!mat) {
      TGeoMaterial* material = medium->GetMaterial();
      G4State state = kStateUndefined;
      double density = material->GetDensity() * (CLHEP::gram / CLHEP::cm3);
      if (density < 1e-25)
        density = 1e-25;
      switch (material->GetState()) {
      case TGeoMaterial::kMatStateSolid:
        state = kStateSolid;
        break;
      case TGeoMaterial::kMatStateLiquid:
        state = kStateLiquid;
        break;
      case TGeoMaterial::kMatStateGas:
        state = kStateGas;
        break;
      default:
      case TGeoMaterial::kMatStateUndefined:
        state = kStateUndefined;
        break;
      }
      if (material->IsMixture()) {
        double A_total = 0.0;
        double W_total = 0.0;
        TGeoMixture* mix = (TGeoMixture*) material;
        int nElements = mix->GetNelements();
        mat = new G4Material(name, density, nElements, state, 
                             material->GetTemperature(), material->GetPressure());
        for (int i = 0; i < nElements; ++i)  {
          A_total += (mix->GetAmixt())[i];
          W_total += (mix->GetWmixt())[i];
        }
        for (int i = 0; i < nElements; ++i) {
          TGeoElement* e = mix->GetElement(i);
          G4Element* g4e = (G4Element*) handleElement(e->GetName(), Atom(e));
          if (!g4e) {
            printout(ERROR, "Material", 
                     "Missing component %s for material %s. A=%f W=%f", 
                     e->GetName(), mix->GetName(), A_total, W_total);
          }
          //mat->AddElement(g4e, (mix->GetAmixt())[i] / A_total);
          mat->AddElement(g4e, (mix->GetWmixt())[i] / W_total);
        }
      }
      else {
        double z = material->GetZ(), a = material->GetA();
        if ( z < 1.0000001 ) z = 1.0;
        if ( a < 0.5000001 ) a = 1.0;
        mat = new G4Material(name, z, a, density, state, 
                             material->GetTemperature(), material->GetPressure());
      }
      stringstream str;
      str << (*mat);
      printout(m_outputLevel, "Geant4Converter", "++ Created G4 %s", str.str().c_str());
    }
    data().g4Materials[medium] = mat;
  }
  return mat;
}

/// Dump solid in GDML format to output stream
void* Geant4Converter::handleSolid(const string& name, const TGeoShape* shape) const {
  G4VSolid* solid = 0;
  if (shape) {
    if (0 != (solid = data().g4Solids[shape])) {
      return solid;
    }
    else if (shape->IsA() == TGeoShapeAssembly::Class()) {
      // Assemblies have no corresponding 'shape' in Geant4. Ignore the shape translation.
      // It does not harm, since this 'shape' is never accessed afterwards.
      data().g4Solids[shape] = solid;
      return solid;
    }
    else if (shape->IsA() == TGeoBBox::Class()) {
      const TGeoBBox* s = (const TGeoBBox*) shape;
      solid = new G4Box(name, s->GetDX() * CM_2_MM, s->GetDY() * CM_2_MM, s->GetDZ() * CM_2_MM);
    }
    else if (shape->IsA() == TGeoTube::Class()) {
      const TGeoTube* s = (const TGeoTube*) shape;
      solid = new G4Tubs(name, s->GetRmin() * CM_2_MM, s->GetRmax() * CM_2_MM, s->GetDz() * CM_2_MM, 0, 2. * M_PI);
    }
    else if (shape->IsA() == TGeoTubeSeg::Class()) {
      const TGeoTubeSeg* s = (const TGeoTubeSeg*) shape;
      solid = new G4Tubs(name, s->GetRmin() * CM_2_MM, s->GetRmax() * CM_2_MM, s->GetDz() * CM_2_MM,
                         s->GetPhi1() * DEGREE_2_RAD, (s->GetPhi2()-s->GetPhi1()) * DEGREE_2_RAD);
    }
    else if (shape->IsA() == TGeoEltu::Class()) {
      const TGeoEltu* s = (const TGeoEltu*) shape;
      solid = new G4EllipticalTube(name,s->GetA() * CM_2_MM, s->GetB() * CM_2_MM, s->GetDz() * CM_2_MM);
    }
    else if (shape->IsA() == TGeoTrd1::Class()) {
      const TGeoTrd1* s = (const TGeoTrd1*) shape;
      solid = new G4Trd(name, s->GetDx1() * CM_2_MM, s->GetDx2() * CM_2_MM, s->GetDy() * CM_2_MM, s->GetDy() * CM_2_MM,
                        s->GetDz() * CM_2_MM);
    }
    else if (shape->IsA() == TGeoTrd2::Class()) {
      const TGeoTrd2* s = (const TGeoTrd2*) shape;
      solid = new G4Trd(name, s->GetDx1() * CM_2_MM, s->GetDx2() * CM_2_MM, s->GetDy1() * CM_2_MM, s->GetDy2() * CM_2_MM,
                        s->GetDz() * CM_2_MM);
    }
    else if (shape->IsA() == TGeoHype::Class()) {
      const TGeoHype* s = (const TGeoHype*) shape;
      solid = new G4Hype(name, s->GetRmin() * CM_2_MM, s->GetRmax() * CM_2_MM,
                         s->GetStIn() * DEGREE_2_RAD, s->GetStOut() * DEGREE_2_RAD,
                         s->GetDz() * CM_2_MM);
    }
    else if (shape->IsA() == TGeoPgon::Class()) {
      const TGeoPgon* s = (const TGeoPgon*) shape;
      double phi_start = s->GetPhi1() * DEGREE_2_RAD;
      double phi_total = (s->GetDphi() + s->GetPhi1()) * DEGREE_2_RAD;
      vector<double> rmin, rmax, z;
      for (Int_t i = 0; i < s->GetNz(); ++i) {
        rmin.push_back(s->GetRmin(i) * CM_2_MM);
        rmax.push_back(s->GetRmax(i) * CM_2_MM);
        z.push_back(s->GetZ(i) * CM_2_MM);
      }
      solid = new G4Polyhedra(name, phi_start, phi_total, s->GetNedges(), s->GetNz(), &z[0], &rmin[0], &rmax[0]);
    }
    else if (shape->IsA() == TGeoPcon::Class()) {
      const TGeoPcon* s = (const TGeoPcon*) shape;
      double phi_start = s->GetPhi1() * DEGREE_2_RAD;
      double phi_total = (s->GetDphi() + s->GetPhi1()) * DEGREE_2_RAD;
      vector<double> rmin, rmax, z;
      for (Int_t i = 0; i < s->GetNz(); ++i) {
        rmin.push_back(s->GetRmin(i) * CM_2_MM);
        rmax.push_back(s->GetRmax(i) * CM_2_MM);
        z.push_back(s->GetZ(i) * CM_2_MM);
      }
      solid = new G4Polycone(name, phi_start, phi_total, s->GetNz(), &z[0], &rmin[0], &rmax[0]);
    }
    else if (shape->IsA() == TGeoCone::Class()) {
      const TGeoCone* s = (const TGeoCone*) shape;
      solid = new G4Cons(name, s->GetRmin1() * CM_2_MM, s->GetRmax1() * CM_2_MM, s->GetRmin2() * CM_2_MM,
                         s->GetRmax2() * CM_2_MM, s->GetDz() * CM_2_MM, 0.0, 2.*M_PI);
    }
    else if (shape->IsA() == TGeoConeSeg::Class()) {
      const TGeoConeSeg* s = (const TGeoConeSeg*) shape;
      solid = new G4Cons(name, s->GetRmin1() * CM_2_MM, s->GetRmax1() * CM_2_MM, s->GetRmin2() * CM_2_MM,
                         s->GetRmax2() * CM_2_MM, s->GetDz() * CM_2_MM, s->GetPhi1() * DEGREE_2_RAD, (s->GetPhi2()-s->GetPhi1()) * DEGREE_2_RAD);
    }
    else if (shape->IsA() == TGeoParaboloid::Class()) {
      const TGeoParaboloid* s = (const TGeoParaboloid*) shape;
      solid = new G4Paraboloid(name, s->GetDz() * CM_2_MM, s->GetRlo() * CM_2_MM, s->GetRhi() * CM_2_MM);
    }
#if 0  /* Not existent */
    else if (shape->IsA() == TGeoEllisoid::Class()) {
      const TGeoParaboloid* s = (const TGeoParaboloid*) shape;
      solid = new G4Paraboloid(name, s->GetDz() * CM_2_MM, s->GetRlo() * CM_2_MM, s->GetRhi() * CM_2_MM);
    }
#endif
    else if (shape->IsA() == TGeoSphere::Class()) {
      const TGeoSphere* s = (const TGeoSphere*) shape;
      solid = new G4Sphere(name, s->GetRmin() * CM_2_MM, s->GetRmax() * CM_2_MM, s->GetPhi1() * DEGREE_2_RAD,
                           s->GetPhi2() * DEGREE_2_RAD, s->GetTheta1() * DEGREE_2_RAD, s->GetTheta2() * DEGREE_2_RAD);
    }
    else if (shape->IsA() == TGeoTorus::Class()) {
      const TGeoTorus* s = (const TGeoTorus*) shape;
      solid = new G4Torus(name, s->GetRmin() * CM_2_MM, s->GetRmax() * CM_2_MM, s->GetR() * CM_2_MM,
                          s->GetPhi1() * DEGREE_2_RAD, s->GetDphi() * DEGREE_2_RAD);
    }
    else if (shape->IsA() == TGeoTrap::Class()) {
      const TGeoTrap* s = (const TGeoTrap*) shape;
      solid = new G4Trap(name, s->GetDz() * CM_2_MM, s->GetTheta(), s->GetPhi(),
                         s->GetH1() * CM_2_MM, s->GetBl1() * CM_2_MM, s->GetTl1() * CM_2_MM, s->GetAlpha1() * DEGREE_2_RAD,
                         s->GetH2() * CM_2_MM, s->GetBl2() * CM_2_MM, s->GetTl2() * CM_2_MM, s->GetAlpha2() * DEGREE_2_RAD);
    }
    else if (shape->IsA() == TGeoCompositeShape::Class()) {
      const TGeoCompositeShape* s = (const TGeoCompositeShape*) shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      TGeoMatrix* m = boolean->GetRightMatrix();
      G4VSolid* left = (G4VSolid*) handleSolid(name + "_left", boolean->GetLeftShape());
      G4VSolid* right = (G4VSolid*) handleSolid(name + "_right", boolean->GetRightShape());

      if (!left) {
        throw runtime_error("G4Converter: No left Geant4 Solid present for composite shape:" + name);
      }
      if (!right) {
        throw runtime_error("G4Converter: No right Geant4 Solid present for composite shape:" + name);
      }

      //specific case!
      //Ellipsoid tag preparing
      //if left == TGeoScaledShape AND right  == TGeoBBox
      //   AND if TGeoScaledShape->GetShape == TGeoSphere
      TGeoShape* ls = boolean->GetLeftShape();
      TGeoShape* rs = boolean->GetRightShape();
      if (strcmp(ls->ClassName(), "TGeoScaledShape") == 0 &&
          strcmp(rs->ClassName(), "TGeoBBox") == 0) {
        if (strcmp(((TGeoScaledShape *)ls)->GetShape()->ClassName(), "TGeoSphere") == 0) {
          if (oper == TGeoBoolNode::kGeoIntersection) {
            TGeoScaledShape* lls = (TGeoScaledShape *)ls;
            TGeoBBox* rrs = (TGeoBBox*)rs;
            double sx = lls->GetScale()->GetScale()[0];
            double sy = lls->GetScale()->GetScale()[1];
            double radius = ((TGeoSphere *)lls->GetShape())->GetRmax();
            double dz = rrs->GetDZ();
            double zorig = rrs->GetOrigin()[2];
            double zcut2 = dz + zorig;
            double zcut1 = 2 * zorig - zcut2;
            solid = new G4Ellipsoid(name,
                                    sx * radius * CM_2_MM,
                                    sy * radius * CM_2_MM,
                                    radius * CM_2_MM,
                                    zcut1 * CM_2_MM,
                                    zcut2 * CM_2_MM);
            data().g4Solids[shape] = solid;
            return solid;
          }
        }
      }

      if (m->IsRotation()) {
        MyTransform3D transform(m->GetTranslation(),m->GetRotationMatrix());
        if (oper == TGeoBoolNode::kGeoSubtraction)
          solid = new G4SubtractionSolid(name, left, right, transform);
        else if (oper == TGeoBoolNode::kGeoUnion)
          solid = new G4UnionSolid(name, left, right, transform);
        else if (oper == TGeoBoolNode::kGeoIntersection)
          solid = new G4IntersectionSolid(name, left, right, transform);
      }
      else {
        const Double_t *t = m->GetTranslation();
        G4ThreeVector transform(t[0] * CM_2_MM, t[1] * CM_2_MM, t[2] * CM_2_MM);
        if (oper == TGeoBoolNode::kGeoSubtraction)
          solid = new G4SubtractionSolid(name, left, right, 0, transform);
        else if (oper == TGeoBoolNode::kGeoUnion)
          solid = new G4UnionSolid(name, left, right, 0, transform);
        else if (oper == TGeoBoolNode::kGeoIntersection)
          solid = new G4IntersectionSolid(name, left, right, 0, transform);
      }
    }

    if (!solid) {
      string err = "Failed to handle unknown solid shape:" + name + " of type " + string(shape->IsA()->GetName());
      throw runtime_error(err);
    }
    data().g4Solids[shape] = solid;
  }
  return solid;
}

/// Dump logical volume in GDML format to output stream
void* Geant4Converter::handleVolume(const string& name, const TGeoVolume* volume) const {
  Geant4GeometryInfo& info = data();
  Geant4GeometryMaps::VolumeMap::const_iterator volIt = info.g4Volumes.find(volume);
  if (volIt == info.g4Volumes.end() ) {
    const TGeoVolume* v = volume;
    Volume _v = Ref_t(v);
    string n = v->GetName();
    TGeoMedium* m = v->GetMedium();
    TGeoShape* s = v->GetShape();
    G4VSolid* solid = (G4VSolid*) handleSolid(s->GetName(), s);
    G4Material* medium = 0;
    bool assembly = s->IsA() == TGeoShapeAssembly::Class() || v->IsA() == TGeoVolumeAssembly::Class();

    LimitSet lim = _v.limitSet();
    G4UserLimits* user_limits = 0;
    if (lim.isValid()) {
      user_limits = info.g4Limits[lim];
      if (!user_limits) {
        throw runtime_error("G4Cnv::volume[" + name + "]:    + FATAL Failed to "
                            "access Geant4 user limits.");
      }
    }
    VisAttr vis = _v.visAttributes();
    G4VisAttributes* vis_attr = 0;
    if (vis.isValid()) {
      vis_attr = (G4VisAttributes*)handleVis(vis.name(), vis);
    }
    Region reg = _v.region();
    G4Region* region = 0;
    if (reg.isValid()) {
      region = info.g4Regions[reg];
      if (!region) {
        throw runtime_error("G4Cnv::volume[" + name + "]:    + FATAL Failed to "
                            "access Geant4 region.");
      }
    }
    PrintLevel lvl = m_outputLevel; //string(det.name())=="SiTrackerBarrel" ? WARNING : m_outputLevel;
    printout(lvl, "Geant4Converter", "++ Convert Volume %-32s: %p %s/%s assembly:%s",
             n.c_str(), v, s->IsA()->GetName(), v->IsA()->GetName(), yes_no(assembly));

    if (assembly) {
      //info.g4AssemblyVolumes[v] = new Geant4AssemblyVolume();
      return 0;
    }
    medium = (G4Material*) handleMaterial(m->GetName(), Material(m));
    if (!solid) {
      throw runtime_error("G4Converter: No Geant4 Solid present for volume:" + n);
    }
    if (!medium) {
      throw runtime_error("G4Converter: No Geant4 material present for volume:" + n);
    }
    if (user_limits) {
      printout(m_outputLevel, "Geant4Converter", "++ Volume     + Apply LIMITS settings:%-24s to volume %s.",
               lim.name(), _v.name());
    }
    G4LogicalVolume* vol = new G4LogicalVolume(solid, medium, n, 0, 0, user_limits);
    if (region) {
      printout(m_outputLevel, "Geant4Converter", "++ Volume     + Apply REGION settings: %s to volume %s.",
               reg.name(), _v.name());
      vol->SetRegion(region);
      region->AddRootLogicalVolume(vol);
    }
    if (vis_attr) {
      vol->SetVisAttributes(vis_attr);
    }
    info.g4Volumes[v] = vol;
    printout(m_outputLevel, "Geant4Converter", "++ Volume     + %s converted: %p ---> G4: %p", n.c_str(), v, vol);
  }
  return 0;
}

/// Dump logical volume in GDML format to output stream
void* Geant4Converter::collectVolume(const string& /* name */, const TGeoVolume* volume) const {
  Geant4GeometryInfo& info = data();
  const TGeoVolume* v = volume;
  Volume _v = Ref_t(v);
  Region reg = _v.region();
  LimitSet lim = _v.limitSet();
  SensitiveDetector det = _v.sensitiveDetector();

  if (lim.isValid())
    info.limits[lim].insert(v);
  if (reg.isValid())
    info.regions[reg].insert(v);
  if (det.isValid())
    info.sensitives[det].insert(v);
  return (void*) v;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handleAssembly(const std::string& name, const TGeoNode* node) const {
  TGeoVolume* mot_vol = node->GetVolume();
  if ( mot_vol->IsA() != TGeoVolumeAssembly::Class() )    {
    return 0;
  }
  Geant4GeometryInfo& info = data();
  Geant4AssemblyVolume* g4 = info.g4AssemblyVolumes[node];

  if ( !g4 )  {
    g4 = new Geant4AssemblyVolume();
    for(Int_t i=0; i < mot_vol->GetNdaughters(); ++i)   {
      TGeoNode*   d = mot_vol->GetNode(i);
      TGeoVolume* dau_vol = d->GetVolume();
      TGeoMatrix* tr = d->GetMatrix();
      MyTransform3D transform(tr->GetTranslation(),tr->IsRotation() ? tr->GetRotationMatrix() : s_identity_rot);
      if ( dau_vol->IsA() == TGeoVolumeAssembly::Class() )  {
        Geant4GeometryMaps::AssemblyMap::iterator assIt = info.g4AssemblyVolumes.find(d);
        if ( assIt == info.g4AssemblyVolumes.end() )  {
          printout(FATAL, "Geant4Converter", "+++ Invalid child assembly at %s : %d  parent: %s child:%s",
                   __FILE__, __LINE__, name.c_str(), d->GetName());
        }
        g4->placeAssembly(d,(*assIt).second,transform);
        printout(m_outputLevel, "Geant4Converter", "+++ Assembly: AddPlacedAssembly : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 dau_vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
      }
      else   {
        Geant4GeometryMaps::VolumeMap::iterator volIt = info.g4Volumes.find(dau_vol);
        if ( volIt == info.g4Volumes.end() )  {
          printout(FATAL, "Geant4Converter", "+++ Invalid child volume at %s : %d  parent: %s child:%s",
                   __FILE__, __LINE__, name.c_str(), d->GetName());
        }
        g4->placeVolume(d,(*volIt).second, transform);
        printout(m_outputLevel, "Geant4Converter", "+++ Assembly: AddPlacedVolume : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 dau_vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
      }
    }
    info.g4AssemblyVolumes[node] = g4;
  }
  return g4;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handlePlacement(const string& name, const TGeoNode* node) const {
  Geant4GeometryInfo& info = data();
  Geant4GeometryMaps::PlacementMap::const_iterator g4it = info.g4Placements.find(node);
  G4VPhysicalVolume* g4 = (g4it == info.g4Placements.end()) ? 0 : (*g4it).second;
  if (!g4) {
    TGeoVolume* mot_vol = node->GetMotherVolume();
    TGeoVolume* vol = node->GetVolume();
    TGeoMatrix* tr = node->GetMatrix();
    if (!tr) {
      printout(FATAL, "Geant4Converter", "++ Attempt to handle placement without transformation:%p %s of type %s vol:%p", node,
               node->GetName(), node->IsA()->GetName(), vol);
    }
    else if (0 == vol) {
      printout(FATAL, "Geant4Converter", "++ Unknown G4 volume:%p %s of type %s ptr:%p", node, node->GetName(),
               node->IsA()->GetName(), vol);
    }
    else {
      int copy = node->GetNumber();
      bool node_is_assembly = vol->IsA() == TGeoVolumeAssembly::Class();
      bool mother_is_assembly = mot_vol ? mot_vol->IsA() == TGeoVolumeAssembly::Class() : false;
      MyTransform3D transform(tr->GetTranslation(),tr->IsRotation() ? tr->GetRotationMatrix() : s_identity_rot);
      Geant4GeometryMaps::VolumeMap::const_iterator volIt = info.g4Volumes.find(mot_vol);

      if ( mother_is_assembly )   {
        //
        // Mother is an assembly:
        // Nothing to do here, because:
        // -- placed volumes were already added before in "handleAssembly"
        // -- imprint cannot be made, because this requires a logical volume as a mother
        //
        printout(m_outputLevel, "Geant4Converter", "+++ Assembly: **** : dau:%s "
                 "to mother %s Tr:x=%8.3f y=%8.3f z=%8.3f",
                 vol->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
        return 0;
      }
      else if ( node_is_assembly )   {
        //
        // Node is an assembly:
        // Imprint the assembly. The mother MUST already be transformed.
        //
        printout(m_outputLevel, "Geant4Converter", "+++ Assembly: makeImprint: dau:%s in mother %s "
                 "Tr:x=%8.3f y=%8.3f z=%8.3f",
                 node->GetName(), mot_vol->GetName(),
                 transform.dx(), transform.dy(), transform.dz());
        Geant4AssemblyVolume* ass = (Geant4AssemblyVolume*)info.g4AssemblyVolumes[node];
        Geant4AssemblyVolume::Chain chain;
        chain.push_back(node);
        ass->imprint(info,node,chain,ass,(*volIt).second, transform, copy, m_checkOverlaps);
        return 0;
      }
      else if ( node != gGeoManager->GetTopNode() && volIt == info.g4Volumes.end() )  {
        throw logic_error("Geant4Converter: Invalid mother volume found!");
      }
      G4LogicalVolume* g4vol = info.g4Volumes[vol];
      G4LogicalVolume* g4mot = info.g4Volumes[mot_vol];
      g4 = new G4PVPlacement(transform,   // no rotation
                             g4vol,     // its logical volume
                             name,      // its name
                             g4mot,     // its mother (logical) volume
                             false,     // no boolean operations
                             copy,      // its copy number
                             m_checkOverlaps);
    }
    info.g4Placements[node] = g4;
  }
  else {
    printout(ERROR, "Geant4Converter", "++ Attempt to DOUBLE-place physical volume: %s No:%d", name.c_str(), node->GetNumber());
  }
  return g4;
}

/// Convert the geometry type region into the corresponding Geant4 object(s).
void* Geant4Converter::handleRegion(Region region, const set<const TGeoVolume*>& /* volumes */) const {
  G4Region* g4 = data().g4Regions[region];
  if (!g4) {
    Region r = Ref_t(region);
    g4 = new G4Region(r.name());
    // set production cut
    G4ProductionCuts* cuts = new G4ProductionCuts();
    cuts->SetProductionCut(r.cut()*CLHEP::mm/dd4hep::mm);
    g4->SetProductionCuts(cuts);

    // create region info with storeSecondaries flag
    G4UserRegionInformation* info = new G4UserRegionInformation();
    info->region = r;
    info->threshold = r.threshold()*CLHEP::MeV/dd4hep::MeV;
    info->storeSecondaries = r.storeSecondaries();
    g4->SetUserInformation(info);

    printout(m_outputLevel, "Geant4Converter", "++ Converted region settings of:%s.", r.name());
    vector < string > &limits = r.limits();
    for (vector<string>::const_iterator i = limits.begin(); i != limits.end(); ++i) {
      const string& nam = *i;
      LimitSet ls = m_lcdd.limitSet(nam);
      if (ls.isValid()) {
        bool found = false;
        const Geant4GeometryMaps::LimitMap& lm = data().g4Limits;
        for (Geant4GeometryMaps::LimitMap::const_iterator j = lm.begin(); j != lm.end(); ++j) {
          if (nam == (*j).first->GetName()) {
            g4->SetUserLimits((*j).second);
            found = true;
            break;
          }
        }
        if (found)
          continue;
      }
      throw runtime_error("G4Region: Failed to resolve user limitset:" + *i);
    }
    data().g4Regions[region] = g4;
  }
  return g4;
}

/// Convert the geometry type LimitSet into the corresponding Geant4 object(s).
void* Geant4Converter::handleLimitSet(LimitSet limitset, const set<const TGeoVolume*>& /* volumes */) const {
  G4UserLimits* g4 = data().g4Limits[limitset];
  if (!g4) {
    LimitSet ls = Ref_t(limitset);
    g4 = new G4UserLimits(limitset->GetName());
    const set<Limit>& limits = ls.limits();
    for (LimitSet::Object::const_iterator i = limits.begin(); i != limits.end(); ++i) {
      const Limit& l = *i;
      if (l.name == "step_length_max")
        g4->SetMaxAllowedStep(l.value*CLHEP::mm/dd4hep::mm);
      else if (l.name == "track_length_max")
        g4->SetMaxAllowedStep(l.value*CLHEP::mm/dd4hep::mm);
      else if (l.name == "time_max")
        g4->SetUserMaxTime(l.value*CLHEP::ns/dd4hep::ns);
      else if (l.name == "ekin_min")
        g4->SetUserMinEkine(l.value*CLHEP::MeV/dd4hep::MeV);
      else if (l.name == "range_min")
        g4->SetUserMinRange(l.value);
      else
        throw runtime_error("Unknown Geant4 user limit: " + l.toString());
    }
    data().g4Limits[limitset] = g4;
  }
  return g4;
}

/// Convert the geometry visualisation attributes to the corresponding Geant4 object(s).
void* Geant4Converter::handleVis(const string& /* name */, VisAttr attr) const {
  Geant4GeometryInfo& info = data();
  G4VisAttributes* g4 = info.g4Vis[attr];
  if (!g4) {
    float r = 0, g = 0, b = 0;
    int style = attr.lineStyle();
    attr.rgb(r, g, b);
    g4 = new G4VisAttributes(attr.visible(), G4Colour(r, g, b, attr.alpha()));
    //g4->SetLineWidth(attr->GetLineWidth());
    g4->SetDaughtersInvisible(!attr.showDaughters());
    if (style == VisAttr::SOLID) {
      g4->SetLineStyle(G4VisAttributes::unbroken);
      g4->SetForceWireframe(false);
      g4->SetForceSolid(true);
    }
    else if (style == VisAttr::WIREFRAME || style == VisAttr::DASHED) {
      g4->SetLineStyle(G4VisAttributes::dashed);
      g4->SetForceSolid(false);
      g4->SetForceWireframe(true);
    }
    info.g4Vis[attr] = g4;
  }
  return g4;
}

/// Handle the geant 4 specific properties
void Geant4Converter::handleProperties(LCDD::Properties& prp) const {
  map < string, string > processors;
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
        char txt[32];
        ::snprintf(txt, sizeof(txt), "%d", ++s_idd);
        id = txt;
      }
      processors.insert(make_pair(id, nam));
    }
  }
  for (map<string, string>::const_iterator i = processors.begin(); i != processors.end(); ++i) {
    const Geometry::GeoHandler* hdlr = this;
    string nam = (*i).second;
    const LCDD::PropertyValues& vals = prp[nam];
    string type = vals.find("type")->second;
    string tag = type + "_Geant4_action";
    long result = PluginService::Create<long>(tag, &m_lcdd, hdlr, &vals);
    if (0 == result) {
      throw runtime_error("Failed to locate plugin to interprete files of type"
                          " \"" + tag + "\" - no factory:" + type);
    }
    result = *(long*) result;
    if (result != 1) {
      throw runtime_error("Failed to invoke the plugin " + tag + " of type " + type);
    }
    printout(m_outputLevel, "Geant4Converter", "+++++ Executed Successfully Geant4 setup module *%s*.", type.c_str());
  }
}

/// Convert the geometry type SensitiveDetector into the corresponding Geant4 object(s).
void Geant4Converter::printSensitive(SensitiveDetector sens_det, const set<const TGeoVolume*>& /* volumes */) const {
  Geant4GeometryInfo& info = data();
  ConstVolumeSet& volset = info.sensitives[sens_det];
  SensitiveDetector sd = Ref_t(sens_det);
  stringstream str;

  printout(INFO, "Geant4Converter", "++ SensitiveDetector: %-18s %-20s Hits:%-16s", sd.name(), ("[" + sd.type() + "]").c_str(),
           sd.hitsCollection().c_str());
  str << "                    | " << "Cutoff:" << setw(6) << left << sd.energyCutoff() << setw(5) << right << volset.size()
      << " volumes ";
  if (sd.region().isValid())
    str << " Region:" << setw(12) << left << sd.region().name();
  if (sd.limits().isValid())
    str << " Limits:" << setw(12) << left << sd.limits().name();
  str << ".";
  printout(INFO, "Geant4Converter", str.str().c_str());

  for (ConstVolumeSet::iterator i = volset.begin(); i != volset.end(); ++i) {
    map<Volume, G4LogicalVolume*>::iterator v = info.g4Volumes.find(*i);
    G4LogicalVolume* vol = (*v).second;
    str.str("");
    str << "                                   | " << "Volume:" << setw(24) << left << vol->GetName() << " "
        << vol->GetNoDaughters() << " daughters.";
    printout(INFO, "Geant4Converter", str.str().c_str());
  }
}

string printSolid(G4VSolid* sol) {
  stringstream str;
  if (typeid(*sol) == typeid(G4Box)) {
    const G4Box* b = (G4Box*) sol;
    str << "++ Box: x=" << b->GetXHalfLength() << " y=" << b->GetYHalfLength() << " z=" << b->GetZHalfLength();
  }
  else if (typeid(*sol) == typeid(G4Tubs)) {
    const G4Tubs* t = (const G4Tubs*) sol;
    str << " Tubs: Ri=" << t->GetInnerRadius() << " Ra=" << t->GetOuterRadius() << " z/2=" << t->GetZHalfLength() << " Phi="
        << t->GetStartPhiAngle() << "..." << t->GetDeltaPhiAngle();
  }
  return str.str();
}

/// Print G4 placement
void* Geant4Converter::printPlacement(const string& name, const TGeoNode* node) const {
  Geant4GeometryInfo& info = data();
  G4VPhysicalVolume* g4 = info.g4Placements[node];
  G4LogicalVolume* vol = info.g4Volumes[node->GetVolume()];
  G4LogicalVolume* mot = info.g4Volumes[node->GetMotherVolume()];
  G4VSolid* sol = vol->GetSolid();
  G4ThreeVector tr = g4->GetObjectTranslation();

  G4VSensitiveDetector* sd = vol->GetSensitiveDetector();
  if (!sd)  {
    return g4;
  }
  stringstream str;
  str << "G4Cnv::placement: + " << name << " No:" << node->GetNumber() << " Vol:" << vol->GetName() << " Solid:"
      << sol->GetName();
  printout(m_outputLevel, "G4Placement", str.str().c_str());
  str.str("");
  str << "                  |" << " Loc: x=" << tr.x() << " y=" << tr.y() << " z=" << tr.z();
  printout(m_outputLevel, "G4Placement", str.str().c_str());
  printout(m_outputLevel, "G4Placement", printSolid(sol).c_str());
  str.str("");
  str << "                  |" << " Ndau:" << vol->GetNoDaughters() << " physvols." << " Mat:" << vol->GetMaterial()->GetName()
      << " Mother:" << (char*) (mot ? mot->GetName().c_str() : "---");
  printout(m_outputLevel, "G4Placement", str.str().c_str());
  str.str("");
  str << "                  |" << " SD:" << sd->GetName();
  printout(m_outputLevel, "G4Placement", str.str().c_str());
  return g4;
}

template <typename O, typename C, typename F> void handleRefs(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
    //(o->*pmf)((*i)->GetName(), *i);
    (o->*pmf)("", *i);
  }
}
template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i) {
    (o->*pmf)((*i)->GetName(), *i);
  }
}

template <typename O, typename C, typename F> void handleMap(const O* o, const C& c, F pmf) {
  for (typename C::const_iterator i = c.begin(); i != c.end(); ++i)
    (o->*pmf)((*i).first, (*i).second);
}

template <typename O, typename C, typename F> void handleRMap(const O* o, const C& c, F pmf) {
  for (typename C::const_reverse_iterator i = c.rbegin(); i != c.rend(); ++i)  {
    //cout << "Handle RMAP [ " << (*i).first << " ]" << endl;
    handle(o, (*i).second, pmf);
  }
}

/// Create geometry conversion
Geant4Converter& Geant4Converter::create(DetElement top) {
  Geant4GeometryInfo& geo = this->init();
  m_data->clear();
  collect(top, geo);
  m_checkOverlaps = false;
  // We do not have to handle defines etc.
  // All positions and the like are not really named.
  // Hence, start creating the G4 objects for materials, solids and log volumes.
  Material mat = m_lcdd.material("Argon");
  handleMaterial(mat.name(), mat);
  mat = m_lcdd.material("Silicon");
  handleMaterial(mat.name(), mat);

  //m_outputLevel = WARNING;
  //setPrintLevel(VERBOSE);

  handle(this, geo.volumes, &Geant4Converter::collectVolume);
  handle(this, geo.solids, &Geant4Converter::handleSolid);
  printout(m_outputLevel, "Geant4Converter", "++ Handled %ld solids.", geo.solids.size());
  handleRefs(this, geo.vis, &Geant4Converter::handleVis);
  printout(m_outputLevel, "Geant4Converter", "++ Handled %ld visualization attributes.", geo.vis.size());
  handleMap(this, geo.limits, &Geant4Converter::handleLimitSet);
  printout(m_outputLevel, "Geant4Converter", "++ Handled %ld limit sets.", geo.limits.size());
  handleMap(this, geo.regions, &Geant4Converter::handleRegion);
  printout(m_outputLevel, "Geant4Converter", "++ Handled %ld regions.", geo.regions.size());
  handle(this, geo.volumes, &Geant4Converter::handleVolume);
  printout(m_outputLevel, "Geant4Converter", "++ Handled %ld volumes.", geo.volumes.size());
  handleRMap(this, *m_data, &Geant4Converter::handleAssembly);
  // Now place all this stuff appropriately
  handleRMap(this, *m_data, &Geant4Converter::handlePlacement);
  //==================== Fields
  handleProperties(m_lcdd.properties());

  //handleMap(this, geo.sensitives, &Geant4Converter::printSensitive);
  //handleRMap(this, *m_data, &Geant4Converter::printPlacement);

  geo.setWorld(top.placement().ptr());
  geo.valid = true;
  printout(INFO, "Geant4Converter", "+++  Successfully converted geometry to Geant4.");
  return *this;
}
