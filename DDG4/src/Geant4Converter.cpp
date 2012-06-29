// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"
#include "Geant4Converter.h"
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
#include "TGeoNode.h"
#include "TClass.h"
#include "TMath.h"
#include <iostream>

using namespace DD4hep::Geometry;
using namespace DD4hep;
using namespace std;

namespace {
  static string indent = "";
  struct MyTransform3D : public G4Transform3D {
    MyTransform3D(double XX, double XY, double XZ, double DX,
	     double YX, double YY, double YZ, double DY,
	     double ZX, double ZY, double ZZ, double DZ) : G4Transform3D() 
    {}
    //  { G4Transform3D::setTransform(XX,XY,XZ,DX,YX,YY,YZ,DY,ZX,ZY,ZZ,DZ);  }
  };
}

/// Dump element in GDML format to output stream
void* Geant4Converter::handleElement(const string& name, const TGeoElement* element) const {
  G4Element* g4e = data().g4Elements[element];
  if ( !g4e ) {
    g4e = G4Element::GetElement(name,false);
    if ( !g4e ) {
      if ( element->GetNisotopes() > 1 ) {
	g4e = new G4Element(name,element->GetTitle(),element->GetNisotopes());
	for(int i=0, n=element->GetNisotopes(); i<n; ++i) {
	  TGeoIsotope*  iso = element->GetIsotope(i);
	  G4Isotope*  g4iso = G4Isotope::GetIsotope(iso->GetName(),false);
	  if ( !g4iso ) {
	    g4iso = new G4Isotope(iso->GetName(),iso->GetZ(),iso->GetN(),iso->GetA());
	  }
	  g4e->AddIsotope(g4iso,element->GetRelativeAbundance(i));
	}
      }
      else {
	g4e = new G4Element(name,element->GetTitle(),element->A(),element->Z());
      }
    }
    data().g4Elements[element] = g4e;
  }
  return g4e;
}

/// Dump material in GDML format to output stream
void* Geant4Converter::handleMaterial(const std::string& name, const TGeoMedium* medium) const {
  G4Material* mat = data().g4Materials[medium];
  if ( !mat ) {
    mat = G4Material::GetMaterial(name,false);
    if ( !mat ) {
      TGeoMaterial* m = medium->GetMaterial();
      if ( m->IsMixture() ) {
	TGeoMixture* mix = (TGeoMixture*)m;
	mat = new G4Material(name,mix->GetDensity(),mix->GetNelements());
	for(int i=0, n=mix->GetNelements(); i<n; ++i) {
	  TGeoElement*  e = mix->GetElement(i);
	  G4Element*  g4e = (G4Element*)handleElement(e->GetName(),e);
	  mat->AddElement(g4e,(mix->GetWmixt())[i]);
	}
      }
      else {
	mat = new G4Material(name,m->GetZ(),m->GetA(),m->GetDensity());
      }
    }
    data().g4Materials[medium] = mat;
  }
  return mat;
}

