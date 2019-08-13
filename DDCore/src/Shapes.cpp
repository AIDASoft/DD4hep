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

#define _USE_MATH_DEFINES

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>
#include <iomanip>
#include <sstream>

// ROOT includes
#include "TClass.h"
#include "TGeoMatrix.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"

using namespace std;
using namespace dd4hep;
namespace units = dd4hep;

#define TRUNCATEDTUBE_TAG "TruncatedTube"
#define PSEUDOTRAP_TAG    "PseudoTrap"
#define UNION_TAG         "Union"
#define SUBTRACTION_TAG   "Subtraction"
#define INTERSECTION_TAG  "Intersection"

namespace dd4hep {
  static bool check_shape_type(const Handle<TGeoShape>& solid, const TClass* cl)   {
    if ( solid.isValid() )   {
      return solid->IsA() == cl;
    }
    return false;
  }

  /// Type check of various shapes.
  template <typename SOLID> bool instanceOf(const Handle<TGeoShape>& solid)   {
    if ( solid.isValid() )   {
      return solid->IsA() == SOLID::Object::Class();
    }
    return false;
  }
  template bool instanceOf<Box>(const Handle<TGeoShape>& solid);
  template bool instanceOf<ShapelessSolid>(const Handle<TGeoShape>& solid);
  template bool instanceOf<HalfSpace>(const Handle<TGeoShape>& solid);
  template bool instanceOf<ConeSegment>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Tube>(const Handle<TGeoShape>& solid);
  template bool instanceOf<CutTube>(const Handle<TGeoShape>& solid);
  template bool instanceOf<EllipticalTube>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Cone>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Trap>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Trd1>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Trd2>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Torus>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Sphere>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Paraboloid>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Hyperboloid>(const Handle<TGeoShape>& solid);
  template bool instanceOf<PolyhedraRegular>(const Handle<TGeoShape>& solid);
  template bool instanceOf<Polyhedra>(const Handle<TGeoShape>& solid);
  template bool instanceOf<ExtrudedPolygon>(const Handle<TGeoShape>& solid);
  template bool instanceOf<BooleanSolid>(const Handle<TGeoShape>& solid);
  template <> bool instanceOf<Polycone>(const Handle<TGeoShape>& solid)   {
    return check_shape_type(solid, TGeoPcon::Class())
      ||   check_shape_type(solid, TGeoPgon::Class());
  }
  template <> bool instanceOf<EightPointSolid>(const Handle<TGeoShape>& solid)   {
    if ( solid.isValid() )   {
      TClass* c = solid->IsA();
      return c==TGeoTrap::Class() || c==TGeoArb8::Class() || c==TGeoGtra::Class();
    }
    return false;
  }
  template <> bool instanceOf<TruncatedTube>(const Handle<TGeoShape>& solid)   {
    if ( solid.isValid() )   {
      return solid->IsA() == TGeoCompositeShape::Class()
        &&   ::strcmp(solid->GetTitle(), TRUNCATEDTUBE_TAG) == 0;
    }
    return false;
  }
  template <> bool instanceOf<PseudoTrap>(const Handle<TGeoShape>& solid)   {
    if ( solid.isValid() )   {
      return solid->IsA() == TGeoCompositeShape::Class()
        &&   ::strcmp(solid->GetTitle(), PSEUDOTRAP_TAG) == 0;
    }
    return false;
  }
  template <> bool instanceOf<SubtractionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoSubtraction;
  }
  template <> bool instanceOf<UnionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoUnion;
  }
  template <> bool instanceOf<IntersectionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoIntersection;
  }
}

namespace dd4hep {
  template <typename SHAPE> void invalidSetDimensionCall(const SHAPE& sh, const vector<double>& params)   {
    stringstream str;
    str << "Shape: " << typeName(typeid(sh)) << "::setDimension: "
        << "Invalid number of parameters: " << params.size();
    throw runtime_error(str.str());
  }

