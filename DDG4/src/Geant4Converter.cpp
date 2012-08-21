// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDG4/Geant4Field.h"
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4SensitiveDetector.h"

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
#include "Reflex/PluginService.h"
#include <iostream>
#include <iomanip>

#include "G4ProductionCuts.hh"
#include "G4VUserRegionInformation.hh"
// Geant4 include files
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Paraboloid.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4Sphere.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"
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

using namespace DD4hep::Simulation;
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

  void handleName(const TGeoNode* n)   {
    TGeoVolume* v = n->GetVolume();
    TGeoMedium* m = v->GetMedium();
    TGeoShape*  s = v->GetShape();
    string nam;
    cout << "Node:'" << n->GetName() 
	 << "' Vol:'" << v->GetName() 
	 << "' Shape:'" << s->GetName() 
	 << "' Medium:'" << m->GetName() 
	 << "'" << endl;
  }

  class G4UserRegionInformation : public G4VUserRegionInformation {
  public:
    Region  region;
    double  threshold;
    bool    storeSecondaries;
    G4UserRegionInformation() : threshold(0.0), storeSecondaries(false) {}
    virtual ~G4UserRegionInformation() {}
    virtual void Print() const {
      if ( region.isValid() ) cout << "Region:" << region.name() << endl; 
    }
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
	G4double z = element->Z();
	G4double aeff = element->A()*(g/mole);
	cout << name << " " << g/mole << " " << element->A() << " " << aeff/(g/mole)
	     << "  " << (aeff/(g/mole) - z) << endl;
	g4e = new G4Element(element->GetTitle(),name,z,aeff);
      }
    }
    data().g4Elements[element] = g4e;
  }
  return g4e;
}