/// Dump solid in GDML format to output stream
void* Geant4Converter::handleSolid(const string& name, const TGeoShape* shape)   const   {
  G4VSolid* solid = data().g4Solids[shape];
  if ( !solid && shape ) {
    if ( shape->IsA() == TGeoBBox::Class() ) {
      const TGeoBBox* s = (const TGeoBBox*)shape;
      solid = new G4Box(name,s->GetDX()*CM_2_MM,s->GetDY()*CM_2_MM,s->GetDZ()*CM_2_MM);
    }
    else if ( shape->IsA() == TGeoTube::Class() ) {
      const TGeoTube* s = (const TGeoTube*)shape;
      solid = new G4Tubs(name,s->GetRmin()*CM_2_MM,s->GetRmax()*CM_2_MM,s->GetDz()*CM_2_MM,0,2.*M_PI);
    }
    else if ( shape->IsA() == TGeoTubeSeg::Class() ) {
      const TGeoTubeSeg* s = (const TGeoTubeSeg*)shape;
      solid = new G4Tubs(name,s->GetRmin()*CM_2_MM,s->GetRmax()*CM_2_MM,s->GetDz()*CM_2_MM,s->GetPhi1()*DEGREE_2_RAD,s->GetPhi2()*DEGREE_2_RAD);
    }
    else if ( shape->IsA() == TGeoTrd1::Class() ) {
      const TGeoTrd1* s = (const TGeoTrd1*)shape;
      solid = new G4Trd(name,s->GetDx1()*CM_2_MM,s->GetDx2()*CM_2_MM,s->GetDy()*CM_2_MM,s->GetDy()*CM_2_MM,s->GetDz()*CM_2_MM);
    }
    else if ( shape->IsA() == TGeoTrd2::Class() ) {
      const TGeoTrd2* s = (const TGeoTrd2*)shape;
      solid = new G4Trd(name,s->GetDx1()*CM_2_MM,s->GetDx2()*CM_2_MM,s->GetDy1()*CM_2_MM,s->GetDy2()*CM_2_MM,s->GetDz()*CM_2_MM);
    }
    else if ( shape->IsA() == TGeoPgon::Class() ) {
#if 0
      const TGeoPgon* s = (const TGeoPgon*)shape;
#endif
      const TGeoPcon* s = (const TGeoPcon*)shape;
      vector<double> rmin, rmax, z;
      for( size_t i=0; i<s->GetNz(); ++i )  {
	rmin.push_back(s->GetRmin(i)*CM_2_MM);
	rmax.push_back(s->GetRmax(i)*CM_2_MM);
	z.push_back(s->GetZ(i)*CM_2_MM);
      }
      solid = new G4Polycone(name,s->GetPhi1()*DEGREE_2_RAD,(s->GetDphi()+s->GetPhi1())*DEGREE_2_RAD,s->GetNz(),
			     &rmin[0], &rmax[0], &z[0]);
    }
    else if ( shape->IsA() == TGeoPcon::Class() ) {
      const TGeoPcon* s = (const TGeoPcon*)shape;
      vector<double> rmin, rmax, z;
      for( size_t i=0; i<s->GetNz(); ++i )  {
	rmin.push_back(s->GetRmin(i)*CM_2_MM);
	rmax.push_back(s->GetRmax(i)*CM_2_MM);
	z.push_back(s->GetZ(i)*CM_2_MM);
      }
      solid = new G4Polycone(name,s->GetPhi1()*DEGREE_2_RAD,(s->GetDphi()+s->GetPhi1())*DEGREE_2_RAD,s->GetNz(),
			     &rmin[0], &rmax[0], &z[0]);
    }
    else if ( shape->IsA() == TGeoParaboloid::Class() ) {
      const TGeoParaboloid* s = (const TGeoParaboloid*)shape;
      solid = new G4Paraboloid(name,s->GetDz()*CM_2_MM,s->GetRlo()*CM_2_MM,s->GetRhi()*CM_2_MM);
    }
    else if ( shape->IsA() == TGeoSphere::Class() ) {
      const TGeoSphere* s = (const TGeoSphere*)shape;
      solid = new G4Sphere(name,s->GetRmin()*CM_2_MM,s->GetRmax()*CM_2_MM,
			   s->GetPhi1()*DEGREE_2_RAD,s->GetPhi2()*DEGREE_2_RAD,
			   s->GetTheta1()*DEGREE_2_RAD,s->GetTheta2()*DEGREE_2_RAD);
    }
    else if ( shape->IsA() == TGeoTorus::Class() ) {
      const TGeoTorus* s = (const TGeoTorus*)shape;
      solid = new G4Torus(name,s->GetRmin()*CM_2_MM,s->GetRmax()*CM_2_MM, s->GetR()*CM_2_MM,
			  s->GetPhi1()*DEGREE_2_RAD,s->GetDphi()*DEGREE_2_RAD);
    }
    else if ( shape->IsA() == TGeoCompositeShape::Class() ) {
      const TGeoCompositeShape* s = (const TGeoCompositeShape*)shape;
      const TGeoBoolNode* boolean = s->GetBoolNode();
      TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
      TGeoMatrix* m     = boolean->GetRightMatrix();
      G4VSolid* left    = (G4VSolid*)handleSolid(name+"_left", boolean->GetLeftShape());
      G4VSolid* right   = (G4VSolid*)handleSolid(name+"_right",boolean->GetRightShape());
      const Double_t *t = m->GetTranslation();
      const Double_t *r = m->GetRotationMatrix();
      
      if ( !left )   {
	throw runtime_error("G4Converter: No left Geant4 Solid present for composite shape:"+name);
      }
      if ( !right )   {
	throw runtime_error("G4Converter: No right Geant4 Solid present for composite shape:"+name);
      }

      if ( m->IsRotation()    )   {
	MyTransform3D transform(r[0],r[1],r[2],t[0],
				r[3],r[4],r[5],t[1],
				r[6],r[7],r[8],t[3]);
	if (      oper == TGeoBoolNode::kGeoSubtraction )
	  solid = new G4SubtractionSolid(name,left,right,transform);
	else if ( oper == TGeoBoolNode::kGeoUnion )
	  solid = new G4UnionSolid(name,left,right,transform);
	else if ( oper == TGeoBoolNode::kGeoIntersection )
	  solid = new G4IntersectionSolid(name,left,right,transform);
      }
      else {
	G4ThreeVector transform(t[0],t[1],t[2]);
	if (      oper == TGeoBoolNode::kGeoSubtraction )
	  solid = new G4SubtractionSolid(name,left,right,0,transform);
	else if ( oper == TGeoBoolNode::kGeoUnion )
	  solid = new G4UnionSolid(name,left,right,0,transform);
	else if ( oper == TGeoBoolNode::kGeoIntersection )
	  solid = new G4IntersectionSolid(name,left,right,0,transform);
      }
    }

    if ( !solid ) {
      string err = "Failed to handle unknown solid shape:" + 
	name + " of type " + string(shape->IsA()->GetName());
      throw runtime_error(err);
    }
    data().g4Solids[shape] = solid;
  }
  return solid;
}

