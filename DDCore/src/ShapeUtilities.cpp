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
#include <DD4hep/Shapes.h>
#include <DD4hep/MatrixHelpers.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/ShapeTags.h>
#include <DD4hep/Printout.h>
#include <DD4hep/detail/ShapesInterna.h>

// C/C++ include files
#include <stdexcept>
#include <iomanip>
#include <sstream>

// ROOT includes
#include <TClass.h>
#include <TGeoMatrix.h>
#include <TGeoBoolNode.h>
#include <TGeoScaledShape.h>
#include <TGeoCompositeShape.h>

using namespace std;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // name units differently
  namespace units = dd4hep;

  // Local, anonymous stuff
  namespace {
    template <typename T> inline T* get_ptr(const TGeoShape* shape)    {
      if ( shape && shape->IsA() == T::Class() ) return (T*)shape;
      except("Dimension","Invalid shape pointer!");
      return 0;
    }
    inline void invalidSetDimensionCall(const TGeoShape* sh, const vector<double>& params)   {
      except("Solid","+++ Shape:%s setDimension: Invalid number of parameters: %ld",
             (sh ? typeName(typeid(*sh)) : typeName(typeid(sh))).c_str(), params.size());
    }
    template <typename T> inline bool check_shape_type(const Handle<TGeoShape>& solid)   {
      return solid.isValid() && solid->IsA() == T::Class();
    }
  }
  
  /// Type check of various shapes. Result like dynamic_cast. Compare with python's isinstance(obj,type)
  template <typename SOLID> bool isInstance(const Handle<TGeoShape>& solid)   {
    return check_shape_type<typename SOLID::Object>(solid);
  }
  template bool isInstance<Box>               (const Handle<TGeoShape>& solid);
  template bool isInstance<ShapelessSolid>    (const Handle<TGeoShape>& solid);
  template bool isInstance<HalfSpace>         (const Handle<TGeoShape>& solid);
  template bool isInstance<ConeSegment>       (const Handle<TGeoShape>& solid);
  template bool isInstance<CutTube>           (const Handle<TGeoShape>& solid);
  template bool isInstance<EllipticalTube>    (const Handle<TGeoShape>& solid);
  template bool isInstance<Trap>              (const Handle<TGeoShape>& solid);
  template bool isInstance<Trd1>              (const Handle<TGeoShape>& solid);
  template bool isInstance<Trd2>              (const Handle<TGeoShape>& solid);
  template bool isInstance<Torus>             (const Handle<TGeoShape>& solid);
  template bool isInstance<Sphere>            (const Handle<TGeoShape>& solid);
  template bool isInstance<Paraboloid>        (const Handle<TGeoShape>& solid);
  template bool isInstance<Hyperboloid>       (const Handle<TGeoShape>& solid);
  template bool isInstance<PolyhedraRegular>  (const Handle<TGeoShape>& solid);
  template bool isInstance<Polyhedra>         (const Handle<TGeoShape>& solid);
  template bool isInstance<ExtrudedPolygon>   (const Handle<TGeoShape>& solid);
  template bool isInstance<BooleanSolid>      (const Handle<TGeoShape>& solid);

  template <> bool isInstance<Cone>(const Handle<TGeoShape>& solid)  {
    return check_shape_type<TGeoConeSeg>(solid) || check_shape_type<TGeoCone>(solid);
  }
  template <> bool isInstance<Tube>(const Handle<TGeoShape>& solid)  {
    return check_shape_type<TGeoTubeSeg>(solid)
      || check_shape_type<TGeoCtub>(solid)
      || check_shape_type<TwistedTubeObject>(solid);
  }
  template <> bool isInstance<Polycone>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TGeoPcon>(solid)    || check_shape_type<TGeoPgon>(solid);
  }
  template <> bool isInstance<EightPointSolid>(const Handle<TGeoShape>& solid)   {
    if ( solid.isValid() )   {
      TClass* c = solid->IsA();
      return c==TGeoArb8::Class() || c==TGeoTrap::Class() || c==TGeoGtra::Class();
    }
    return false;
  }
  template <> bool isInstance<TwistedTube>(const Handle<TGeoShape>& solid)  {
    return check_shape_type<TwistedTubeObject>(solid);
  }
  template <> bool isInstance<TruncatedTube>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TGeoCompositeShape>(solid)
      &&   ::strcmp(solid->GetTitle(), TRUNCATEDTUBE_TAG) == 0;
  }
  template <> bool isInstance<PseudoTrap>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TGeoCompositeShape>(solid)
      &&   ::strcmp(solid->GetTitle(), PSEUDOTRAP_TAG) == 0;
  }
  template <> bool isInstance<SubtractionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoSubtraction;
  }
  template <> bool isInstance<UnionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoUnion;
  }
  template <> bool isInstance<IntersectionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoIntersection;
  }

  /// Type check of various shapes. Do not allow for polymorphism. Types must match exactly
  template <typename SOLID> bool isA(const Handle<TGeoShape>& solid)   {
    return check_shape_type<typename SOLID::Object>(solid);
  }
  template bool isA<Box>(const Handle<TGeoShape>& solid);
  template bool isA<ShapelessSolid>(const Handle<TGeoShape>& solid);
  template bool isA<HalfSpace>(const Handle<TGeoShape>& solid);
  template bool isA<Cone>(const Handle<TGeoShape>& solid);
  template bool isA<ConeSegment>(const Handle<TGeoShape>& solid);
  template bool isA<Tube>(const Handle<TGeoShape>& solid);
  template bool isA<CutTube>(const Handle<TGeoShape>& solid);
  template bool isA<EllipticalTube>(const Handle<TGeoShape>& solid);
  template bool isA<Trap>(const Handle<TGeoShape>& solid);
  template bool isA<Trd1>(const Handle<TGeoShape>& solid);
  template bool isA<Trd2>(const Handle<TGeoShape>& solid);
  template bool isA<Torus>(const Handle<TGeoShape>& solid);
  template bool isA<Sphere>(const Handle<TGeoShape>& solid);
  template bool isA<Paraboloid>(const Handle<TGeoShape>& solid);
  template bool isA<Hyperboloid>(const Handle<TGeoShape>& solid);
  template bool isA<PolyhedraRegular>(const Handle<TGeoShape>& solid);
  template bool isA<Polyhedra>(const Handle<TGeoShape>& solid);
  template bool isA<ExtrudedPolygon>(const Handle<TGeoShape>& solid);
  template bool isA<Polycone>(const Handle<TGeoShape>& solid);
  template bool isA<EightPointSolid>(const Handle<TGeoShape>& solid);

  template <> bool isA<TwistedTube>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TwistedTubeObject>(solid)
      &&   ::strcmp(solid->GetTitle(), TWISTEDTUBE_TAG) == 0;
  }
  template <> bool isA<TruncatedTube>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TGeoCompositeShape>(solid)
      &&   ::strcmp(solid->GetTitle(), TRUNCATEDTUBE_TAG) == 0;
  }
  template <> bool isA<PseudoTrap>(const Handle<TGeoShape>& solid)   {
    return check_shape_type<TGeoCompositeShape>(solid)
      &&   ::strcmp(solid->GetTitle(), PSEUDOTRAP_TAG) == 0;
  }
  template <> bool isA<SubtractionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoSubtraction
      &&   ::strcmp(sh->GetTitle(), SUBTRACTION_TAG) == 0;
  }
  template <> bool isA<UnionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoUnion
      &&   ::strcmp(sh->GetTitle(), UNION_TAG) == 0;
  }
  template <> bool isA<IntersectionSolid>(const Handle<TGeoShape>& solid)   {
    TGeoCompositeShape* sh = (TGeoCompositeShape*)solid.ptr();
    return sh && sh->IsA() == TGeoCompositeShape::Class()
      &&   sh->GetBoolNode()->GetBooleanOperator() == TGeoBoolNode::kGeoIntersection
      &&   ::strcmp(sh->GetTitle(), INTERSECTION_TAG) == 0;
  }

  template <typename T> vector<double> dimensions(const TGeoShape* shape)    {
    stringstream str;
    if ( shape )
      str << "Shape: dimension(" << typeName(typeid(*shape)) << "): Invalid call!";
    else
      str << "Shape: dimension<TGeoShape): Invalid call && pointer!";
    throw runtime_error(str.str());
  }
  template <> vector<double> dimensions<TGeoShapeAssembly>(const TGeoShape* shape)    {
    const auto* sh = get_ptr<TGeoShapeAssembly>(shape);
    return { sh->GetDX(), sh->GetDY(), sh->GetDZ() };
  }
  template <> vector<double> dimensions<TGeoBBox>(const TGeoShape* shape)    {
    const auto* sh = get_ptr<TGeoBBox>(shape);
    return { sh->GetDX(), sh->GetDY(), sh->GetDZ() };
  }
  template <> vector<double> dimensions<TGeoHalfSpace>(const TGeoShape* shape)    {
    auto* sh = get_ptr<TGeoHalfSpace>(shape);
    return { sh->GetPoint()[0], sh->GetPoint()[1], sh->GetPoint()[2],
        sh->GetNorm()[0], sh->GetNorm()[1], sh->GetNorm()[2] };
  }
  template <> vector<double> dimensions<TGeoPcon>(const TGeoShape* shape)    {
    const TGeoPcon* sh = get_ptr<TGeoPcon>(shape);
    vector<double> pars { sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg, double(sh->GetNz()) };
    pars.reserve(3+3*sh->GetNz());
    for (Int_t i = 0; i < sh->GetNz(); ++i) {
      pars.emplace_back(sh->GetZ(i));
      pars.emplace_back(sh->GetRmin(i));
      pars.emplace_back(sh->GetRmax(i));
    }
    return pars;
  }
  template <> vector<double> dimensions<TGeoConeSeg>(const TGeoShape* shape)    {
    const TGeoConeSeg* sh = get_ptr<TGeoConeSeg>(shape);
    return { sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(), sh->GetRmin2(), sh->GetRmax2(),
        sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg };
  }
  template <> vector<double> dimensions<TGeoCone>(const TGeoShape* shape)    {
    const TGeoCone* sh = get_ptr<TGeoCone>(shape);
    return { sh->GetDz(), sh->GetRmin1(), sh->GetRmax1(), sh->GetRmin2(), sh->GetRmax2() };
  }  
  template <> vector<double> dimensions<TGeoTube>(const TGeoShape* shape)    {
    const TGeoTube* sh = get_ptr<TGeoTube>(shape);
    return { sh->GetRmin(), sh->GetRmax(), sh->GetDz() };
  }
  template <> vector<double> dimensions<TGeoTubeSeg>(const TGeoShape* shape)    {
    const TGeoTubeSeg* sh = get_ptr<TGeoTubeSeg>(shape);
    return { sh->GetRmin(), sh->GetRmax(), sh->GetDz(), sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg };
  }
  template <> vector<double> dimensions<TwistedTubeObject>(const TGeoShape* shape)    {
    const TwistedTubeObject* sh = get_ptr<TwistedTubeObject>(shape);
    return { sh->GetPhiTwist(), sh->GetRmin(), sh->GetRmax(),
        sh->GetNegativeEndZ(), sh->GetPositiveEndZ(),
        double(sh->GetNsegments()), sh->GetPhi2()*units::deg };
  }
  template <> vector<double> dimensions<TGeoCtub>(const TGeoShape* shape)    {
    const TGeoCtub* sh = get_ptr<TGeoCtub>(shape);
    const Double_t*	lo = sh->GetNlow();
    const Double_t*	hi = sh->GetNhigh();
    return { sh->GetRmin(), sh->GetRmax(), sh->GetDz(),
        sh->GetPhi1()*units::deg, sh->GetPhi2()*units::deg,
        lo[0], lo[1], lo[2], hi[0], hi[1], hi[2] };
  }
  template <> vector<double> dimensions<TGeoEltu>(const TGeoShape* shape)    {
    const TGeoEltu* sh = get_ptr<TGeoEltu>(shape);
    return { sh->GetA(), sh->GetB(), sh->GetDz() };
  }
  template <> vector<double> dimensions<TGeoTrd1>(const TGeoShape* shape)    {
    const TGeoTrd1* sh = get_ptr<TGeoTrd1>(shape);
    return { sh->GetDx1(), sh->GetDx2(), sh->GetDy(), sh->GetDz() };
  }
  template <> vector<double> dimensions<TGeoTrd2>(const TGeoShape* shape)    {
    const TGeoTrd2* sh = get_ptr<TGeoTrd2>(shape);
    return { sh->GetDx1(), sh->GetDx2(), sh->GetDy1(), sh->GetDy2(), sh->GetDz() };
  }
  template <> vector<double> dimensions<TGeoParaboloid>(const TGeoShape* shape)    {
    const TGeoParaboloid* sh = get_ptr<TGeoParaboloid>(shape);
    return { sh->GetRlo(), sh->GetRhi(), sh->GetDz() };
  }
  template <> vector<double> dimensions<TGeoHype>(const TGeoShape* shape)    {
    const TGeoHype* sh = get_ptr<TGeoHype>(shape);
    return { sh->GetDz(),
        sh->GetRmin(), sh->GetStIn()*units::deg,
        sh->GetRmax(), sh->GetStOut()*units::deg };
  }
  template <> vector<double> dimensions<TGeoSphere>(const TGeoShape* shape)    {
    const TGeoSphere* sh = get_ptr<TGeoSphere>(shape);
    return { sh->GetRmin(), sh->GetRmax(),
        sh->GetTheta1()*units::deg, sh->GetTheta2()*units::deg,
        sh->GetPhi1()*units::deg,   sh->GetPhi2()*units::deg };
  }
  template <> vector<double> dimensions<TGeoTorus>(const TGeoShape* shape)    {
    const TGeoTorus* sh = get_ptr<TGeoTorus>(shape);
    return { sh->GetR(), sh->GetRmin(), sh->GetRmax(),
        sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg };
  }
  template <> vector<double> dimensions<TGeoPgon>(const TGeoShape* shape)    {
    const TGeoPgon* sh = get_ptr<TGeoPgon>(shape);
    vector<double> pars { sh->GetPhi1()*units::deg, sh->GetDphi()*units::deg, double(sh->GetNedges()), double(sh->GetNz()) };
    pars.reserve(4+3*sh->GetNz());
    for (Int_t i = 0; i < sh->GetNz(); ++i) {
      pars.emplace_back(sh->GetZ(i));
      pars.emplace_back(sh->GetRmin(i));
      pars.emplace_back(sh->GetRmax(i));
    }
    return pars;
  }
  template <> vector<double> dimensions<TGeoXtru>(const TGeoShape* shape)    {
    const TGeoXtru* sh = get_ptr<TGeoXtru>(shape);
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
  template <> vector<double> dimensions<TGeoArb8>(const TGeoShape* shape)    {
    TGeoArb8* sh = get_ptr<TGeoArb8>(shape);
    struct _V { double xy[8][2]; } *vtx = (_V*)sh->GetVertices();
    vector<double> pars { sh->GetDz() };
    for ( size_t i=0; i<8; ++i )   {
      pars.emplace_back(vtx->xy[i][0]);
      pars.emplace_back(vtx->xy[i][1]);
    }
    return pars;
  }
  template <> vector<double> dimensions<TGeoTrap>(const TGeoShape* shape)    {
    const TGeoTrap* sh = get_ptr<TGeoTrap>(shape);
    return { sh->GetDz(), sh->GetTheta()*units::deg, sh->GetPhi()*units::deg,
        sh->GetH1(), sh->GetBl1(), sh->GetTl1(), sh->GetAlpha1()*units::deg,
        sh->GetH2(), sh->GetBl2(), sh->GetTl2(), sh->GetAlpha2()*units::deg };
  }
  template <> vector<double> dimensions<TGeoGtra>(const TGeoShape* shape)    {
    const TGeoGtra* sh = get_ptr<TGeoGtra>(shape);
    return { sh->GetDz(), sh->GetTheta()*units::deg, sh->GetPhi()*units::deg,
        sh->GetH1(), sh->GetBl1(), sh->GetTl1(), sh->GetAlpha1()*units::deg,
        sh->GetH2(), sh->GetBl2(), sh->GetTl2(), sh->GetAlpha2()*units::deg,
        sh->GetTwistAngle()*units::deg };
  }
  template <> vector<double> dimensions<TGeoScaledShape>(const TGeoShape* shape)    {
    TGeoScaledShape* sh = get_ptr<TGeoScaledShape>(shape);
    TGeoShape*       s_sh = sh->GetShape();
    const Double_t*  scale = sh->GetScale()->GetScale();
    vector<double>   pars {scale[0],scale[1],scale[2]};
    vector<double>   s_pars = get_shape_dimensions(s_sh);
    for(auto p : s_pars) pars.push_back(p);
    return pars;
  }
  template <> vector<double> dimensions<TGeoCompositeShape>(const TGeoShape* shape)    {
    const TGeoCompositeShape* sh = get_ptr<TGeoCompositeShape>(shape);
    const TGeoBoolNode*  boolean = sh->GetBoolNode();
    TGeoMatrix*    right_matrix  = boolean->GetRightMatrix();
    TGeoShape*     left_solid    = boolean->GetLeftShape();
    TGeoShape*     right_solid   = boolean->GetRightShape();

    TGeoBoolNode::EGeoBoolType oper = boolean->GetBooleanOperator();
    TGeoMatrix*   left_matrix   = boolean->GetRightMatrix();
    const Double_t*   left_tr   = left_matrix->GetTranslation();
    const Double_t*   left_rot  = left_matrix->GetRotationMatrix();
    const Double_t*   right_tr  = right_matrix->GetTranslation();
    const Double_t*   right_rot = right_matrix->GetRotationMatrix();

    vector<double>    pars { double(oper) };
    vector<double>    left_par  = Solid(left_solid).dimensions();
    vector<double>    right_par = Solid(right_solid).dimensions();

    pars.insert(pars.end(), left_par.begin(), left_par.end());
    pars.insert(pars.end(), left_rot, left_rot+9);
    pars.insert(pars.end(), left_tr, left_tr+3);

    pars.insert(pars.end(), right_par.begin(), right_par.end());
    pars.insert(pars.end(), right_rot, right_rot+9);
    pars.insert(pars.end(), right_tr, right_tr+3);
    return pars;
  }

  template <typename T> vector<double> dimensions(const Handle<TGeoShape>& shape)
  {  return dimensions<typename T::Object>(get_ptr<typename T::Object>(shape.ptr()));  }
  template vector<double> dimensions<ShapelessSolid>   (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Box>              (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<HalfSpace>        (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Polycone>         (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<ConeSegment>      (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Tube>             (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<CutTube>          (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<TwistedTube>      (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<EllipticalTube>   (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Cone>             (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Trap>             (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Trd1>             (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Trd2>             (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Torus>            (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Sphere>           (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Paraboloid>       (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Hyperboloid>      (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<PolyhedraRegular> (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<Polyhedra>        (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<ExtrudedPolygon>  (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<EightPointSolid>  (const Handle<TGeoShape>& shape); 
  template vector<double> dimensions<BooleanSolid>     (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<SubtractionSolid> (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<UnionSolid>       (const Handle<TGeoShape>& shape);
  template vector<double> dimensions<IntersectionSolid>(const Handle<TGeoShape>& shape);

  template <> vector<double> dimensions<PseudoTrap>(const Handle<TGeoShape>& shape)   {
    const TGeoCompositeShape* sh = get_ptr<TGeoCompositeShape>(shape.ptr());
    TGeoMatrix*    right_matrix  = sh->GetBoolNode()->GetRightMatrix();
    stringstream params(right_matrix->GetTitle());
    vector<double> pars;
    pars.reserve(7);
#ifdef DIMENSION_DEBUG
    cout << "dimensions: [" << PSEUDOTRAP_TAG << "]" << endl
         << right_matrix->GetTitle() << endl;
#endif
    for(size_t i=0; i<7; ++i)   {
      double val;
      params >> val;
      pars.emplace_back(val);
      if ( !params.good() )   {
        except("PseudoTrap","+++ dimensions: Invalid parameter stream.");
      }
    }
    return pars;
  }

  template <> vector<double> dimensions<TruncatedTube>(const Handle<TGeoShape>& shape)   {
    const TGeoCompositeShape* sh = get_ptr<TGeoCompositeShape>(shape.ptr());
    TGeoMatrix* right_matrix  = sh->GetBoolNode()->GetRightMatrix();
    stringstream params(right_matrix->GetTitle());
    vector<double> pars;
    pars.reserve(8);
    for(size_t i=0; i<8; ++i)   {
      double val;
      params >> val;
      pars.emplace_back(val);
      if ( !params.good() )   {
        except("TruncatedTube","+++ dimensions: Invalid parameter stream.");
      }
    }
    return pars;
  }
  
  template <> vector<double> dimensions<Solid>(const Handle<TGeoShape>& shape)   {
    if ( shape.ptr() )   {
      TClass* cl = shape->IsA();
      if (cl == TGeoShapeAssembly::Class())
        return dimensions<TGeoShapeAssembly>(shape.ptr());
      else if (cl == TGeoBBox::Class())
        return dimensions<TGeoBBox>(shape.ptr());
      else if (cl == TGeoHalfSpace::Class())
        return dimensions<TGeoHalfSpace>(shape.ptr());
      else if (cl == TGeoPgon::Class())
        return dimensions<TGeoPgon>(shape.ptr());
      else if (cl == TGeoPcon::Class())
        return dimensions<TGeoPcon>(shape.ptr());
      else if (cl == TGeoConeSeg::Class())
        return dimensions<TGeoConeSeg>(shape.ptr());
      else if (cl == TGeoCone::Class())
        return dimensions<TGeoCone>(shape.ptr());
      else if (cl == TGeoTube::Class())
        return dimensions<TGeoTube>(shape.ptr());
      else if (cl == TGeoTubeSeg::Class())
        return dimensions<TGeoTubeSeg>(shape.ptr());
      else if (cl == TGeoCtub::Class())
        return dimensions<TGeoCtub>(shape.ptr());
      else if (cl == TGeoEltu::Class())
        return dimensions<TGeoEltu>(shape.ptr());
      else if (cl == TwistedTubeObject::Class())
        return dimensions<TwistedTubeObject>(shape.ptr());
      else if (cl == TGeoTrd1::Class())
        return dimensions<TGeoTrd1>(shape.ptr());
      else if (cl == TGeoTrd2::Class())
        return dimensions<TGeoTrd2>(shape.ptr());
      else if (cl == TGeoParaboloid::Class())
        return dimensions<TGeoParaboloid>(shape.ptr());
      else if (cl == TGeoHype::Class())
        return dimensions<TGeoHype>(shape.ptr());
      else if (cl == TGeoGtra::Class())
        return dimensions<TGeoGtra>(shape.ptr());
      else if (cl == TGeoTrap::Class())
        return dimensions<TGeoTrap>(shape.ptr());
      else if (cl == TGeoArb8::Class())
        return dimensions<TGeoArb8>(shape.ptr());
      else if (cl == TGeoSphere::Class())
        return dimensions<TGeoSphere>(shape.ptr());
      else if (cl == TGeoTorus::Class())
        return dimensions<TGeoTorus>(shape.ptr());
      else if (cl == TGeoXtru::Class())
        return dimensions<TGeoXtru>(shape.ptr());
      else if (cl == TGeoScaledShape::Class())
        return dimensions<TGeoScaledShape>(shape.ptr());
      else if (isA<TruncatedTube>(shape.ptr()))
        return dimensions<TruncatedTube>(shape);
      else if (isA<PseudoTrap>(shape.ptr()))
        return dimensions<PseudoTrap>(shape);
      else if (cl == TGeoCompositeShape::Class())
        return dimensions<TGeoCompositeShape>(shape.ptr());
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
  vector<double> get_shape_dimensions(const Handle<TGeoShape>& shape)    {
    return dimensions<Solid>(shape);
  }
  /// Access Shape dimension parameters (As in TGeo, but angles in radians rather than degrees)
  vector<double> get_shape_dimensions(TGeoShape* shape)   {
    return dimensions<Solid>(Solid(shape));
  }

  template <typename T> void set_dimensions(T shape, const std::vector<double>& )    {
    stringstream str;
    if ( shape )
      str << "Shape: set_dimension(" << typeName(typeid(*shape)) << "): Invalid call!";
    else
      str << "Shape: set_dimension<TGeoShape): Invalid call && pointer!";
    throw runtime_error(str.str());
  }

  template <> void set_dimensions(TGeoShapeAssembly* , const std::vector<double>& )    {
    printout(WARNING,"ShapelessSolid","+++ setDimensions is a compatibility call. Nothing implemented.");
  }
  template <> void set_dimensions(TGeoBBox* sh, const std::vector<double>& params)    {
    auto pars = params;
    if ( pars.size() != 3 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoHalfSpace* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 6 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoPcon* sh, const std::vector<double>& params)   {
    if ( params.size() < 3 )   {
      invalidSetDimensionCall(sh,params);
    }
    size_t nz = size_t(params[2]);
    if ( params.size() != 3 + 3*nz )   {
      invalidSetDimensionCall(sh,params);
    }
    vector<double> pars = params;
    pars[0] /= units::deg;
    pars[1] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoConeSeg* sh, const std::vector<double>& params)   {
    if ( params.size() != 7 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    pars[5] /= units::deg;
    pars[6] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoCone* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 5 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoTube* sh, const std::vector<double>& params)   {
    if ( params.size() != 3 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoTubeSeg* sh, const std::vector<double>& params)   {
    if ( params.size() != 5 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    pars[3] /= units::deg;
    pars[4] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TwistedTubeObject* sh, const std::vector<double>& params)   {
    if ( params.size() != 7 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    sh->fPhiTwist = pars[0]/units::deg;
    sh->fNegativeEndz = pars[3];
    sh->fPositiveEndz = pars[4];
    sh->fNsegments    = (int)pars[5];

    pars[0] = pars[1];
    pars[1] = pars[2];
    pars[2] = (-pars[3]+pars[4])/2.0;
    pars[3] = 0.0;
    pars[4] = pars[6];
    pars.resize(5);
    set_dimensions((TGeoTubeSeg*)sh, pars);
  }
  template <> void set_dimensions(TGeoCtub* sh, const std::vector<double>& params)   {
    if ( params.size() != 11 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    pars[3] /= units::deg;
    pars[4] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoEltu* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 3 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoTrd1* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 4 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoTrd2* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 5 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoParaboloid* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 3 )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoHype* sh, const std::vector<double>& params)   {
    if ( params.size() != 5 )   {
      invalidSetDimensionCall(sh,params);
    }
    double pars[] = { params[0], params[1], params[2]/units::deg, params[3], params[4]/units::deg };
    Solid(sh)._setDimensions(pars);
  }
  template <> void set_dimensions(TGeoSphere* sh, const std::vector<double>& params)   {
    if ( params.size() < 2 )   {
      invalidSetDimensionCall(sh,params);
    }
    double pars[] = { params[0], params[1], 0.0, M_PI/units::deg, 0.0, 2*M_PI/units::deg };
    if (params.size() > 2) pars[2] = params[2]/units::deg;
    if (params.size() > 3) pars[3] = params[3]/units::deg;
    if (params.size() > 4) pars[4] = params[4]/units::deg;
    if (params.size() > 5) pars[5] = params[5]/units::deg;
    Sphere(sh).access()->SetDimensions(pars, params.size());
    sh->ComputeBBox();
  }
  template <> void set_dimensions(TGeoTorus* sh, const std::vector<double>& params)   {
    if ( params.size() != 5 )   {
      invalidSetDimensionCall(sh,params);
    }
    double pars[] = { params[0], params[1], params[2], params[3]/units::deg, params[4]/units::deg };
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoPgon* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() < 4 || params.size() != 4 + 3*size_t(params[3]) )   {
      invalidSetDimensionCall(sh,params);
    }
    pars[0]  /= units::deg;
    pars[1]  /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoXtru* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() < 1 || params.size() != 1 + 4*size_t(params[0]) )   {
      invalidSetDimensionCall(sh,params);
    }
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoArb8* sh, const std::vector<double>& params)   {
    if ( params.size() != 17 )   {
      invalidSetDimensionCall(sh,params);
    }
    auto pars = params;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoTrap* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 11 )   {
      invalidSetDimensionCall(sh,params);
    }
    pars[1]  /= units::deg;
    pars[2]  /= units::deg;
    pars[6]  /= units::deg;
    pars[10] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoGtra* sh, const std::vector<double>& params)   {
    auto pars = params;
    if ( params.size() != 12 )   {
      invalidSetDimensionCall(sh,params);
    }
    pars[1]  /= units::deg;
    pars[2]  /= units::deg;
    pars[6]  /= units::deg;
    pars[10] /= units::deg;
    pars[11] /= units::deg;
    Solid(sh)._setDimensions(&pars[0]);
  }
  template <> void set_dimensions(TGeoScaledShape* sh, const std::vector<double>& params)   {
    Solid            s_sh(sh->GetShape());
    sh->GetScale()->SetScale(params[0], params[1], params[2]);
    auto pars = params;
    s_sh.access()->SetDimensions(&pars[3]);
  }
  template <> void set_dimensions(TGeoCompositeShape*, const std::vector<double>&)   {
    // In general TGeoCompositeShape instances have an empty SetDimension call
#if 0
    TGeoCompositeShape* sh      = (TGeoCompositeShape*) shape;
    TGeoBoolNode* boolean       = sh->GetBoolNode();
    TGeoMatrix*   right_matrix  = boolean->GetRightMatrix();
    TGeoShape*    left_solid    = boolean->GetLeftShape();
    TGeoShape*    right_solid   = boolean->GetRightShape();
    auto pars = params;
    Solid(sh)._setDimensions(&pars[0]);
#endif
#ifdef DIMENSION_DEBUG
    throw runtime_error("Composite shape. setDimensions is not implemented!");
#endif
  }

  template <> void set_dimensions(ShapelessSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Box shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(HalfSpace shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Polycone shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Cone shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(ConeSegment shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Tube shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(CutTube shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(TwistedTube shape, const std::vector<double>& params)
  {  set_dimensions((TwistedTubeObject*)shape.ptr(), params);   }
  template <> void set_dimensions(EllipticalTube shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Trap shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Trd1 shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Trd2 shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Paraboloid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Hyperboloid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Sphere shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Torus shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(PolyhedraRegular shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(Polyhedra shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(ExtrudedPolygon shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(EightPointSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(BooleanSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(SubtractionSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(UnionSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }
  template <> void set_dimensions(IntersectionSolid shape, const std::vector<double>& params)
  {  set_dimensions(shape.ptr(), params);   }

  template <> void set_dimensions(TruncatedTube shape, const std::vector<double>& params)  {
    TGeoCompositeShape* sh = (TGeoCompositeShape*) shape.ptr();
    TGeoBoolNode* boolean = sh->GetBoolNode();
    TGeoMatrix* right_matrix  = boolean->GetRightMatrix();
    TGeoShape*  left_solid    = boolean->GetLeftShape();
    TGeoShape*  right_solid   = boolean->GetRightShape();
    TGeoTubeSeg* tubs = (TGeoTubeSeg*)left_solid;
    TGeoBBox*    box  = (TGeoBBox*)right_solid;
    double zhalf = params[0];
    double rmin  = params[1];
    double rmax  = params[2];
    double startPhi = params[3]/units::deg;
    double deltaPhi = params[4]/units::deg;
    double cutAtStart = params[5];
    double cutAtDelta = params[6];
    bool   cutInside  = params[7] > 0.5;
#ifdef DIMENSION_DEBUG
    cout << "setDimensions: [" << TRUNCATEDTUBE_TAG << "]" << endl
         << right_matrix->GetTitle() << endl;
#endif
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
    rot.RotateZ( -alpha/units::deg );
    double box_dim[] = {boxX, boxY, boxZ};
    double tub_dim[] = {rmin, rmax, zhalf, startPhi, deltaPhi};
    box->SetDimensions(box_dim);
    tubs->SetDimensions(tub_dim);
    TGeoCombiTrans* combi = (TGeoCombiTrans*)right_matrix;
    combi->SetRotation(rot);
    combi->SetTranslation(xBox, 0, 0);
    return;
  }
  template <> void set_dimensions(PseudoTrap shape, const std::vector<double>& params)  {
    TGeoCompositeShape* sh = (TGeoCompositeShape*) shape.ptr();
    TGeoBoolNode* boolean = sh->GetBoolNode();
    TGeoMatrix* right_matrix  = boolean->GetRightMatrix();
    TGeoShape*  left_solid    = boolean->GetLeftShape();
    TGeoShape*  right_solid   = boolean->GetRightShape();
    double x1 = params[0];
    double x2 = params[1];
    double y1 = params[2];
    double y2 = params[3];
    double z  = params[4];
    double r  = params[5];
    bool   atMinusZ = params[6] > 0.5;
    double x            = atMinusZ ? x1 : x2;
    double h            = 0;
    bool   intersec     = false; // union or intersection solid
    double displacement = 0;
    double startPhi     = 0;
    double halfZ        = z;
    double halfOpeningAngle = std::asin( x / std::abs( r ))/units::deg;
    /// calculate the displacement of the tubs w.r.t. to the trap, determine the opening angle of the tubs
    double delta        = std::sqrt( r * r - x * x );

#ifdef DIMENSION_DEBUG
    cout << "setDimensions: [" << PSEUDOTRAP_TAG << "]" << endl
         << right_matrix->GetTitle() << endl;
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
    double trd2_dim[] = { x1, x2, y1, y2, halfZ };
    double tube_dim[] = { 0.0, std::abs(r), h, startPhi, startPhi + halfOpeningAngle*2. };
    if( intersec && left_solid->IsA() == TGeoTrd2::Class() )   {
      left_solid->SetDimensions(trd2_dim);
      right_solid->SetDimensions(tube_dim);
    }
    else if ( right_solid->IsA() == TGeoCompositeShape::Class() )   {
      double box_dim[] = { 1.1*x, 1.1*h, std::sqrt(r*r-x*x) };
      TGeoCompositeShape* comp = (TGeoCompositeShape*)right_solid;
      TGeoBoolNode* b_s = comp->GetBoolNode();
      b_s->GetLeftShape()->SetDimensions(tube_dim);
      b_s->GetRightShape()->SetDimensions(box_dim);
      left_solid->SetDimensions(trd2_dim);
    }
    else  {
      except("PseudoTrap","+++ Incompatible change of parameters.");
    }
    ((TGeoTranslation*)right_matrix)->SetTranslation(0,0,displacement);
    stringstream str;
    str << x1 << " " << x2 << " " << y1 << " " << y2 << " " << z << " "
        << r << " " << char(atMinusZ ? '1' : '0') << " ";
    right_matrix->SetTitle(str.str().c_str());
  }

  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  template <> void set_dimensions(Solid shape, const std::vector<double>& params)  {
    if ( shape.ptr() ) {
      TClass* cl = shape->IsA();
      Solid solid(shape);
      if (cl == TGeoShapeAssembly::Class())
        set_dimensions(ShapelessSolid(shape), params);
      else if (cl == TGeoBBox::Class())
        set_dimensions(Box(shape), params);
      else if (cl == TGeoHalfSpace::Class())
        set_dimensions(HalfSpace(shape), params);
      else if (cl == TGeoPcon::Class())
        set_dimensions(Polycone(shape), params);
      else if (cl == TGeoConeSeg::Class())
        set_dimensions(ConeSegment(shape), params);
      else if (cl == TGeoCone::Class())
        set_dimensions(Cone(shape), params);
      else if (cl == TGeoTube::Class())
        set_dimensions(Tube(shape), params);
      else if (cl == TGeoTubeSeg::Class())
        set_dimensions(Tube(shape), params);
      else if (cl == TGeoCtub::Class())
        set_dimensions(CutTube(shape), params);
      else if (cl == TGeoEltu::Class())
        set_dimensions(EllipticalTube(shape), params);
      else if (cl == TwistedTubeObject::Class())
        set_dimensions(TwistedTube(shape), params);
      else if (cl == TGeoTrd1::Class())
        set_dimensions(Trd1(shape), params);
      else if (cl == TGeoTrd2::Class())
        set_dimensions(Trd2(shape), params);
      else if (cl == TGeoParaboloid::Class())
        set_dimensions(Paraboloid(shape), params);
      else if (cl == TGeoHype::Class())
        set_dimensions(Hyperboloid(shape), params);
      else if (cl == TGeoSphere::Class())
        set_dimensions(Sphere(shape), params);
      else if (cl == TGeoTorus::Class())
        set_dimensions(Torus(shape), params);
      else if (cl == TGeoTrap::Class())
        set_dimensions(Trap(shape), params);
      else if (cl == TGeoPgon::Class())
        set_dimensions(PolyhedraRegular(shape), params);
      else if (cl == TGeoXtru::Class())
        set_dimensions(ExtrudedPolygon(shape), params);
      else if (cl == TGeoArb8::Class())
        set_dimensions(EightPointSolid(shape), params);
      else if (cl == TGeoScaledShape::Class())  {
        TGeoScaledShape* sh = (TGeoScaledShape*) shape.ptr();
        set_dimensions(sh, params);
      }
      else if ( isA<TruncatedTube>(shape) )
        set_dimensions(TruncatedTube(shape), params);
      else if ( isA<PseudoTrap>(shape) )
        set_dimensions(PseudoTrap(shape), params);
      else if ( isA<SubtractionSolid>(solid) )
        set_dimensions(SubtractionSolid(shape), params);
      else if ( isA<UnionSolid>(solid) )
        set_dimensions(UnionSolid(shape), params);
      else if ( isA<IntersectionSolid>(solid) )
        set_dimensions(IntersectionSolid(shape), params);
      else if ( isA<BooleanSolid>(solid) )
        set_dimensions(BooleanSolid(shape), params);
      else
        printout(ERROR,"Solid","Failed to set dimensions for shape of type:%s.",cl->GetName());
      return;
    }
    except("Solid","set_shape_dimensions: Failed to set dimensions [Invalid handle].");
  }
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  template <> void set_dimensions(const TGeoShape* shape, const std::vector<double>& params)  {
    set_dimensions<Solid>(Solid(shape), params);
  }
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  void set_shape_dimensions(TGeoShape* shape, const vector<double>& params)   {
    set_dimensions<Solid>(Solid(shape), params);
  }
  /// Set the shape dimensions. As for the TGeo shape, but angles in rad rather than degrees.
  void set_shape_dimensions(Solid solid, const vector<double>& params)   {
    set_dimensions<Solid>(solid, params);
  }

  /// Pretty print of solid attributes
  string toStringSolid(const TGeoShape* shape, int precision)   {
    if ( !shape )  {
      return "[Invalid shape]";
    }

    stringstream log;
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
      const TGeoShape* left  = boolean->GetLeftShape();
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
  string toStringMesh(const TGeoShape* shape, int prec)  {
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
}

