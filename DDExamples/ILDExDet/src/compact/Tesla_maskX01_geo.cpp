// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
#define _U(text)  Unicode(#text)
namespace {
  typedef enum {          // These constants are also used in the MySQL database:
    kCenter          = 0, // centered on the z-axis
    kUpstream        = 1, // on the upstream branch, rotated by half the crossing angle
    kDnstream        = 2, // on the downstream branch, rotated by half the crossing angle
    kPunchedCenter   = 3, // centered, with one or two inner holes
    kPunchedUpstream = 4, // on the upstream branch, with two inner holes
    kPunchedDnstream = 5  // on the downstream branch, with two inner holes
  } ECrossType;
}

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(M_PI,0,0);
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name);
  double      crossingAngle = x_det.attr<double>(_U(crossing_angle));

  PlacedVolume pv;
  struct dim_t { double inner_r, outer_r, z; };
  for(xml_coll_t c(x_det,_U(component)); c; ++c)  {
    xml_comp_t x_c = c;
    xml_comp_t x_end = c.child(_A(end));
    xml_comp_t x_start = c.child(_A(start));
    double angle = 0;
    string nam = x_c.nameStr();
    int crossType = x_c.attr<int>(_U(crossType));
    Material mat = lcdd.material(x_c.materialStr());
    VisAttr  vis = lcdd.visAttributes(x_c.visStr());
    dim_t  end   = { x_end.inner_r(), x_end.outer_r(), x_end.z()};
    dim_t  start = { x_start.inner_r(), x_start.outer_r(), x_start.z()};
    double zHalf = std::fabs(start.z - end.z) / 2; // half z length of the cone
    double zPos  = std::fabs(start.z + end.z) / 2; // middle z position

    switch (crossType) {
    case kUpstream:
    case kPunchedUpstream:
      angle = -crossingAngle;
      break;
    case kDnstream:
    case kPunchedDnstream:
      angle = +crossingAngle;
      break;
    default:
      angle = 0;
      break;
    }
    double reflect_angle = M_PI - angle;
    switch (crossType) {
    case kCenter:
    case kUpstream:
    case kDnstream: {   // Create a solid with the given dimensions
      ConeSegment cone(zHalf, start.inner_r, start.outer_r, end.inner_r, end.outer_r);
      Volume      vol (name+"_"+nam,cone,mat);
      vol.setVisAttributes(vis);
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(0,reflect_angle,0));
      break;
    }
    case kPunchedCenter: {  // a cone with one or two inner holes (two tubes are punched out)        
      // the main solid and the two pieces (only tubes, for the moment) which will be punched out
      ConeSegment cone(zHalf,0,start.outer_r,0,end.outer_r);
      Solid solid1 = cone, solid2 = cone;
      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      if ( start.inner_r ) { // do we need a hole on the upstream branch?
	Tube punch(0,start.inner_r,2*zHalf);
	solid1 = SubtractionSolid(cone,punch,Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
	solid2 = SubtractionSolid(cone,punch,Position(zPos*std::tan(+crossingAngle),0,0),Rotation(0,+crossingAngle,0));
      }
          
      if ( end.inner_r ) { // do we need a hole on the downstream branch?
	Tube punch(0,end.inner_r,2*zHalf);
	solid1 = SubtractionSolid(solid1,punch,Position(zPos*std::tan(+crossingAngle),0,0),Rotation(0,+crossingAngle,0));
	solid2 = SubtractionSolid(solid2,punch,Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
      }
      Volume vol(name+"_"+nam+"_pos",solid1,mat);
      vol.setVisAttributes(vis);
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
      vol = Volume(name+"_"+nam+"_neg",solid2,mat);
      vol.setVisAttributes(vis);
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(0,reflect_angle,0));
      break;
    }
    case kPunchedUpstream:
    case kPunchedDnstream: {
      // a volume on the upstream or downstream branch with two inner holes
      // (implemented as a cone from which another tube is punched out)        
      double rCenterPunch = (crossType == kPunchedUpstream) ? start.inner_r : end.inner_r; // radius of the central hole
      double rOffsetPunch = (crossType == kPunchedDnstream) ? start.inner_r : end.inner_r; // radius of the off-axis hole
      // the main solid and the piece (only a tube, for the moment) which will be punched out
      ConeSegment whole(rCenterPunch,start.outer_r,rCenterPunch,end.outer_r,zHalf);
      Tube        punch(0,rOffsetPunch,2*zHalf);    
      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      SubtractionSolid solid(whole, punch, Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
      Volume vol(name+"_"+nam+"_pos",solid,mat);
      vol.setVisAttributes(vis);
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));

      solid = SubtractionSolid(whole, punch, Position(zPos*std::tan(crossingAngle),0,0),Rotation(0,crossingAngle,0));
      vol = Volume(name+"_"+nam+"_neg",solid,mat);
      vol.setVisAttributes(vis);
      assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(0,reflect_angle,0));
      break;
    }
    default: {
      cout << "MaskX01: Unimplemented \"crossType\" code:" << crossType << endl;
      throw runtime_error("MaskX01: Unimplemented \"crossType\" code.");
      break;
    }
    }
  }
  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_maskX01,create_element);