/// Dump logical volume in GDML format to output stream
void* Geant4Converter::handleVolume(const string& name, const TGeoVolume* volume)   const   {
  const TGeoVolume* v =  volume;
  G4LogicalVolume* vol = data().g4Volumes[v];
  if ( !vol ) {
    string      n =  v->GetName();
    TGeoMedium* m =  v->GetMedium();
    TGeoShape*  s =  v->GetShape();
    G4VSolid*   solid  = (G4VSolid*)handleSolid(s->GetName(),s);
    G4Material* medium = (G4Material*)handleMaterial(m->GetName(),m);

    if ( !solid )   {
      throw runtime_error("G4Converter: No Geant4 Solid present for volume:"+n);
    }
    if ( !medium )   {
      throw runtime_error("G4Converter: No Geant4 material present for volume:"+n);
    }
    vol = new G4LogicalVolume(solid, medium, n);
    data().g4Volumes[v] = vol;
  }
  return vol;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handlePlacement(const std::string& name, const TGeoNode* node) const {
  G4PVPlacement*     g4pv   = data().g4Placements[node];
  if ( !g4pv )   {
    TGeoMatrix*        trafo  = node->GetMatrix();
    if ( trafo ) {
      const Double_t*    trans  = trafo->GetTranslation();
      const Double_t*    rot    = trafo->GetRotationMatrix();
      int                copy   = node->GetNumber();
      G4LogicalVolume*   g4vol  = data().g4Volumes[node->GetVolume()];
      G4LogicalVolume*   g4mot  = data().g4Volumes[node->GetMotherVolume()];

      if ( trafo->IsRotation() )    {
	MyTransform3D transform(rot[0],rot[1],rot[2],trans[0],
				rot[3],rot[4],rot[5],trans[1],
				rot[6],rot[7],rot[8],trans[3]);
	g4pv = new G4PVPlacement(transform, // no rotation
				 g4vol,     // its logical volume
				 name,      // its name
				 g4mot,     // its mother (logical) volume
				 false,     // no boolean operations
				 copy);     // its copy number
      }
      else {
	G4ThreeVector pos(trans[0],trans[1],trans[2]);
	g4pv = new G4PVPlacement(0,         // no rotation
				 pos,       // translation position
				 g4vol,     // its logical volume
				 name,      // its name
				 g4mot,     // its mother (logical) volume
				 false,     // no boolean operations
				 copy);     // its copy number
      }
      data().g4Placements[node] = g4pv;
      //  cout << "Created volume placement:" << name << " No:" << copy << endl;
    }
  }
  else {
    cout << "Attempt to DOUBLE-place physical volume:" << name << " No:" << node->GetNumber() << endl;    
  }
  return g4pv;
}

template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf)  {
  for(typename C::const_iterator i=c.begin(); i != c.end(); ++i) {
    (o->*pmf)((*i).first, (*i).second);
  }
}

static void handleName(const TGeoNode* n)   {
  TGeoVolume* v = n->GetVolume();
  TGeoMedium* m = v->GetMedium();
  TGeoShape*  s = v->GetShape();
  string nam;
  cout << "Node:'" << n->GetName() << "' Vol:'" << v->GetName() << "' Shape:'" << s->GetName() << "' Medium:'" << m->GetName() << "'" << endl;
}

void Geant4Converter::create(DetElement top) {
  G4GeometryInfo geo;
  m_dataPtr = &geo;
  m_data->clear();
  collect(top,geo);

  // We do not have to handle defines etc.
  // All positions and the like are not really named.
  // Hence, start creating the G4 objects for materials, solids and log volumes.
  handle(this, geo.materials, &Geant4Converter::handleMaterial);
  handle(this, geo.solids,    &Geant4Converter::handleSolid);
  handle(this, geo.volumes,   &Geant4Converter::handleVolume);
  // Now place all this stuff appropriately
  for(Data::const_reverse_iterator i=m_data->rbegin(); i != m_data->rend(); ++i)   {
    const Data::mapped_type& v = (*i).second;
    for(Data::mapped_type::const_iterator j=v.begin(); j != v.end(); ++j) {
      handlePlacement((*j)->GetName(),*j);
      handleName(*j);
    }
  }
}
