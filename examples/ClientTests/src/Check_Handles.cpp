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
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"
#include "DD4hep/World.h"

using namespace dd4hep;
using namespace dd4hep::detail;

namespace {
  typedef std::pair<int,int> Result;
  std::pair<int,int>& operator += (std::pair<int,int>& left, const std::pair<int,int>& right)  {
    left.first  += right.first;
    left.second += right.second;
    return left;
  }
  template <typename T, typename Q> Result test_handle(T& from)  {
    Result r(0,0);
    /// Test handle copy constructors
    try  {
      Q to(from);
      if ( to.ptr() && from.ptr() )  {
        printout(INFO,"Check_Handles","PASSED: Handle Construction from %s to %s.",
                 typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
        ++r.first;
        T ffrom(to);
        if ( to.ptr() && ffrom.ptr() )  {
          printout(INFO,"Check_Handles","PASSED: Handle Construction from %s to %s.",
                   typeName(typeid(Q)).c_str(),typeName(typeid(T)).c_str());
          ++r.first;
        }
      }
      else if ( !to.ptr() && from.ptr() )   {
        printout(INFO,"Check_Handles","FAILED: Handle Construction from %s to %s.",
                 typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
        ++r.second;
      }
    }
    catch(const std::exception& e)   {
      printout(INFO,"Check_Handles",  "FAILED: Handle Construction from %s to %s. [Exception]",
               typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
      ++r.second;
    }
    /// Test handle assignments
    try  {
      Q to = from;
      if ( to.ptr() && from.ptr() )  {
        printout(INFO,"Check_Handles","PASSED: Handle Assignment   from %s to %s.",
                 typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
        ++r.first;
        T ffrom = to;
        if ( to.ptr() && ffrom.ptr() )  {
          printout(INFO,"Check_Handles","PASSED: Handle Construction from %s to %s.",
                   typeName(typeid(Q)).c_str(),typeName(typeid(T)).c_str());
          ++r.first;
        }
      }
      else if ( !to.ptr() && from.ptr() )   {
        printout(INFO,"Check_Handles","FAILED: Handle Assignment   from %s to %s.",
                 typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
        ++r.second;
      }
    }
    catch(const std::exception& e)   {
      printout(INFO,"Check_Handles","FAILED: Handle Assignment     from %s to %s. [Exception]",
               typeName(typeid(T)).c_str(),typeName(typeid(Q)).c_str());
      ++r.second;
    }
    return r;
  }
  template <typename T> Result check_handle(T& h)  {
    Result r(0,0);
    r += test_handle<T, T>(h);
    r += test_handle<T, Handle<typename T::Object> >(h);
    r += test_handle<T, Handle<NamedObject> >(h);
    r += test_handle<T, Handle<TObject> >(h);
    r += test_handle<T, Handle<TNamed> >(h);
    return r;
  }
  Result check_handle(Solid& h)  {
    Result r(0,0);
    r += test_handle<Solid, Solid>(h);
    r += test_handle<Solid, Box>(h);
    r += test_handle<Solid, HalfSpace>(h);
    r += test_handle<Solid, Polycone>(h);
    r += test_handle<Solid, ConeSegment>(h);
    r += test_handle<Solid, Tube>(h);
    r += test_handle<Solid, EllipticalTube>(h);
    r += test_handle<Solid, Cone>(h);
    r += test_handle<Solid, Trap>(h);
    r += test_handle<Solid, Trapezoid>(h);
    r += test_handle<Solid, Paraboloid>(h);
    r += test_handle<Solid, Hyperboloid>(h);
    r += test_handle<Solid, PolyhedraRegular>(h);
    r += test_handle<Solid, Torus>(h);
    r += test_handle<Solid, Sphere>(h);
    r += test_handle<Solid, EightPointSolid>(h);
    r += test_handle<Solid, SubtractionSolid>(h);
    r += test_handle<Solid, UnionSolid>(h);
    r += test_handle<Solid, IntersectionSolid>(h);
    r += test_handle<Solid, Handle<TGeoShape> >(h);
    r += test_handle<Solid, Handle<TGeoBBox> >(h);
    r += test_handle<Solid, Handle<TGeoCone> >(h);
    r += test_handle<Solid, Handle<TGeoArb8> >(h);
    r += test_handle<Solid, Handle<TGeoConeSeg> >(h);
    r += test_handle<Solid, Handle<TGeoParaboloid> >(h);
    r += test_handle<Solid, Handle<TGeoPcon> >(h);
    r += test_handle<Solid, Handle<TGeoHype> >(h);
    r += test_handle<Solid, Handle<TGeoPgon> >(h);
    r += test_handle<Solid, Handle<TGeoTube> >(h);
    r += test_handle<Solid, Handle<TGeoEltu> >(h);
    r += test_handle<Solid, Handle<TGeoTubeSeg> >(h);
    r += test_handle<Solid, Handle<TGeoTrap> >(h);
    r += test_handle<Solid, Handle<TGeoTrd1> >(h);
    r += test_handle<Solid, Handle<TGeoTrd2> >(h);
    r += test_handle<Solid, Handle<TGeoSphere> >(h);
    r += test_handle<Solid, Handle<TGeoTorus> >(h);
    r += test_handle<Solid, Handle<TGeoHalfSpace> >(h);
    r += test_handle<Solid, Handle<TGeoShapeAssembly> >(h);
    r += test_handle<Solid, Handle<TGeoCompositeShape> >(h);
    return r;
  }
  Result check_handle(Volume& h)  {
    Result r(0,0);
    r += test_handle<Volume, Volume>(h);
    r += test_handle<Volume, Assembly>(h);
    r += test_handle<Volume, Handle<TGeoAtt> >(h);
    r += test_handle<Volume, Handle<NamedObject> >(h);
    r += test_handle<Volume, Handle<TObject> >(h);
    r += test_handle<Volume, Handle<TNamed> >(h);
    r += test_handle<Volume, Handle<TGeoAtt> >(h);
    r += test_handle<Volume, Handle<TAttLine> >(h);
    r += test_handle<Volume, Handle<TAtt3D> >(h);
    return r;
  }
  Result check_handle(Assembly& h)  {
    Result r(0,0);
    r += test_handle<Assembly, Assembly>(h);
    r += test_handle<Assembly, Volume>(h);
    r += test_handle<Assembly, Handle<TGeoAtt> >(h);
    r += test_handle<Assembly, Handle<NamedObject> >(h);
    r += test_handle<Assembly, Handle<TObject> >(h);
    r += test_handle<Assembly, Handle<TNamed> >(h);
    r += test_handle<Assembly, Handle<TGeoAtt> >(h);
    r += test_handle<Assembly, Handle<TAttLine> >(h);
    r += test_handle<Assembly, Handle<TAtt3D> >(h);
    return r;
  }
  Result check_handle(PlacedVolume& h)  {
    Result r(0,0);
    r += test_handle<PlacedVolume, PlacedVolume>(h);
    r += test_handle<PlacedVolume, Handle<NamedObject> >(h);
    r += test_handle<PlacedVolume, Handle<TObject> >(h);
    r += test_handle<PlacedVolume, Handle<TNamed> >(h);
    return r;
  }
}

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  // XML detector object: DDCore/XML/XMLDetector.h
  xml_dim_t  x_det = e;  
  xml_dim_t  x_det_dim(x_det.dimensions());
  DetElement d_det(x_det.nameStr(),x_det.id());
  Assembly   calo_vol(x_det.nameStr()+"_envelope");
  VisAttr    vis = description.visAttributes(x_det.visStr());
  Region     reg = description.region(x_det.regionStr());
  LimitSet   lim = description.limitSet(x_det.limitsStr());
  
  // Declare this sensitive detector as a calorimeter
  Tube tub(x_det_dim.rmin(), x_det_dim.rmax(), x_det_dim.z()/2.0,0.0,2*M_PI);
  Volume tub_vol(x_det.nameStr()+"_tube",tub,description.material("Iron"));
  calo_vol.placeVolume(tub_vol).addPhysVolID("module",1);
  sens.setType("calorimeter");
  tub_vol.setSensitiveDetector(sens);

  // Place the calo inside the world
  PlacedVolume  calo_plv = description.pickMotherVolume(d_det).placeVolume(calo_vol);
  calo_plv.addPhysVolID("system",x_det.id());
  calo_plv.addPhysVolID("barrel",0);
  d_det.setPlacement(calo_plv);
  Result result(0,0);
  Solid  sol = tub;
  result += check_handle(calo_plv);
  result += check_handle(tub_vol);
  result += check_handle(sol);
  result += check_handle(calo_vol);
  result += check_handle(d_det);
  result += check_handle(sens);
  result += check_handle(vis);
  result += check_handle(lim);
  result += check_handle(reg);

  //result += test_handle<Assembly,Region>(calo_vol);
  printout(INFO,"Check_Handles","+-----------------------------------------------------------------+");
  printout(INFO,"Check_Handles","|   %-4d casts PASSED   %4d casts FAILED                         |",
           result.first, result.second);
  printout(INFO,"Check_Handles","+-----------------------------------------------------------------+");
  return d_det;
}
DECLARE_DETELEMENT(DD4hep_Check_Handles,create_detector)