  /// Access Shape dimension parameters (As in TGeo, but angles in radians rather than degrees)
  std::vector<double> get_shape_dimensions(TGeoShape* shape)   {
    if (shape) {
      TClass* cl = shape->IsA();
      if (cl == TGeoShapeAssembly::Class()) {
        const TGeoShapeAssembly* sh = (const TGeoShapeAssembly*) shape;
        return { sh->GetDX(), sh->GetDY(), sh->GetDZ() };
      }
      else if (cl == TGeoBBox::Class()) {
        const TGeoBBox* sh = (const TGeoBBox*) shape;
        return { sh->GetDX(), sh->GetDY(), sh->GetDZ() };
      }
      else if (cl == TGeoHalfSpace::Class()) {
        TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
        return { sh->GetPoint()[0], sh->GetPoint()[1], sh->GetPoint()[2],
            sh->GetNorm()[0], sh->GetNorm()[1], sh->GetNorm()[2] };
      }
      else if (cl == TGeoPcon::Class()) {
        const TGeoPcon* sh = (const TGeoPcon*) shape;
        vector<double> pars { sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg, double(sh->GetNz()) };
        pars.reserve(3+3*sh->GetNz());
        for (Int_t i = 0; i < sh->GetNz(); ++i) {
          pars.emplace_back(sh->GetZ(i));
          pars.emplace_back(sh->GetRmin(i));
          pars.emplace_back(sh->GetRmax(i));
        }
        return pars;
      }
      else if (cl == TGeoConeSeg::Class()) {
        const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
        return { sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(), sh->GetRmin2(), sh->GetRmax2(),
            sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg };
      }
      else if (cl == TGeoCone::Class()) {
        const TGeoCone* sh = (const TGeoCone*) shape;
        return { sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(), sh->GetRmin2(), sh->GetRmax2() };
      }
      else if (cl == TGeoTube::Class()) {
        const TGeoTube* sh = (const TGeoTube*) shape;
        return { sh->GetRmin(), sh->GetRmax(), sh->GetDz() };
      }
      else if (cl == TGeoTubeSeg::Class()) {
        const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
        return { sh->GetRmin(), sh->GetRmax(), sh->GetDz(), sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg };
      }
      else if (cl == TGeoCtub::Class()) {
        const TGeoCtub* sh = (const TGeoCtub*) shape;
        const Double_t*	lo = sh->GetNlow();
        const Double_t*	hi = sh->GetNhigh();
        return { sh->GetRmin(), sh->GetRmax(), sh->GetDz(),
            sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg,
            lo[0], lo[1], lo[2], hi[0], hi[1], hi[2] };
      }
      else if (cl == TGeoEltu::Class()) {
        const TGeoEltu* sh = (const TGeoEltu*) shape;
        return { sh->GetA(), sh->GetB(), sh->GetDz() };
      }
      else if (cl == TGeoTrd1::Class()) {
        const TGeoTrd1* sh = (const TGeoTrd1*) shape;
        return { sh->GetDx1(), sh->GetDx2(), sh->GetDy(), sh->GetDz() };
      }
      else if (cl == TGeoTrd2::Class()) {
        const TGeoTrd2* sh = (const TGeoTrd2*) shape;
        return { sh->GetDx1(), sh->GetDx2(), sh->GetDy1(), sh->GetDy2(), sh->GetDz() };
      }
      else if (cl == TGeoParaboloid::Class()) {
        const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
        return { sh->GetRlo(), sh->GetRhi(), sh->GetDz() };
      }
      else if (cl == TGeoHype::Class()) {
        const TGeoHype* sh = (const TGeoHype*) shape;
        return { sh->GetDz(),
            sh->GetRmin(), sh->GetStIn()*units::deg,
            sh->GetRmax(), sh->GetStOut()*units::deg };
      }
      else if (cl == TGeoSphere::Class()) {
        const TGeoSphere* sh = (const TGeoSphere*) shape;
        return { sh->GetRmin(), sh->GetRmax(),
            sh->GetTheta1()*units::deg, sh->GetTheta2()*units::deg,
            sh->GetPhi1()*units::deg,   sh->GetPhi2()*units::deg };
      }
      else if (cl == TGeoTorus::Class()) {
        const TGeoTorus* sh = (const TGeoTorus*) shape;
        return { sh->GetR(), sh->GetRmin(), sh->GetRmax(),
            sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg };
      }
      else if (cl == TGeoTrap::Class()) {
        const TGeoTrap* sh = (const TGeoTrap*) shape;
        return { sh->GetDz(), sh->GetTheta()*units::deg, sh->GetPhi()*units::deg,
            sh->GetH1(), sh->GetBl1(), sh->GetTl1(), sh->GetAlpha1()*units::deg,
            sh->GetH2(), sh->GetBl2(), sh->GetTl2(), sh->GetAlpha2()*units::deg };
      }
      else if (cl == TGeoPgon::Class()) {
        const TGeoPgon* sh = (const TGeoPgon*) shape;
        vector<double> pars { sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg, double(sh->GetNedges()), double(sh->GetNz()) };
        pars.reserve(4+3*sh->GetNz());
        for (Int_t i = 0; i < sh->GetNz(); ++i) {
          pars.emplace_back(sh->GetZ(i));
          pars.emplace_back(sh->GetRmin(i));
          pars.emplace_back(sh->GetRmax(i));
        }
        return pars;
      }
      else if (cl == TGeoXtru::Class()) {
        const TGeoXtru* sh = (const TGeoXtru*) shape;
        Int_t nz = sh->GetNz();
        vector<double> pars { double(nz) };
        pars.reserve(1+4*nz);
        for(Int_t i=0; i<nz; ++i)   {
          pars.emplace_back(sh->GetZ(i));
          pars.emplace_back(sh->GetXOffset(i));
          pars.emplace_back(sh->GetYOffset(i));
          pars.emplace_back(sh->GetScale(i));
        }
        return pars;
      }
      else if (cl == TGeoArb8::Class())  {
        TGeoTrap* sh = (TGeoTrap*) shape;
        struct _V { double xy[8][2]; } *vtx = (_V*)sh->GetVertices();
        vector<double> pars { sh->GetDz() };
        for ( size_t i=0; i<8; ++i )   {
          pars.emplace_back(vtx->xy[i][0]);
          pars.emplace_back(vtx->xy[i][1]);
        }
#if 0
        const Double_t* vertices = sh->GetVertices();
        vector<double> pars { sh->GetDz() };
        pars.reserve(17);
        for ( size_t i=0; i<8; ++i )
          pars.emplace_back(vertices[i*2]);
        for ( size_t i=0; i<8; ++i )
          pars.emplace_back(vertices[i*2]+1);
#endif
        return pars;
      }
      else if (cl == TGeoCompositeShape::Class()) {
        const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
        const TGeoBoolNode* boolean = sh->GetBoolNode();
        TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
        TGeoMatrix* left_matrix   = boolean->GetRightMatrix();
        TGeoMatrix* right_matrix  = boolean->GetRightMatrix();
        TGeoShape*  left_solid    = boolean->GetLeftShape();
        TGeoShape*  right_solid   = boolean->GetLeftShape();
        const Double_t* left_tr   = left_matrix->GetTranslation();
        const Double_t* left_rot  = left_matrix->GetRotationMatrix();
        const Double_t* right_tr  = right_matrix->GetTranslation();
        const Double_t* right_rot = right_matrix->GetRotationMatrix();

        vector<double> pars { double(oper) };
        vector<double> left_par  = Solid(left_solid).dimensions();
        vector<double> right_par = Solid(right_solid).dimensions();

        pars.insert(pars.end(), left_par.begin(), left_par.end());
        pars.insert(pars.end(), left_rot, left_rot+9);
        pars.insert(pars.end(), left_tr, left_tr+3);

        pars.insert(pars.end(), right_par.begin(), right_par.end());
        pars.insert(pars.end(), right_rot, right_rot+9);
        pars.insert(pars.end(), right_tr, right_tr+3);
        return pars;
      }
      else  {
        printout(ERROR,"Solid","Failed to access dimensions for shape of type:%s.",
                 cl->GetName());
      }
      return {};
    }
    except("Solid","Failed to access dimensions [Invalid handle].");
   return {};
  }

