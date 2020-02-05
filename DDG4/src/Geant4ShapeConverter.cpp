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
#include "DD4hep/Shapes.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/detail/ShapesInterna.h"

#include "Geant4ShapeConverter.h"

// ROOT includes
#include "TClass.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoScaledShape.h"

// Geant4 include files
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4Cons.hh"
#include "G4Hype.hh"
#include "G4Torus.hh"
#include "G4Sphere.hh"
#include "G4CutTubs.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4Ellipsoid.hh"
#include "G4Paraboloid.hh"
#include "G4TwistedTubs.hh"
#include "G4GenericTrap.hh"
#include "G4ExtrudedSolid.hh"
#include "G4EllipticalTube.hh"

// C/C++ include files

using namespace std;
using namespace dd4hep::detail;
namespace units = dd4hep;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    static const double CM_2_MM = (CLHEP::centimeter/dd4hep::centimeter);

    /// Convert a specific TGeo shape into the geant4 equivalent
    template <typename T> G4VSolid* convertShape(const TGeoShape* shape)    {
      if ( shape )   {
        dd4hep::except("convertShape","Unsupported shape: %s",shape->IsA()->GetName());
      }
      dd4hep::except("convertShape","Invalid shape conversion requested.");
      return 0;
    }

    template <> G4VSolid* convertShape<TGeoShapeAssembly>(const TGeoShape* /* shape */)  {
      return 0;
    }

    template <> G4VSolid* convertShape<TGeoBBox>(const TGeoShape* shape)  {
      const TGeoBBox* sh = (const TGeoBBox*) shape;
      return new G4Box(sh->GetName(), sh->GetDX() * CM_2_MM, sh->GetDY() * CM_2_MM, sh->GetDZ() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TGeoTube>(const TGeoShape* shape)  {
      const TGeoTube* sh = (const TGeoTube*) shape;
      return new G4Tubs(sh->GetName(), sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM, sh->GetDz() * CM_2_MM, 0, 2. * M_PI);
    }

    template <> G4VSolid* convertShape<TGeoTubeSeg>(const TGeoShape* shape)  {
      const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
      return new G4Tubs(sh->GetName(), sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM, sh->GetDz() * CM_2_MM,
                        sh->GetPhi1() * DEGREE_2_RAD, (sh->GetPhi2()-sh->GetPhi1()) * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<TGeoCtub>(const TGeoShape* shape)  {
      const TGeoCtub* sh = (const TGeoCtub*) shape;
      const Double_t* ln = sh->GetNlow();
      const Double_t* hn = sh->GetNhigh();
      G4ThreeVector   lowNorm (ln[0], ln[1], ln[2]);
      G4ThreeVector   highNorm(hn[0], hn[1], hn[2]);
      return new G4CutTubs(sh->GetName(),
                           sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM, sh->GetDz() * CM_2_MM,
                           sh->GetPhi1() * DEGREE_2_RAD, (sh->GetPhi2()-sh->GetPhi1()) * DEGREE_2_RAD, lowNorm, highNorm);
    }

    template <> G4VSolid* convertShape<TGeoEltu>(const TGeoShape* shape)  {
      const TGeoEltu* sh = (const TGeoEltu*) shape;
      return new G4EllipticalTube(sh->GetName(),sh->GetA() * CM_2_MM, sh->GetB() * CM_2_MM, sh->GetDz() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TwistedTubeObject>(const TGeoShape* shape)  {
      const TwistedTubeObject* sh = (const TwistedTubeObject*) shape;
      return new G4TwistedTubs(sh->GetName(),sh->GetPhiTwist() * DEGREE_2_RAD,
                               sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM,
                               sh->GetNegativeEndZ() * CM_2_MM, sh->GetPositiveEndZ() * CM_2_MM,
                               sh->GetNsegments(), (sh->GetPhi2()-sh->GetPhi1()) * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<TGeoTrd1>(const TGeoShape* shape)  {
      const TGeoTrd1* sh = (const TGeoTrd1*) shape;
      return new G4Trd(sh->GetName(),
                       sh->GetDx1() * CM_2_MM, sh->GetDx2() * CM_2_MM,
                       sh->GetDy() * CM_2_MM, sh->GetDy() * CM_2_MM,
                       sh->GetDz() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TGeoTrd2>(const TGeoShape* shape)  {
      const TGeoTrd2* sh = (const TGeoTrd2*) shape;
      return new G4Trd(sh->GetName(),
                       sh->GetDx1() * CM_2_MM, sh->GetDx2() * CM_2_MM,
                       sh->GetDy1() * CM_2_MM, sh->GetDy2() * CM_2_MM,
                       sh->GetDz() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TGeoHype>(const TGeoShape* shape)  {
      const TGeoHype* sh = (const TGeoHype*) shape;
      return new G4Hype(sh->GetName(), sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM,
                        sh->GetStIn() * DEGREE_2_RAD, sh->GetStOut() * DEGREE_2_RAD,
                        sh->GetDz() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TGeoArb8>(const TGeoShape* shape)  {
      vector<G4TwoVector> vertices;
      TGeoArb8* sh = (TGeoArb8*) shape;
      Double_t* vtx_xy = sh->GetVertices();
      for ( size_t i=0; i<8; ++i, vtx_xy +=2 )
        vertices.emplace_back(vtx_xy[0] * CM_2_MM, vtx_xy[1] * CM_2_MM);
      return new G4GenericTrap(sh->GetName(), sh->GetDz() * CM_2_MM, vertices);
    }

    template <> G4VSolid* convertShape<TGeoXtru>(const TGeoShape* shape)  {
      const TGeoXtru* sh = (const TGeoXtru*) shape;
      size_t nz = sh->GetNz();
      vector<G4ExtrudedSolid::ZSection> z;
      vector<G4TwoVector> polygon;
      z.reserve(nz);
      polygon.reserve(nz);
      for(size_t i=0; i<nz; ++i)   {
        z.emplace_back(G4ExtrudedSolid::ZSection(sh->GetZ(i) * CM_2_MM, {sh->GetXOffset(i), sh->GetYOffset(i)}, sh->GetScale(i)));
        polygon.emplace_back(sh->GetX(i) * CM_2_MM,sh->GetY(i) * CM_2_MM);
      }
      return new G4ExtrudedSolid(sh->GetName(), polygon, z);
    }

    template <> G4VSolid* convertShape<TGeoPgon>(const TGeoShape* shape)  {
      const TGeoPgon* sh = (const TGeoPgon*) shape;
      vector<double> rmin, rmax, z;
      for (Int_t i = 0; i < sh->GetNz(); ++i) {
        rmin.emplace_back(sh->GetRmin(i) * CM_2_MM);
        rmax.emplace_back(sh->GetRmax(i) * CM_2_MM);
        z.emplace_back(sh->GetZ(i) * CM_2_MM);
      }
      return new G4Polyhedra(sh->GetName(), sh->GetPhi1() * DEGREE_2_RAD, sh->GetDphi() * DEGREE_2_RAD,
                             sh->GetNedges(), sh->GetNz(), &z[0], &rmin[0], &rmax[0]);
    }

    template <> G4VSolid* convertShape<TGeoPcon>(const TGeoShape* shape)  {
      const TGeoPcon* sh = (const TGeoPcon*) shape;
      vector<double> rmin, rmax, z;
      for (Int_t i = 0; i < sh->GetNz(); ++i) {
        rmin.emplace_back(sh->GetRmin(i) * CM_2_MM);
        rmax.emplace_back(sh->GetRmax(i) * CM_2_MM);
        z.emplace_back(sh->GetZ(i) * CM_2_MM);
      }
      return new G4Polycone(sh->GetName(), sh->GetPhi1() * DEGREE_2_RAD, sh->GetDphi() * DEGREE_2_RAD,
                            sh->GetNz(), &z[0], &rmin[0], &rmax[0]);
    }

    template <> G4VSolid* convertShape<TGeoCone>(const TGeoShape* shape)  {
      const TGeoCone* sh = (const TGeoCone*) shape;
      return new G4Cons(sh->GetName(), sh->GetRmin1() * CM_2_MM, sh->GetRmax1() * CM_2_MM, sh->GetRmin2() * CM_2_MM,
                        sh->GetRmax2() * CM_2_MM, sh->GetDz() * CM_2_MM, 0.0, 2.*M_PI);
    }

    template <> G4VSolid* convertShape<TGeoConeSeg>(const TGeoShape* shape)  {
      const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
      return new G4Cons(sh->GetName(), sh->GetRmin1() * CM_2_MM, sh->GetRmax1() * CM_2_MM,
                        sh->GetRmin2() * CM_2_MM, sh->GetRmax2() * CM_2_MM,
                        sh->GetDz() * CM_2_MM,
                        sh->GetPhi1() * DEGREE_2_RAD, (sh->GetPhi2()-sh->GetPhi1()) * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<TGeoParaboloid>(const TGeoShape* shape)  {
      const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
      return new G4Paraboloid(sh->GetName(), sh->GetDz() * CM_2_MM, sh->GetRlo() * CM_2_MM, sh->GetRhi() * CM_2_MM);
    }

    template <> G4VSolid* convertShape<TGeoSphere>(const TGeoShape* shape)  {
      const TGeoSphere* sh = (const TGeoSphere*) shape;
      return new G4Sphere(sh->GetName(), sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM, sh->GetPhi1() * DEGREE_2_RAD,
                          sh->GetPhi2() * DEGREE_2_RAD, sh->GetTheta1() * DEGREE_2_RAD, sh->GetTheta2() * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<TGeoTorus>(const TGeoShape* shape)  {
      const TGeoTorus* sh = (const TGeoTorus*) shape;
      return new G4Torus(sh->GetName(), sh->GetRmin() * CM_2_MM, sh->GetRmax() * CM_2_MM, sh->GetR() * CM_2_MM,
                         sh->GetPhi1() * DEGREE_2_RAD, sh->GetDphi() * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<TGeoTrap>(const TGeoShape* shape)  {
      const TGeoTrap* sh = (const TGeoTrap*) shape;
      return new G4Trap(sh->GetName(), sh->GetDz() * CM_2_MM, sh->GetTheta() * DEGREE_2_RAD, sh->GetPhi() * DEGREE_2_RAD,
                        sh->GetH1() * CM_2_MM, sh->GetBl1() * CM_2_MM, sh->GetTl1() * CM_2_MM, sh->GetAlpha1() * DEGREE_2_RAD,
                        sh->GetH2() * CM_2_MM, sh->GetBl2() * CM_2_MM, sh->GetTl2() * CM_2_MM, sh->GetAlpha2() * DEGREE_2_RAD);
    }

    template <> G4VSolid* convertShape<G4GenericTrap>(const TGeoShape* shape)  {
      vector<G4TwoVector> vertices;
      TGeoTrap* sh = (TGeoTrap*) shape;
      Double_t* vtx_xy = sh->GetVertices();
      for ( size_t i=0; i<8; ++i, vtx_xy +=2 )
        vertices.emplace_back(vtx_xy[0] * CM_2_MM, vtx_xy[1] * CM_2_MM);
      return new G4GenericTrap(sh->GetName(), sh->GetDz() * CM_2_MM, vertices);
    }
  }    // End namespace sim
}      // End namespace dd4hep

#if ROOT_VERSION_CODE > ROOT_VERSION(6,19,0)
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    template <> G4VSolid* convertShape<TGeoTessellated>(const TGeoShape* shape)  {
      TGeoTessellated*   sh  = (TGeoTessellated*) shape;
      G4TessellatedSolid* g4 = new G4TessellatedSolid(sh->GetName());
      int num_facet = sh->GetNfacets();

      printout(DEBUG,"TessellatedSolid","+++ %s> Converting %d facets", sh->GetName(), num_facet);
      for(int i=0; i<num_facet; ++i)  {
        const TGeoFacet& facet = sh->GetFacet(i);
        int nv = facet.GetNvert();
        const auto& v0 = sh->GetVertex(facet.GetVertexIndex(0));
        const auto& v1 = sh->GetVertex(facet.GetVertexIndex(1));
        const auto& v2 = sh->GetVertex(facet.GetVertexIndex(2));
        G4VFacet* g4f = 0;
        if ( nv == 3 )    {
          g4f = new G4TriangularFacet(G4ThreeVector(v0.x() * CM_2_MM, v0.y() * CM_2_MM, v0.z() * CM_2_MM),
                                      G4ThreeVector(v1.x() * CM_2_MM, v1.y() * CM_2_MM, v1.z() * CM_2_MM),
                                      G4ThreeVector(v2.x() * CM_2_MM, v2.y() * CM_2_MM, v2.z() * CM_2_MM),
                                      ABSOLUTE);
        }
        else if ( nv == 4 )    {
          const auto& v3 = sh->GetVertex(facet.GetVertexIndex(3));
          g4f = new G4QuadrangularFacet(G4ThreeVector(v0.x() * CM_2_MM, v0.y() * CM_2_MM, v0.z() * CM_2_MM),
                                        G4ThreeVector(v1.x() * CM_2_MM, v1.y() * CM_2_MM, v1.z() * CM_2_MM),
                                        G4ThreeVector(v2.x() * CM_2_MM, v2.y() * CM_2_MM, v2.z() * CM_2_MM),
                                        G4ThreeVector(v3.x() * CM_2_MM, v3.y() * CM_2_MM, v3.z() * CM_2_MM),
                                        ABSOLUTE);
        }
        else   {
          except("TGeoTessellated", "Tessellated shape [%s] has facet with wrong number of vertices: %d",
                 sh->GetName(), nv);
        }
        g4->AddFacet(g4f);
      }
      return g4;
    }
    
  }    // End namespace sim
}      // End namespace dd4hep
#endif
