// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4CONVERTER_H
#define DD4HEP_GEANT4CONVERTER_H

#include "DD4hep/GeoHandler.h"
#include "DD4hep/LCDD.h"
#include <set>
#include <map>
#include <vector>
class TGeoVolume;
class TGeoNode;

#if 0

struct G4VAny   {  virtual ~G4VAny() {}   };

template <class T> class G4AnyThing : public T {
 public:
  G4AnyThing() : T() {}
  template <class A> G4AnyThing(A) : T() {}
  template <class A,class B> G4AnyThing(A,B) : T() {}
  template <class A,class B,class C> G4AnyThing(A,B,C) : T() {}
  template <class A, class B, class C, class D> G4AnyThing(A,B,C,D) : T() {}
  template <class A, class B, class C, class D, class E> G4AnyThing(A,B,C,D,E) : T() {}
  template <class A, class B, class C, class D, class E, class F> G4AnyThing(A,B,C,D,E,F) : T() {}
  template <class A, class B, class C, class D, class E, class F, class G> G4AnyThing(A,B,C,D,E,F,G) : T() {}
  template <class A, class B, class C, class D, class E, class F, class G, class H> G4AnyThing(A,B,C,D,E,F,G,H) : T() {}
  template <class A, class B, class C, class D, class E, class F, class G, class H, class I> G4AnyThing(A,B,C,D,E,F,G,H,I) : T() {}
  void AddIsotope(void*, double) {}
  void AddElement(void*, double) {}
  static G4AnyThing<G4VAny>* GetIsotope(const std::string&, bool) { return 0; }
  static G4AnyThing<G4VAny>* GetElement(const std::string&, bool) { return 0; }
  static G4AnyThing<G4VAny>* GetMaterial(const std::string&, bool) { return 0; }
  int GetNelements() const { return 0; }
  double GetDensity() const { return 0.0; }
};

typedef G4VAny G4VSolid;
typedef G4AnyThing<G4VSolid> G4Box;
typedef G4AnyThing<G4VSolid> G4Tubs;
typedef G4AnyThing<G4VSolid> G4Trd;
typedef G4AnyThing<G4VSolid> G4Paraboloid;
typedef G4AnyThing<G4VSolid> G4Polycone;
typedef G4AnyThing<G4VSolid> G4Polyhedra;
typedef G4AnyThing<G4VSolid> G4Sphere;
typedef G4AnyThing<G4VSolid> G4Torus;
typedef G4AnyThing<G4VSolid> G4UnionSolid;
typedef G4AnyThing<G4VSolid> G4SubtractionSolid;
typedef G4AnyThing<G4VSolid> G4IntersectionSolid;

typedef G4AnyThing<G4VAny>   G4LogicalVolume;
typedef G4AnyThing<G4VAny>   G4Material;
typedef G4AnyThing<G4VAny>   G4Element;
typedef G4AnyThing<G4VAny>   G4Isotope;
typedef G4AnyThing<G4VAny>   G4Transform3D;
typedef G4AnyThing<G4VAny>   G4ThreeVector;
typedef G4AnyThing<G4VAny>   G4PVPlacement;

#else

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

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

#endif

namespace DD4hep {
  namespace Geometry {

    struct Geant4Converter : public GeoHandler  {
      struct G4GeometryInfo : public GeometryInfo {
	std::map<const TGeoElement*,G4Element*>       g4Elements;
	std::map<const TGeoMedium*, G4Material*>      g4Materials;
	std::map<const TGeoShape*,  G4VSolid*>        g4Solids;
	std::map<const TGeoVolume*, G4LogicalVolume*> g4Volumes;
	std::map<const TGeoNode*,   G4PVPlacement*>   g4Placements;
      };
      G4GeometryInfo* m_dataPtr;
      G4GeometryInfo& data() const { return *m_dataPtr; }

      /// Constructor
      Geant4Converter() {}
      /// Standard destructor
      virtual ~Geant4Converter() {}
      /// Create geometry dump
      void create(DetElement top);

      /// Dump material in GDML format to output stream
      virtual void* handleMaterial(const std::string& name, const TGeoMedium* medium) const;
      /// Dump element in GDML format to output stream
      virtual void* handleElement(const std::string& name, const TGeoElement* element) const;
      /// Dump solid in GDML format to output stream
      virtual void* handleSolid(const std::string& name, const TGeoShape* volume) const;
      /// Dump logical volume in GDML format to output stream
      virtual void* handleVolume(const std::string& name, const TGeoVolume* volume) const;
      /// Dump volume placement in GDML format to output stream
      virtual void* handlePlacement(const std::string& name, const TGeoNode* node) const;
    };
  }    // End namespace Geometry
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4CONVERTER_H