  /// Access Shape dimension parameters (As in TGeo, but angles in radians rather than degrees)
  std::vector<double> get_shape_dimensions(Solid solid)   {
    return get_shape_dimensions(solid.ptr());
  }
  
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  void set_shape_dimensions(TGeoShape* shape, const std::vector<double>& params)   {
    if (shape) {
      TClass* cl = shape->IsA();
      Solid solid(shape);
      if (cl == TGeoShapeAssembly::Class()) {
        //TGeoShapeAssembly* sh = (TGeoShapeAssembly*) shape;
        printout(WARNING,"ShapelessSolid","+++ setDimensions is a compatibility call. Nothing implemented.");
      }
      else if (cl == TGeoBBox::Class()) {
        TGeoBBox* sh = (TGeoBBox*) shape;
        auto pars = params;
        if ( params.size() != 3 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoHalfSpace::Class()) {
        TGeoHalfSpace* sh = (TGeoHalfSpace*)shape;
        auto pars = params;
        if ( params.size() != 6 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoPcon::Class()) {
        TGeoPcon* sh = (TGeoPcon*) shape;
        if ( params.size() < 3 )   {
          invalidSetDimensionCall(*sh,params);
        }
        size_t nz = size_t(params[2]);
        if ( params.size() != 3 + 3*nz )   {
          invalidSetDimensionCall(*sh,params);
        }
        vector<double> pars = params;
        pars[0] /= units::deg;
        pars[1] /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoConeSeg::Class()) {
        TGeoConeSeg* sh = (TGeoConeSeg*) shape;
        if ( params.size() != 7 )   {
          invalidSetDimensionCall(*sh,params);
        }
        auto pars = params;
        pars[5] /= units::deg;
        pars[6] /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoCone::Class()) {
        TGeoCone* sh = (TGeoCone*) shape;
        auto pars = params;
        if ( params.size() != 5 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoTube::Class()) {
        TGeoTube* sh = (TGeoTube*) shape;
        if ( params.size() != 3 )   {
          invalidSetDimensionCall(*sh,params);
        }
        auto pars = params;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoTubeSeg::Class()) {
        TGeoTubeSeg* sh = (TGeoTubeSeg*) shape;
        if ( params.size() != 5 )   {
          invalidSetDimensionCall(*sh,params);
        }
        auto pars = params;
        pars[3] /= units::deg;
        pars[4] /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoCtub::Class()) {
        TGeoCtub* sh = (TGeoCtub*) shape;
        if ( params.size() != 11 )   {
          invalidSetDimensionCall(*sh,params);
        }
        auto pars = params;
        pars[3] /= units::deg;
        pars[4] /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoEltu::Class()) {
        TGeoEltu* sh = (TGeoEltu*) shape;
        auto pars = params;
        if ( params.size() != 3 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoTrd1::Class()) {
        TGeoTrd1* sh = (TGeoTrd1*) shape;
        auto pars = params;
        if ( params.size() != 4 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoTrd2::Class()) {
        TGeoTrd2* sh = (TGeoTrd2*) shape;
        auto pars = params;
        if ( params.size() != 5 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoParaboloid::Class()) {
        TGeoParaboloid* sh = (TGeoParaboloid*) shape;
        auto pars = params;
        if ( params.size() != 3 )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoHype::Class()) {
        TGeoHype* sh = (TGeoHype*) shape;
        if ( params.size() != 5 )   {
          invalidSetDimensionCall(*sh,params);
        }
        double pars[] = { params[0], params[1], params[2]/units::deg, params[3], params[4]/units::deg };
        solid._setDimensions(pars);
      }
      else if (cl == TGeoSphere::Class()) {
        TGeoSphere* sh = (TGeoSphere*) shape;
        if ( params.size() < 2 )   {
          invalidSetDimensionCall(*sh,params);
        }
        double pars[] = { params[0], params[1], 0.0, M_PI/units::deg, 0.0, 2*M_PI/units::deg };
        if (params.size() > 2) pars[2] = params[2]/units::deg;
        if (params.size() > 3) pars[3] = params[3]/units::deg;
        if (params.size() > 4) pars[4] = params[4]/units::deg;
        if (params.size() > 5) pars[5] = params[5]/units::deg;
        sh->SetDimensions(pars, params.size());
        sh->ComputeBBox();
      }
      else if (cl == TGeoTorus::Class()) {
        TGeoTorus* sh = (TGeoTorus*) shape;
        if ( params.size() != 5 )   {
          invalidSetDimensionCall(*sh,params);
        }
        double pars[] = { params[0], params[1], params[2], params[3]/units::deg, params[4]/units::deg };
        solid._setDimensions(pars);
      }
      else if (cl == TGeoTrap::Class()) {
        TGeoTrap* sh = (TGeoTrap*) shape;
        auto pars = params;
        if ( params.size() != 11 )   {
          invalidSetDimensionCall(*sh,params);
        }
        pars[1]  /= units::deg;
        pars[2]  /= units::deg;
        pars[6]  /= units::deg;
        pars[10] /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoPgon::Class()) {
        TGeoPgon* sh = (TGeoPgon*) shape;
        auto pars = params;
        if ( params.size() != 3 + 3*size_t(params[2]) )   {
          invalidSetDimensionCall(*sh,params);
        }
        pars[0]  /= units::deg;
        pars[1]  /= units::deg;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoXtru::Class()) {
        TGeoXtru* sh = (TGeoXtru*) shape;
        auto pars = params;
        if ( params.size() != 1 + 4*size_t(params[0]) )   {
          invalidSetDimensionCall(*sh,params);
        }
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoArb8::Class())  {
        TGeoTrap* sh = (TGeoTrap*) shape;
        if ( params.size() != 17 )   {
          invalidSetDimensionCall(*sh,params);
        }
        auto pars = params;
        solid._setDimensions(&pars[0]);
      }
      else if (cl == TGeoCompositeShape::Class()) {
        //TGeoCompositeShape* sh = (TGeoCompositeShape*) shape;
        if ( instanceOf<TruncatedTube>(solid) )   {
        }
        else if ( instanceOf<PseudoTrap>(solid) )   {
        }
        else if ( instanceOf<SubtractionSolid>(solid) )   {
        }
        else if ( instanceOf<UnionSolid>(solid) )   {
        }
        else if ( instanceOf<IntersectionSolid>(solid) )   {
        }
        throw runtime_error("Composite shape. setDimensions is not implemented!");
      }
      else  {
        printout(ERROR,"Solid","Failed to access dimensions for shape of type:%s.",
                 cl->GetName());
      }
      return;
    }
    except("Solid","set_shape_dimensions: Failed to set dimensions [Invalid handle].");
  }
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  void set_shape_dimensions(Solid solid, const std::vector<double>& params)   {
    set_shape_dimensions(solid.ptr(), params);
  }
}

/// Pretty print of solid attributes
string dd4hep::toStringSolid(const TGeoShape* shape, int precision)   {
  stringstream log;

  if ( !shape )  {
    return "[Invalid shape]";
  }

  TClass* cl = shape->IsA();
  int prec = log.precision();
  log.setf(ios::fixed,ios::floatfield);
  log << setprecision(precision);
  log << cl->GetName();
  if ( cl == TGeoBBox::Class() )   {
    TGeoBBox* sh = (TGeoBBox*) shape;
    log << " x:" << sh->GetDX()
        << " y:" << sh->GetDY()
        << " z:" << sh->GetDZ();
  }
  else if (cl == TGeoHalfSpace::Class()) {
    TGeoHalfSpace* sh = (TGeoHalfSpace*)(const_cast<TGeoShape*>(shape));
    log << " Point:  (" << sh->GetPoint()[0] << ", " << sh->GetPoint()[1] << ", " << sh->GetPoint()[2] << ") " 
        << " Normal: (" << sh->GetNorm()[0]  << ", " << sh->GetNorm()[1]  << ", " << sh->GetNorm()[2]  << ") ";
  }
  else if (cl == TGeoTube::Class()) {
    const TGeoTube* sh = (const TGeoTube*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz();
  }
  else if (cl == TGeoTubeSeg::Class()) {
    const TGeoTubeSeg* sh = (const TGeoTubeSeg*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2();
  }
  else if (cl == TGeoCtub::Class()) {
    const TGeoCtub* sh = (const TGeoCtub*) shape;
    const Double_t*	hi = sh->GetNhigh();
    const Double_t*	lo = sh->GetNlow();
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " dz:" << sh->GetDz()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2()
        << " lx:" << lo[0] << " ly:" << lo[1] << " lz:" << lo[2]
        << " hx:" << hi[0] << " hy:" << hi[1] << " hz:" << hi[2];
  }
  else if (cl == TGeoEltu::Class()) {
    const TGeoEltu* sh = (const TGeoEltu*) shape;
    log << " A:" << sh->GetA() << " B:" << sh->GetB() << " dz:" << sh->GetDz();
  }
  else if (cl == TGeoTrd1::Class()) {
    const TGeoTrd1* sh = (const TGeoTrd1*) shape;
    log << " x1:" << sh->GetDx1() << " x2:" << sh->GetDx2() << " y:" << sh->GetDy() << " z:" << sh->GetDz();
  }
  else if (cl == TGeoTrd2::Class()) {
    const TGeoTrd2* sh = (const TGeoTrd2*) shape;
    log << " x1:" << sh->GetDx1() << " x2:" << sh->GetDx2()
        << " y1:" << sh->GetDy1() << " y2:" << sh->GetDy2() << " z:" << sh->GetDz();
  }
  else if (cl == TGeoTrap::Class()) {
    const TGeoTrap* sh = (const TGeoTrap*) shape;
    log << " dz:" << sh->GetDz() << " Theta:" << sh->GetTheta() << " Phi:" << sh->GetPhi()
        << " H1:" << sh->GetH1() << " Bl1:"   << sh->GetBl1()   << " Tl1:" << sh->GetTl1() << " Alpha1:" << sh->GetAlpha1()
        << " H2:" << sh->GetH2() << " Bl2:"   << sh->GetBl2()   << " Tl2:" << sh->GetTl2() << " Alpha2:" << sh->GetAlpha2();
  }
  else if (cl == TGeoHype::Class()) {
    const TGeoHype* sh = (const TGeoHype*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:"  << sh->GetRmax() << " dz:" << sh->GetDz()
        << " StIn:" << sh->GetStIn() << " StOut:" << sh->GetStOut();
  }
  else if (cl == TGeoPgon::Class()) {
    const TGeoPgon* sh = (const TGeoPgon*) shape;
    log << " Phi1:"   << sh->GetPhi1()   << " dPhi:" << sh->GetDphi()
        << " NEdges:" << sh->GetNedges() << " Nz:" << sh->GetNz();
    for(int i=0, n=sh->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << sh->GetZ(i)
          << " r:[" << sh->GetRmin(i) << "," << sh->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoPcon::Class()) {
    const TGeoPcon* sh = (const TGeoPcon*) shape;
    log << " Phi1:" << sh->GetPhi1() << " dPhi:" << sh->GetDphi() << " Nz:" << sh->GetNz();
    for(int i=0, n=sh->GetNz(); i<n; ++i)  {
      log << " i=" << i << " z:" << sh->GetZ(i)
          << " r:[" << sh->GetRmin(i) << "," << sh->GetRmax(i) << "]";
    }
  }
  else if (cl == TGeoCone::Class()) {
    const TGeoCone* sh = (const TGeoCone*) shape;
    log << " rmin1:" << sh->GetRmin1() << " rmax1:" << sh->GetRmax1()
        << " rmin2:" << sh->GetRmin2() << " rmax2:" << sh->GetRmax2()
        << " dz:"    << sh->GetDz();
  }
  else if (cl == TGeoConeSeg::Class()) {
    const TGeoConeSeg* sh = (const TGeoConeSeg*) shape;
    log << " rmin1:" << sh->GetRmin1() << " rmax1:" << sh->GetRmax1()
        << " rmin2:" << sh->GetRmin2() << " rmax2:" << sh->GetRmax2()
        << " dz:"    << sh->GetDz()
        << " Phi1:"  << sh->GetPhi1() << " Phi2:" << sh->GetPhi2();
  }
  else if (cl == TGeoParaboloid::Class()) {
    const TGeoParaboloid* sh = (const TGeoParaboloid*) shape;
    log << " dz:" << sh->GetDz() << " RLo:" << sh->GetRlo() << " Rhi:" << sh->GetRhi();
  }
  else if (cl == TGeoSphere::Class()) {
    const TGeoSphere* sh = (const TGeoSphere*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax()
        << " Phi1:" << sh->GetPhi1() << " Phi2:" << sh->GetPhi2()
        << " Theta1:" << sh->GetTheta1() << " Theta2:" << sh->GetTheta2();
  }
  else if (cl == TGeoTorus::Class()) {
    const TGeoTorus* sh = (const TGeoTorus*) shape;
    log << " rmin:" << sh->GetRmin() << " rmax:" << sh->GetRmax() << " r:" << sh->GetR()
        << " Phi1:" << sh->GetPhi1() << " dPhi:" << sh->GetDphi();
  }
  else if (cl == TGeoArb8::Class()) {
    TGeoArb8* sh = (TGeoArb8*) shape;
    const Double_t* v = sh->GetVertices();
    log << " dz:" << sh->GetDz();
    for(int i=0; i<8; ++i) {
      log << " x[" << i << "]:" << *v; ++v;
      log << " y[" << i << "]:" << *v; ++v;
    }
  }
  else if (cl == TGeoXtru::Class()) {
    const TGeoXtru* sh = (const TGeoXtru*) shape;
    log << " X:   " << sh->GetX(0)
        << " Y:   " << sh->GetY(0)
        << " Z:   " << sh->GetZ(0)
        << " Xoff:" << sh->GetXOffset(0)
        << " Yoff:" << sh->GetYOffset(0)
        << " Nvtx:" << sh->GetNvert()
        << " Nz:  " << sh->GetNz();
  }
  else if (shape->IsA() == TGeoCompositeShape::Class()) {
    const TGeoCompositeShape* sh = (const TGeoCompositeShape*) shape;
    const TGeoBoolNode* boolean = sh->GetBoolNode();
    const TGeoShape* left = boolean->GetLeftShape();
    const TGeoShape* right = boolean->GetRightShape();
    TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
    if (oper == TGeoBoolNode::kGeoSubtraction)
      log << "Subtraction: ";
    else if (oper == TGeoBoolNode::kGeoUnion)
      log << "Union: ";
    else if (oper == TGeoBoolNode::kGeoIntersection)
      log << "Intersection: ";
    log << " Left:" << toStringSolid(left) << " Right:" << toStringSolid(right);
  }
  log << setprecision(prec);
  return log.str();
}

/// Output mesh vertices to string
std::string dd4hep::toStringMesh(const TGeoShape* shape, int prec)  {
  Solid sol(shape);
  stringstream os;

  // Prints shape parameters
  Int_t nvert = 0, nsegs = 0, npols = 0;

  sol->GetMeshNumbers(nvert, nsegs, npols);
  Double_t* points = new Double_t[3*nvert];
  sol->SetPoints(points);

  os << setw(16) << left << sol->IsA()->GetName()
     << " " << nvert << " Mesh-points:" << endl;
  os << setw(16) << left << sol->IsA()->GetName() << " " << sol->GetName()
     << " N(mesh)=" << sol->GetNmeshVertices()
     << "  N(vert)=" << nvert << "  N(seg)=" << nsegs << "  N(pols)=" << npols << endl;
    
  for(Int_t i=0; i<nvert; ++i)   {
    Double_t* p = points + 3*i;
    os << setw(16) << left << sol->IsA()->GetName() << " " << setw(3) << left << i
       << " Local  ("  << setw(7) << setprecision(prec) << fixed << right << p[0]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << p[1]
       << ", "         << setw(7) << setprecision(prec) << fixed << right << p[2]
       << ")" << endl;
  }
  Box box = sol;
  const Double_t* org = box->GetOrigin();
  os << setw(16) << left << sol->IsA()->GetName()
     << " Bounding box: "
     << " dx="        << setw(7) << setprecision(prec) << fixed << right << box->GetDX()
     << " dy="        << setw(7) << setprecision(prec) << fixed << right << box->GetDY()
     << " dz="        << setw(7) << setprecision(prec) << fixed << right << box->GetDZ()
     << " Origin: x=" << setw(7) << setprecision(prec) << fixed << right << org[0]
     << " y="         << setw(7) << setprecision(prec) << fixed << right << org[1]
     << " z="         << setw(7) << setprecision(prec) << fixed << right << org[2]
     << endl;
  
  /// -------------------- DONE --------------------
  delete [] points;
  return os.str();
}

template <typename T> void Solid_type<T>::_setDimensions(double* param)   const {
  this->ptr()->SetDimensions(param);
  this->ptr()->ComputeBBox();
}

/// Assign pointrs and register solid to geometry
template <typename T>
void Solid_type<T>::_assign(T* n, const string& nam, const string& tit, bool cbbox) {
  this->assign(n, nam, tit);
  if (cbbox)
    n->ComputeBBox();
}

/// Access to shape name
template <typename T> const char* Solid_type<T>::name() const {
  return this->ptr()->GetName();
}

/// Set new shape name
template <typename T> Solid_type<T>& Solid_type<T>::setName(const char* value)    {
  this->access()->SetName(value);
  return *this;
}

/// Set new shape name
template <typename T> Solid_type<T>& Solid_type<T>::setName(const string& value)    {
  this->access()->SetName(value.c_str());
  return *this;
}

/// Access to shape type (The TClass name of the ROOT implementation)
template <typename T> const char* Solid_type<T>::type() const  {
  if ( this->ptr() )  {
    return this->ptr()->IsA()->GetName();
  }
  return "";
}

/// Access the dimensions of the shape: inverse of the setDimensions member function
template <typename T> vector<double> Solid_type<T>::dimensions()  {
  return move( get_shape_dimensions(this->access()) );
}

/// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
template <typename T> Solid_type<T>& Solid_type<T>::setDimensions(const std::vector<double>& params)  {
  set_shape_dimensions(this->access(), params);
  return *this;
}

/// Divide volume into subsections (See the ROOT manuloa for details)
template <typename T> TGeoVolume*
Solid_type<T>::divide(const Volume& voldiv, const std::string& divname,
                      int iaxis, int ndiv, double start, double step)   const {
  T* p = this->ptr();
  if ( p )  {
    VolumeMulti mv(p->Divide(voldiv.ptr(), divname.c_str(), iaxis, ndiv, start, step));
    return mv.ptr();
  }
  except("dd4hep","Volume: Attempt to divide an invalid logical volume.");
  return 0;
}

/// Constructor to create an anonymous new box object (retrieves name from volume)
ShapelessSolid::ShapelessSolid(const string& nam)  {
  _assign(new TGeoShapeAssembly(), nam, "Assembly", true);
}

void Box::make(const std::string& nam, double x_val, double y_val, double z_val)   {
  _assign(new TGeoBBox(nam.c_str(), x_val, y_val, z_val), "", "Box", true);
}

/// Set the box dimensionsy
Box& Box::setDimensions(double x_val, double y_val, double z_val)   {
  double params[] = { x_val, y_val, z_val};
  _setDimensions(params);
  return *this;
}

/// Access half "length" of the box
double Box::x() const {
  return this->ptr()->GetDX();
}

/// Access half "width" of the box
double Box::y() const {
  return this->ptr()->GetDY();
}

/// Access half "depth" of the box
double Box::z() const {
  return this->ptr()->GetDZ();
}

/// Internal helper method to support object construction
void HalfSpace::make(const std::string& nam, const double* const point, const double* const normal)   {
  _assign(new TGeoHalfSpace(nam.c_str(),(Double_t*)point, (Double_t*)normal), "", "HalfSpace",true);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(double startPhi, double deltaPhi) {
  _assign(new TGeoPcon(startPhi/units::deg, deltaPhi/units::deg, 0), "", "Polycone", false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double startPhi, double deltaPhi,
                   const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  vector<double> params;
  if (rmin.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=rmin.size()) || (z.size()!=rmax.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,rmin,rmax not of same length");
  }
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", "Polycone", true);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(double startPhi, double deltaPhi, const vector<double>& r, const vector<double>& z) {
  vector<double> params;
  if (r.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=r.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,r not of same length");
  } 
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(r.size());
  for (size_t i = 0; i < r.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(0.0  );
    params.emplace_back(r[i] );
  }
  _assign(new TGeoPcon(&params[0]), "", "Polycone", true);
}

/// Constructor to be used when creating a new object
Polycone::Polycone(const std::string& nam, double startPhi, double deltaPhi) {
  _assign(new TGeoPcon(nam.c_str(), startPhi/units::deg, deltaPhi/units::deg, 0), "", "Polycone", false);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const std::string& nam, double startPhi, double deltaPhi,
                   const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  vector<double> params;
  if (rmin.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=rmin.size()) || (z.size()!=rmax.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,rmin,rmax not of same length");
  }
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(rmin.size());
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _assign(new TGeoPcon(&params[0]), nam, "Polycone", true);
}

/// Constructor to be used when creating a new polycone object. Add at the same time all Z planes
Polycone::Polycone(const std::string& nam, double startPhi, double deltaPhi, const vector<double>& r, const vector<double>& z) {
  vector<double> params;
  if (r.size() < 2) {
    throw runtime_error("dd4hep: PolyCone Not enough Z planes. minimum is 2!");
  }
  if((z.size()!=r.size()) )    {
    throw runtime_error("dd4hep: Polycone: vectors z,r not of same length");
  } 
  params.emplace_back(startPhi/units::deg);
  params.emplace_back(deltaPhi/units::deg);
  params.emplace_back(r.size());
  for (size_t i = 0; i < r.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(0.0  );
    params.emplace_back(r[i] );
  }
  _assign(new TGeoPcon(&params[0]), nam, "Polycone", true);
}

/// Add Z-planes to the Polycone
void Polycone::addZPlanes(const vector<double>& rmin, const vector<double>& rmax, const vector<double>& z) {
  TGeoPcon* sh = *this;
  vector<double> params;
  size_t num = sh->GetNz();
  if (rmin.size() < 2)   {
    throw runtime_error("dd4hep: PolyCone::addZPlanes> Not enough Z planes. minimum is 2!");
  }
  params.emplace_back(sh->GetPhi1());
  params.emplace_back(sh->GetDphi());
  params.emplace_back(num + rmin.size());
  for (size_t i = 0; i < num; ++i) {
    params.emplace_back(sh->GetZ(i));
    params.emplace_back(sh->GetRmin(i));
    params.emplace_back(sh->GetRmax(i));
  }
  for (size_t i = 0; i < rmin.size(); ++i) {
    params.emplace_back(z[i] );
    params.emplace_back(rmin[i] );
    params.emplace_back(rmax[i] );
  }
  _setDimensions(&params[0]);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(double dz, 
                         double rmin1,     double rmax1,
                         double rmin2,     double rmax2,
                         double startPhi,  double endPhi)
{
  _assign(new TGeoConeSeg(dz, rmin1, rmax1, rmin2, rmax2,
                          startPhi/units::deg, endPhi/units::deg), "", "ConeSegment", true);
}

/// Constructor to be used when creating a new cone segment object
ConeSegment::ConeSegment(const std::string& nam,
                         double dz, 
                         double rmin1,     double rmax1,
                         double rmin2,     double rmax2,
                         double startPhi,  double endPhi)
{
  _assign(new TGeoConeSeg(nam.c_str(), dz, rmin1, rmax1, rmin2, rmax2,
                          startPhi/units::deg, endPhi/units::deg), "", "ConeSegment", true);
}

/// Set the cone segment dimensions
ConeSegment& ConeSegment::setDimensions(double dz, 
                                        double rmin1, double rmax1,
                                        double rmin2, double rmax2,
                                        double startPhi,  double endPhi) {
  double params[] = { dz, rmin1, rmax1, rmin2, rmax2, startPhi/units::deg, endPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Cone::make(const std::string& nam, double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  _assign(new TGeoCone(nam.c_str(), z, rmin1, rmax1, rmin2, rmax2 ), "", "Cone", true);
}

/// Set the box dimensions (startPhi=0.0, endPhi=2*pi)
Cone& Cone::setDimensions(double z, double rmin1, double rmax1, double rmin2, double rmax2) {
  double params[] = { z, rmin1, rmax1, rmin2, rmax2  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Tube::make(const string& nam, double rmin, double rmax, double z, double startPhi, double endPhi) {
  _assign(new TGeoTubeSeg(nam.c_str(), rmin, rmax, z, startPhi/units::deg, endPhi/units::deg),nam,"Tube",true);
}

/// Set the tube dimensions
Tube& Tube::setDimensions(double rmin, double rmax, double z, double startPhi, double endPhi) {
  double params[] = {rmin,rmax,z,startPhi/units::deg,endPhi/units::deg};
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
CutTube::CutTube(double rmin, double rmax, double dz, double startPhi, double endPhi,
                 double lx, double ly, double lz, double tx, double ty, double tz)  {
  make("", rmin,rmax,dz,startPhi/units::deg,endPhi/units::deg,lx,ly,lz,tx,ty,tz);
}

/// Constructor to be used when creating a new object with attribute initialization
CutTube::CutTube(const std::string& nam,
                 double rmin, double rmax, double dz, double startPhi, double endPhi,
                 double lx, double ly, double lz, double tx, double ty, double tz)  {
  make(nam, rmin,rmax,dz,startPhi/units::deg,endPhi/units::deg,lx,ly,lz,tx,ty,tz);
}

/// Constructor to be used when creating a new object with attribute initialization
void CutTube::make(const std::string& nam, double rmin, double rmax, double dz, double startPhi, double endPhi,
                   double lx, double ly, double lz, double tx, double ty, double tz)  {
  _assign(new TGeoCtub(nam.c_str(), rmin,rmax,dz,startPhi,endPhi,lx,ly,lz,tx,ty,tz),"","CutTube",true);
}

/// Constructor to create a truncated tube object with attribute initialization
TruncatedTube::TruncatedTube(double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                             double cutAtStart, double cutAtDelta, bool cutInside)
{  make("", zhalf, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg, cutAtStart, cutAtDelta, cutInside);    }

/// Constructor to create a truncated tube object with attribute initialization
TruncatedTube::TruncatedTube(const std::string& nam,
                             double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                             double cutAtStart, double cutAtDelta, bool cutInside)
{  make(nam, zhalf, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg, cutAtStart, cutAtDelta, cutInside);    }

/// Internal helper method to support object construction
void TruncatedTube::make(const std::string& nam,
                         double zhalf, double rmin, double rmax, double startPhi, double deltaPhi,
                         double cutAtStart, double cutAtDelta, bool cutInside)   {
  // check the parameters
  if( rmin <= 0 || rmax <= 0 || cutAtStart <= 0 || cutAtDelta <= 0 )
    except(TRUNCATEDTUBE_TAG,"++ 0 <= rIn,cutAtStart,rOut,cutAtDelta,rOut violated!");
  else if( rmin >= rmax )
    except(TRUNCATEDTUBE_TAG,"++ rIn<rOut violated!");
  else if( startPhi != 0. )
    except(TRUNCATEDTUBE_TAG,"++ startPhi != 0 not supported!");

  double r         = cutAtStart;
  double R         = cutAtDelta;
  // angle of the box w.r.t. tubs
  double cath      = r - R * std::cos( deltaPhi*units::deg );
  double hypo      = std::sqrt( r*r + R*R - 2.*r*R * std::cos( deltaPhi*units::deg ));
  double cos_alpha = cath / hypo;
  double alpha     = std::acos( cos_alpha );
  double sin_alpha = std::sin( std::fabs(alpha) );
  
  // exaggerate dimensions - does not matter, it's subtracted!
  // If we don't, the **edge** of the box would cut into the tube segment
  // for larger delta-phi values
  double boxX      = 1.1*rmax + rmax/sin_alpha; // Need to adjust for move!
  double boxY      = rmax;
  // width of the box > width of the tubs
  double boxZ      = 1.1 * zhalf;
  double xBox;      // center point of the box
  if( cutInside )
    xBox = r - boxY / sin_alpha;
  else
    xBox = r + boxY / sin_alpha;

  // rotationmatrix of box w.r.t. tubs
  TGeoRotation rot;
  //rot.RotateX( 90.0 );
  rot.RotateZ( -alpha/dd4hep::deg );
  TGeoTranslation trans(xBox, 0., 0.);  
  TGeoBBox* box  = new TGeoBBox((nam+"Box").c_str(), boxX, boxY, boxZ);
  TGeoTubeSeg* tubs = new TGeoTubeSeg((nam+"Tubs").c_str(), rmin, rmax, zhalf, startPhi, deltaPhi);
  TGeoSubtraction* sub = new TGeoSubtraction(tubs, box, nullptr, new TGeoCombiTrans(trans, rot));
  // For debugging:
  // TGeoUnion* sub = new TGeoUnion(tubs, box, nullptr, new TGeoCombiTrans(trans, rot));
  _assign(new TGeoCompositeShape(nam.c_str(), sub),"",TRUNCATEDTUBE_TAG,true);
#if 0
  cout << "Trans:";  trans.Print(); cout << endl;
  cout << "Rot:  ";  rot.Print();   cout << endl;
  cout << " Zhalf:        " << zhalf
       << " rmin:         " << rmin
       << " rmax:         " << rmax
       << " r/cutAtStart: " << r
       << " R/cutAtDelta: " << R
       << " cutInside:    " << (cutInside ? "YES" : "NO ")
       << endl;
  cout << " cath:      " << cath
       << " hypo:      " << hypo
       << " cos_alpha: " << cos_alpha
       << " alpha:     " << alpha
       << " alpha(deg):" << alpha/dd4hep::deg
       << endl;
  cout << " Deg:       " << dd4hep::deg
       << " cm:        " << dd4hep::cm
       << " xBox:      " << xBox
       << endl;
  cout << "Box:" << "x:" << box->GetDX() << " y:" << box->GetDY() << " z:" << box->GetDZ() << endl;
  cout << "Tubs:" << " rmin:" << rmin << " rmax" << rmax << "zhalf" << zhalf
       << " startPhi:" <<  startPhi << " deltaPhi:" << deltaPhi << endl;
#endif
}

/// Constructor to be used when creating a new object with attribute initialization
void EllipticalTube::make(const std::string& nam, double a, double b, double dz) {
  _assign(new TGeoEltu(nam.c_str(), a, b, dz), "", "EllipticalTube", true);
}

/// Constructor to be used when creating a new object with attribute initialization
void Trd1::make(const std::string& nam, double x1, double x2, double y, double z) {
  _assign(new TGeoTrd1(nam.c_str(), x1, x2, y, z ), "", "Trd1", true);
}

/// Set the Trd1 dimensions
Trd1& Trd1::setDimensions(double x1, double x2, double y, double z) {
  double params[] = { x1, x2, y, z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Trd2::make(const std::string& nam, double x1, double x2, double y1, double y2, double z) {
  _assign(new TGeoTrd2(nam.c_str(), x1, x2, y1, y2, z ), "", "Trd2", true);
}

/// Set the Trd2 dimensions
Trd2& Trd2::setDimensions(double x1, double x2, double y1, double y2, double z) {
  double params[] = { x1, x2, y1, y2, z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Paraboloid::make(const std::string& nam, double r_low, double r_high, double delta_z) {
  _assign(new TGeoParaboloid(nam.c_str(), r_low, r_high, delta_z ), "", "Paraboloid", true);
}

/// Set the Paraboloid dimensions
Paraboloid& Paraboloid::setDimensions(double r_low, double r_high, double delta_z) {
  double params[] = { r_low, r_high, delta_z  };
  _setDimensions(params);
  return *this;
}

/// Constructor to create a new anonymous object with attribute initialization
void Hyperboloid::make(const std::string& nam, double rin, double stin, double rout, double stout, double dz) {
  _assign(new TGeoHype(nam.c_str(), rin, stin/units::deg, rout, stout/units::deg, dz), "", "Hyperboloid", true);
}

/// Set the Hyperboloid dimensions
Hyperboloid& Hyperboloid::setDimensions(double rin, double stin, double rout, double stout, double dz)  {
  double params[] = { rin, stin/units::deg, rout, stout/units::deg, dz};
  _setDimensions(params);
  return *this;
}

/// Constructor function to be used when creating a new object with attribute initialization
void Sphere::make(const std::string& nam, double rmin, double rmax, double startTheta, double endTheta, double startPhi, double endPhi) {
  _assign(new TGeoSphere(nam.c_str(), rmin, rmax,
                         startTheta/units::deg, endTheta/units::deg,
                         startPhi/units::deg,   endPhi/units::deg), "", "Sphere", true);
}

/// Set the Sphere dimensions
Sphere& Sphere::setDimensions(double rmin, double rmax, double startTheta, double endTheta, double startPhi, double endPhi) {
  double params[] = { rmin, rmax, startTheta/units::deg, endTheta/units::deg, startPhi/units::deg, endPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new object with attribute initialization
void Torus::make(const std::string& nam, double r, double rmin, double rmax, double startPhi, double deltaPhi) {
  _assign(new TGeoTorus(nam.c_str(), r, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg), "", "Torus", true);
}

/// Set the Torus dimensions
Torus& Torus::setDimensions(double r, double rmin, double rmax, double startPhi, double deltaPhi) {
  double params[] = { r, rmin, rmax, startPhi/units::deg, deltaPhi/units::deg };
  _setDimensions(params);
  return *this;
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(double z, double theta, double phi,
           double h1, double bl1, double tl1, double alpha1,
           double h2, double bl2, double tl2, double alpha2) {
  _assign(new TGeoTrap(z, theta/units::deg, phi/units::deg,
                       h1, bl1, tl1, alpha1/units::deg,
                       h2, bl2, tl2, alpha2/units::deg), "", "Trap", true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
Trap::Trap(const std::string& nam,
           double z, double theta, double phi,
           double h1, double bl1, double tl1, double alpha1,
           double h2, double bl2, double tl2, double alpha2) {
  _assign(new TGeoTrap(nam.c_str(), z, theta/units::deg, phi/units::deg,
                       h1, bl1, tl1, alpha1/units::deg,
                       h2, bl2, tl2, alpha2/units::deg), "", "Trap", true);
}

/// Constructor to be used when creating a new anonymous object with attribute initialization
void Trap::make(const std::string& nam, double pz, double py, double px, double pLTX) {
  double z      = pz / 2e0;
  double theta  = 0e0;
  double phi    = 0e0;
  double h      = py / 2e0;
  double bl     = px / 2e0;
  double tl     = pLTX / 2e0;
  double alpha1 = (pLTX - px) / py;
  _assign(new TGeoTrap(nam.c_str(), z, theta, phi,
                       h, bl, tl, alpha1/units::deg,
                       h, bl, tl, alpha1/units::deg), "", "Trap", true);
}

/// Set the trap dimensions
Trap& Trap::setDimensions(double z, double theta, double phi,
                          double h1, double bl1, double tl1, double alpha1,
                          double h2, double bl2, double tl2, double alpha2) {
  double params[] = { z,  theta/units::deg, phi/units::deg,
                      h1, bl1, tl1, alpha1/units::deg,
                      h2, bl2, tl2, alpha2/units::deg };
  _setDimensions(params);
  return *this;
}

/// Internal helper method to support object construction
void PseudoTrap::make(const std::string& nam, double x1, double x2, double y1, double y2, double z, double r, bool atMinusZ)    {
  double x            = atMinusZ ? x1 : x2;
  double h            = 0;
  bool   intersec     = false; // union or intersection solid
  double displacement = 0;
  double startPhi     = 0;
  double halfZ        = z;
  double halfOpeningAngle = std::asin( x / std::abs( r ))/units::deg;

  /// calculate the displacement of the tubs w.r.t. to the trap, determine the opening angle of the tubs
  double delta        = std::sqrt( r * r - x * x );
 
#if 0
  // Implementation from : (Crappy)
  // https://cmssdt.cern.ch/lxr/source/SimG4Core/Geometry/src/DDG4SolidConverter.cc#0362
  if( r < 0 && std::abs(r) >= x )  {
    intersec = true;       // intersection solid
    h = y1 < y2 ? y2 : y1; // tubs half height
    h += h/20.;            // enlarge a bit - for subtraction solid
    if( atMinusZ )    {
      displacement = -halfZ - delta; 
      startPhi     =  270.0 - halfOpeningAngle;
    }
    else    {
      displacement =  halfZ + delta;
      startPhi     =  90.0  - halfOpeningAngle;
    }
  }
  else if( r > 0 && std::abs(r) >= x )  {
    if( atMinusZ )    {
      displacement = -halfZ + delta;
      startPhi     =  90.0  - halfOpeningAngle;
      h = y1;
    }
    else
    {
      displacement =  halfZ - delta; 
      startPhi     =  270.0 - halfOpeningAngle;
      h = y2;
    }    
  }
  else  {
    except(PSEUDOTRAP_TAG,"Check parameters of the PseudoTrap!");   
  }
#endif

  // Implementation from :
  // https://cmssdt.cern.ch/lxr/source/Fireworks/Geometry/src/TGeoMgrFromDdd.cc#0538
  if( r < 0 && std::abs(r) >= x )  {
    intersec = true;       // intersection solid
    h = y1 < y2 ? y2 : y1; // tubs half height
    h += h/20.;            // enlarge a bit - for subtraction solid
    if( atMinusZ )    {
      displacement = - halfZ - delta; 
      startPhi     =  90.0 - halfOpeningAngle;
    }
    else    {
      displacement =   halfZ + delta;
      startPhi     = -90.0 - halfOpeningAngle;
    }
  }
  else if( r > 0 && std::abs(r) >= x )  {
    if( atMinusZ )    {
      displacement = - halfZ + delta;
      startPhi     = 270.0 - halfOpeningAngle;
      h = y1;
    }
    else
    {
      displacement =   halfZ - delta; 
      startPhi     =  90.0 - halfOpeningAngle;
      h = y2;
    }    
  }
  else  {
    except(PSEUDOTRAP_TAG,"Check parameters of the PseudoTrap!");   
  }

  Solid trap(new TGeoTrd2((nam+"Trd2").c_str(), x1, x2, y1, y2, halfZ));
  Solid tubs(new TGeoTubeSeg((nam+"Tubs").c_str(), 0.,std::abs(r),h,startPhi,startPhi + halfOpeningAngle*2.));
  TGeoCompositeShape* solid = 0;
  if( intersec )  {
    solid = SubtractionSolid(nam, trap, tubs, Transform3D(RotationX(M_PI/2.), Position(0.,0.,displacement))).ptr();
  }
  else  {
    SubtractionSolid sub((nam+"Subs").c_str(), tubs, Box(1.1*x, 1.1*h, std::sqrt(r*r-x*x)), Transform3D(RotationX(M_PI/2.)));
    solid = UnionSolid(nam, trap, sub, Transform3D(RotationX(M_PI/2.), Position(0,0,displacement))).ptr();
  }
  _assign(solid,"",PSEUDOTRAP_TAG, true);
}

/// Helper function to create poly hedron
void PolyhedraRegular::make(const std::string& nam, int nsides, double rmin, double rmax,
                            double zpos, double zneg, double start, double delta) {
  if (rmin < 0e0 || rmin > rmax)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmin:<" + _toString(rmin) + "> is invalid!");
  else if (rmax < 0e0)
    throw runtime_error("dd4hep: PolyhedraRegular: Illegal argument rmax:<" + _toString(rmax) + "> is invalid!");
  double params[] = { start/units::deg, delta/units::deg, double(nsides), 2e0, zpos, rmin, rmax, zneg, rmin, rmax };
  _assign(new TGeoPgon(params), nam, "Polyhedra", false);
  //_setDimensions(&params[0]);
}

/// Helper function to create poly hedron
void Polyhedra::make(const std::string& nam, int nsides, double start, double delta,
                     const vector<double>& z, const vector<double>& rmin, const vector<double>& rmax)  {
  vector<double> temp;
  if ( rmin.size() != z.size() || rmax.size() != z.size() )  {
    except("Polyhedra",
           "Number of values to define zplanes are incorrect: z:%ld rmin:%ld rmax:%ld",
           z.size(), rmin.size(), rmax.size());
  }
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  temp.reserve(4+z.size()*2);
  temp.emplace_back(start/units::deg);
  temp.emplace_back(delta/units::deg);
  temp.emplace_back(double(nsides));
  temp.emplace_back(double(z.size()));
  for(size_t i=0; i<z.size(); ++i)   {
    temp.emplace_back(z[i]);
    temp.emplace_back(rmin[i]);
    temp.emplace_back(rmax[i]);
  }
  _assign(new TGeoPgon(&temp[0]), nam, "Polyhedra", false);
}

/// Helper function to create the polyhedron
void ExtrudedPolygon::make(const std::string& nam,
                           const vector<double>& pt_x,
                           const vector<double>& pt_y,
                           const vector<double>& sec_z,
                           const vector<double>& sec_x,
                           const vector<double>& sec_y,
                           const vector<double>& sec_scale)
{
  TGeoXtru* solid = new TGeoXtru(sec_z.size());
  _assign(solid, nam, "ExtrudedPolygon", false);
  // No need to transform coordinates to cm. We are in the dd4hep world: all is already in cm.
  solid->DefinePolygon(pt_x.size(), &(*pt_x.begin()), &(*pt_y.begin()));
  for( size_t i = 0; i < sec_z.size(); ++i )
    solid->DefineSection(i, sec_z[i], sec_x[i], sec_y[i], sec_scale[i]);
}

/// Creator method
void EightPointSolid::make(const std::string& nam, double dz, const double* vtx)   {
  _assign(new TGeoArb8(nam.c_str(), dz, (double*)vtx), "", "EightPointSolid", true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
SubtractionSolid::SubtractionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is the identity rotation
SubtractionSolid::SubtractionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object
SubtractionSolid::SubtractionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoSubtraction* sub = new TGeoSubtraction(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), sub), "", SUBTRACTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
UnionSolid::UnionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
UnionSolid::UnionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Rotation is identity rotation
UnionSolid::UnionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoUnion* uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object
UnionSolid::UnionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoUnion *uni = new TGeoUnion(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), uni), "", UNION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape("", inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity, Rotation is identity rotation
IntersectionSolid::IntersectionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_identity());
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Placement by a generic transformation within the mother
IntersectionSolid::IntersectionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Transform3D& trans) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_transform(trans));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object. Position is identity.
IntersectionSolid::IntersectionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Position& pos) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_translation(pos));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const RotationZYX& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotationZYX(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}

/// Constructor to be used when creating a new object
IntersectionSolid::IntersectionSolid(const std::string& nam, const Solid& shape1, const Solid& shape2, const Rotation3D& rot) {
  TGeoIntersection* inter = new TGeoIntersection(shape1, shape2, detail::matrix::_identity(), detail::matrix::_rotation3D(rot));
  _assign(new TGeoCompositeShape(nam.c_str(), inter), "", INTERSECTION_TAG, true);
}


#define INSTANTIATE(X) template class dd4hep::Solid_type<X>

INSTANTIATE(TGeoShape);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoHalfSpace);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoConeSeg);
INSTANTIATE(TGeoParaboloid);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoSphere);
INSTANTIATE(TGeoTorus);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoEltu);
INSTANTIATE(TGeoXtru);
INSTANTIATE(TGeoHype);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd1);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoCompositeShape);