/// Dump material in GDML format to output stream
void* Geant4Converter::handleMaterial(const string& name, const TGeoMedium* medium) const {
  G4Material* mat = data().g4Materials[medium];
  if ( !mat ) {
    mat = G4Material::GetMaterial(name,false);
    if ( !mat ) {
      int nElements = 0;
      const char* opt = "----";
      TGeoMaterial* m = medium->GetMaterial();
      if ( m->IsMixture() ) {
	double A_total = 0.0;
	TGeoMixture* mix = (TGeoMixture*)m;
	nElements = mix->GetNelements();
	mat = new G4Material(name,mix->GetDensity(),nElements);
	for(int i=0; i<nElements; ++i)
	  A_total += (mix->GetAmixt())[i];
	for(int i=0; i<nElements; ++i) {
	  TGeoElement*  e = mix->GetElement(i);
	  G4Element*  g4e = (G4Element*)handleElement(e->GetName(),e);
	  if ( !g4e ) {
	    cout << "ERROR: Missing component " << e->GetName() 
		 << " for material " << mix->GetName() << endl;
	  }
	  mat->AddElement(g4e,(mix->GetAmixt())[i]/A_total);
	}
	opt = "mixture";
      }
      else {
	mat = new G4Material(name,m->GetZ(),m->GetA(),m->GetDensity());
	opt = "raw_material";
      }
      cout << "Created G4 Material " << m->GetName() << " as " << opt << " " << nElements << endl;
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
      const TGeoPgon* s = (const TGeoPgon*)shape;
      double phi_start = s->GetPhi1()*DEGREE_2_RAD;
      double phi_total = (s->GetDphi()+s->GetPhi1())*DEGREE_2_RAD;
      vector<double> rmin, rmax, z;
      for( size_t i=0; i<s->GetNz(); ++i )  {
	rmin.push_back(s->GetRmin(i)*CM_2_MM);
	rmax.push_back(s->GetRmax(i)*CM_2_MM);
	z.push_back(s->GetZ(i)*CM_2_MM);
      }
      solid = new G4Polyhedra(name,phi_start,phi_total,s->GetNedges()-1,s->GetNz(),&z[0],&rmin[0],&rmax[0]);
    }
    else if ( shape->IsA() == TGeoPcon::Class() ) {
      const TGeoPcon* s = (const TGeoPcon*)shape;
      double phi_start = s->GetPhi1()*DEGREE_2_RAD;
      double phi_total = (s->GetDphi()+s->GetPhi1())*DEGREE_2_RAD;
      vector<double> rmin, rmax, z;
      for( size_t i=0; i<s->GetNz(); ++i )  {
	rmin.push_back(s->GetRmin(i)*CM_2_MM);
	rmax.push_back(s->GetRmax(i)*CM_2_MM);
	z.push_back(s->GetZ(i)*CM_2_MM);
      }
      solid = new G4Polycone(name,phi_start,phi_total,s->GetNz(),&z[0],&rmin[0],&rmax[0]);
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
  G4LogicalVolume* vol = data().g4Volumes[volume];
  if ( !vol ) {
    const TGeoVolume* v = volume;
    string      n       =  v->GetName();
    TGeoMedium* m       =  v->GetMedium();
    TGeoShape*  s       =  v->GetShape();
    G4VSolid*   solid   = (G4VSolid*)handleSolid(s->GetName(),s);
    G4Material* medium  = (G4Material*)handleMaterial(m->GetName(),m);

    if ( !solid )   {
      throw runtime_error("G4Converter: No Geant4 Solid present for volume:"+n);
    }
    if ( !medium )   {
      throw runtime_error("G4Converter: No Geant4 material present for volume:"+n);
    }
    vol = new G4LogicalVolume(solid, medium, n);
    data().g4Volumes[v] = vol;

    // Here we collect all information we need later to handle to volume internals
    Volume            _v  = Ref_t(v);
    Region            reg = _v.region();
    LimitSet          lim = _v.limitSet();
    SensitiveDetector det = _v.sensitiveDetector();

    if ( lim.isValid() ) data().limits[lim.ptr()].insert(v);
    if ( reg.isValid() ) data().regions[reg.ptr()].insert(v);
    if ( det.isValid() ) data().sensitives[det.ptr()].insert(v);
    
    //cout << "Converted logical volume [" << n << "]:" << v.ptr() << " ---> G4 " << vol << endl;
  }
  return vol;
}

/// Dump volume placement in GDML format to output stream
void* Geant4Converter::handlePlacement(const string& name, const TGeoNode* node) const {
  G4PVPlacement* g4   = data().g4Placements[node];
  if ( !g4 )   {
    TGeoMatrix* trafo = node->GetMatrix();
    if ( trafo ) {
      const Double_t*  trans = trafo->GetTranslation();
      const Double_t*  rot   = trafo->GetRotationMatrix();
      int              copy  = node->GetNumber();
      G4LogicalVolume* g4vol = data().g4Volumes[node->GetVolume()];
      G4LogicalVolume* g4mot = data().g4Volumes[node->GetMotherVolume()];
      const Value<TGeoNodeMatrix,PlacedVolume::Object>* obj = 
	dynamic_cast<const Value<TGeoNodeMatrix,PlacedVolume::Object>* >(node);
      if ( 0 == g4vol ) {
	cout << "FATAL: Unknown G4 volume:" << (void*)obj << " " << obj->GetName() << endl; 
      }
      else if ( trafo->IsRotation() )    {
	MyTransform3D transform(rot[0],rot[1],rot[2],trans[0],
				rot[3],rot[4],rot[5],trans[1],
				rot[6],rot[7],rot[8],trans[3]);
	g4 = new G4PVPlacement(transform, // no rotation
			       g4vol,     // its logical volume
			       name,      // its name
			       g4mot,     // its mother (logical) volume
			       false,     // no boolean operations
			       copy);     // its copy number
      }
      else {
	G4ThreeVector pos(trans[0],trans[1],trans[2]);
	g4 = new G4PVPlacement(0,         // no rotation
			       pos,       // translation position
			       g4vol,     // its logical volume
			       name,      // its name
			       g4mot,     // its mother (logical) volume
			       false,     // no boolean operations
			       copy);     // its copy number
      }
      data().g4Placements[node] = g4;
      //  cout << "Created volume placement:" << name << " No:" << copy << endl;
    }
  }
  else {
    cout << "Attempt to DOUBLE-place physical volume:" << name << " No:" << node->GetNumber() << endl;    
  }
  return g4;
}

/// Convert the geometry type region into the corresponding Geant4 object(s).
void* Geant4Converter::handleRegion(const TNamed* region, const set<const TGeoVolume*>& volumes) const  {
  G4Region* g4 = data().g4Regions[region];
  if ( !g4 )   {
    Region r = Ref_t(region);
    g4 = new G4Region(region->GetName());
    // set production cut
    G4ProductionCuts* cuts = new G4ProductionCuts();
    cuts->SetProductionCut(r.cut());
    g4->SetProductionCuts(cuts);

    // create region info with storeSecondaries flag
    G4UserRegionInformation* info = new G4UserRegionInformation();
    info->region = r;
    info->threshold = r.threshold();
    info->storeSecondaries = r.storeSecondaries();
    g4->SetUserInformation(info);

    vector<string>& limits = r.limits();
    for(vector<string>::const_iterator i=limits.begin(); i!=limits.end(); ++i) {
      const string& nam = *i;
      LimitSet ls = m_lcdd.limitSet(nam);
      if ( ls.isValid() ) {
	bool found = false;
	const LimitMap& lm = data().g4Limits;
	for(LimitMap::const_iterator j=lm.begin(); j!=lm.end();++j) {
	  if ( nam == (*j).first->GetName() ) {
	    g4->SetUserLimits((*j).second);
	    found = true;
	    break;
	  }
	}
	if ( found ) continue;
      }
      throw runtime_error("G4Region: Failed to resolve user limitset:"+*i);
    }
    data().g4Regions[region] = g4;    
  }
  return g4;
}

/// Convert the geometry type LimitSet into the corresponding Geant4 object(s).
void* Geant4Converter::handleLimitSet(const TNamed* limitset, const set<const TGeoVolume*>& volumes) const  {
  G4UserLimits* g4   = data().g4Limits[limitset];
  if ( !g4 )   {
    LimitSet ls = Ref_t(limitset);
    g4 = new G4UserLimits(limitset->GetName());
    const LimitSet::Object& obj = ls.limits();
    for(LimitSet::Object::const_iterator i=obj.begin(); i!=obj.end(); ++i) {
      const Limit& l = *i;
      if ( l.name == "step_length_max" )
	g4->SetMaxAllowedStep(l.value);
      else if ( l.name == "track_length_max" )
	g4->SetMaxAllowedStep(l.value);
      else if ( l.name == "time_max" )
	g4->SetUserMaxTime(l.value);
      else if ( l.name == "ekin_min" )
	g4->SetUserMinEkine(l.value);
      else if ( l.name == "range_min" )
	g4->SetUserMinRange(l.value);
      else
	throw runtime_error("Unknown Geant4 user limit: "+l.toString());
    }
    data().g4Limits[limitset] = g4;
  }
  return g4;
}

/// Convert the geometry type SensitiveDetector into the corresponding Geant4 object(s).
void* Geant4Converter::handleSensitive(const TNamed* sens_det, const set<const TGeoVolume*>& volumes) const  {
  Geant4SensitiveDetector* g4 = data().g4SensDets[sens_det];
  if ( !g4 )   {
    G4GeometryInfo& info = data();
    SensitiveDetector sd = Ref_t(sens_det);
    /// LCDD: Sensitive detector type == item tag name
    string type = sd.type(), name = sd.name();
    g4 = ROOT::Reflex::PluginService::Create<Geant4SensitiveDetector*>(type,name,&m_lcdd);
    if ( !g4 ) {
      throw runtime_error("Geant4Converter<SensitiveDetector>: FATAL Failed to "
			  "create Geant4 sensitive detector "+name+" of type "+type+".");
    }
    g4->defineCollection(sd.hitsCollection());
    ConstVolumeSet& volset = info.sensitives[sens_det];
    for(ConstVolumeSet::iterator i=volset.begin(); i!=volset.end();++i)    {
      map<const TGeoVolume*, G4LogicalVolume*>::iterator v = info.g4Volumes.find(*i);
      if ( v == info.g4Volumes.end() )  {
	throw runtime_error("Geant4Converter<SensitiveDetector>: FATAL Missing converted "
			    "Geant 4 logical volume");
      }
      (*v).second->SetSensitiveDetector(g4);
    }
    if ( sd.verbose() ) {
      cout << "Geant4Converter<SensitiveDetector> +" << setw(18) << left << sd.name() 
	   << setw(20)  << left << " ["+sd.type()+"]" 
	   << " Hits:"  << setw(16) << left << sd.hitsCollection() << endl;
      cout << "                                   | "
	   << "Cutoff:" << setw(6) << left << sd.energyCutoff()
	   << setw(5) << right << volset.size() << " volumes ";
      if ( sd.region().isValid() ) cout << " Region:" << setw(12) << left << sd.region().name();
      if ( sd.limits().isValid() ) cout << " Limits:" << setw(12) << left << sd.limits().name();
      cout << "." << endl;
    }
    data().g4SensDets[sens_det] = g4;
  }
  return g4;
}

/// Handle the geant 4 specific properties
void Geant4Converter::handleProperties(LCDD::Properties& prp)   const {
  map<string,string>  processors;
  static int s_idd = 9999999;
  string id;
  for(LCDD::Properties::const_iterator i=prp.begin(); i!=prp.end(); ++i) {
    const string& nam = (*i).first;
    const LCDD::PropertyValues& vals = (*i).second;
    if ( nam.substr(0,6) == "geant4" ) {
      LCDD::PropertyValues::const_iterator id_it = vals.find("id");
      if ( id_it != vals.end() )  {
	id= (*id_it).second;
      }
      else {
	char txt[32];
	::sprintf(txt,"%d",++s_idd);
	id = txt;
      }
      processors.insert(make_pair(id,nam));
    }
  }
  for(map<string,string>::const_iterator i=processors.begin(); i!=processors.end(); ++i) {
    const Geant4Converter* ptr = this;
    string nam = (*i).second;
    const LCDD::PropertyValues& vals = prp[nam];
    string type = vals.find("type")->second;
    string tag  = type + "_Geant4_action";
    long result = ROOT::Reflex::PluginService::Create<long>(tag,&m_lcdd,ptr,&vals);
    if ( 0 == result ) {
      throw runtime_error("Failed to locate plugin to interprete files of type"
			  " \""+tag+"\" - no factory:"+type);
    }
    result = *(long*)result;
    if ( result != 1 ) {
      throw runtime_error("Failed to invoke the plugin "+tag+" of type "+type);
    }
    cout << "+++++ Executed Successfully Geant4 setup module *" << type << "* ." << endl;
  }
}

template <typename O, typename C, typename F> void handle(const O* o, const C& c, F pmf)  {
  for(typename C::const_iterator i=c.begin(); i != c.end(); ++i) {
    (o->*pmf)((*i)->GetName(),*i);
  }
}

template <typename O, typename C, typename F> void handleMap(const O* o, const C& c, F pmf)  {
  for(typename C::const_iterator i=c.begin(); i != c.end(); ++i)
    (o->*pmf)((*i).first, (*i).second);
}

void Geant4Converter::create(DetElement top) {
  LCDD& lcdd = m_lcdd;
  G4GeometryInfo& geo = *(m_dataPtr=new G4GeometryInfo);
  m_data->clear();
  collect(top,geo);

  // We do not have to handle defines etc.
  // All positions and the like are not really named.
  // Hence, start creating the G4 objects for materials, solids and log volumes.
  handle(this, geo.materials, &Geant4Converter::handleMaterial);
  cout << "++ Handled " << geo.materials.size() << " materials." << endl;
  handle(this, geo.solids,    &Geant4Converter::handleSolid);
  cout << "++ Handled " << geo.solids.size() << " solids." << endl;
  handle(this, geo.volumes,   &Geant4Converter::handleVolume);
  cout << "++ Handled " << geo.volumes.size() << " volumes." << endl;
  //handleMap(this, lcdd.fields(),   &Geant4Converter::handleField);
  //cout << "++ Handled " << geo.fields.size() << " field entries." << endl;

  handleMap(this, geo.limits, &Geant4Converter::handleLimitSet);
  cout << "++ Handled " << geo.limits.size() << " limit sets." << endl;
  handleMap(this, geo.regions, &Geant4Converter::handleRegion);
  cout << "++ Handled " << geo.regions.size() << " regions." << endl;
  handleMap(this, geo.sensitives, &Geant4Converter::handleSensitive);
  cout << "++ Handled " << geo.sensitives.size() << " sensitive detectors." << endl;

  // Now place all this stuff appropriately
  for(Data::const_reverse_iterator i=m_data->rbegin(); i != m_data->rend(); ++i)
    handle(this, (*i).second, &Geant4Converter::handlePlacement);

  //==================== Fields
  handleProperties(m_lcdd.properties());
}
