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
  typedef enum {                // These constants are also used in the MySQL database:
    kCenter               =  0, // centered on the z-axis
    kUpstream             =  1, // on the upstream branch, rotated by half the crossing angle
    kDnstream             =  2, // on the downstream branch, rotated by half the crossing angle

    kPunchedCenter        =  3, // centered, with one or two inner holes
    kPunchedUpstream      =  4, // on the upstream branch, with two inner holes
    kPunchedDnstream      =  5, // on the downstrem branch, with two inner holes

    kUpstreamClippedFront =  6, // upstream, with the front face parallel to the xy-plane
    kDnstreamClippedFront =  7, // downstream, with the front face parallel to the xy-plane
    kUpstreamClippedRear  =  8, // upstream, with the rear face parallel to the xy-plane
    kDnstreamClippedRear  =  9, // downstream, with the rear face parallel to the xy-plane
    kUpstreamClippedBoth  = 10, // upstream, with both faces parallel to the xy-plane
    kDnstreamClippedBoth  = 11, // downstream, with both faces parallel to the xy-plane

    kUpstreamSlicedFront  = 12, // upstream, with the front face parallel to a tilted piece
    kDnstreamSlicedFront  = 13, // downstream, with the front face parallel to a tilted piece
    kUpstreamSlicedRear   = 14, // upstream, with the rear face parallel to a tilted piece
    kDnstreamSlicedRear   = 15, // downstream, with the rear face parallel to a tilted piece
    kUpstreamSlicedBoth   = 16, // upstream, with both faces parallel to a tilted piece
    kDnstreamSlicedBoth   = 17  // downstream, with both faces parallel to a tilted piece
  } ECrossType;
}


static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector&)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(M_PI,0,0);
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name);
  double      crossingAngle = x_det.attr<double>(_U(crossing_angle));
  Material    coreMat   = lcdd.vacuum();
  VisAttr     vacuumVis = lcdd.visAttributes("TubeVacuumVis");

  PlacedVolume pv;
  struct dim_t { double inner_r, outer_r, z; };
  for(xml_coll_t c(x_det.child(_U(tubes)),_U(tube)); c; ++c)  {
    xml_comp_t x_c = c;
    xml_comp_t x_end   = c.child(_A(end));
    xml_comp_t x_start = c.child(_A(start));
    dim_t  end   = { x_end.inner_r(), x_end.outer_r(), x_end.z()};
    dim_t  start = { x_start.inner_r(), x_start.outer_r(), x_start.z()};
    Material mat = lcdd.material(x_c.materialStr());
    VisAttr  vis = lcdd.visAttributes(x_c.visStr());
    int crossType = x_c.attr<int>(_U(crossType));

    double angle = 0;
    string nam = x_c.nameStr();
    double zHalf = std::fabs(start.z - end.z) / 2; // half z length of the cone
    double zPos  = std::fabs(start.z + end.z) / 2; // middle z position

    switch (crossType) {
      case kUpstream:
      case kPunchedUpstream:
      case kUpstreamClippedFront:
      case kUpstreamClippedRear:
      case kUpstreamClippedBoth:
      case kUpstreamSlicedFront:
      case kUpstreamSlicedRear:
      case kUpstreamSlicedBoth:
        angle = -crossingAngle; break;
      case kDnstream:
      case kPunchedDnstream:
      case kDnstreamClippedFront:
      case kDnstreamClippedRear:
      case kDnstreamClippedBoth:
      case kDnstreamSlicedFront:
      case kDnstreamSlicedRear:
      case kDnstreamSlicedBoth:
        angle = +crossingAngle; break;
      default:
        angle = 0; break;
    }

    cout << name << "> Building " << nam << " " << crossType << " " << vis.name() << endl;
    double reflect_angle = M_PI - angle;
    Volume vol, wall;
    SubtractionSolid solid;
    switch (crossType) {
      case kCenter:
      case kUpstream:
      case kDnstream: {
        // a volume on the z-axis, on the upstream branch, or on the downstream branch
	ConeSegment cone(zHalf, 0, start.outer_r, 0, end.outer_r);
	vol = Volume(name+"_"+nam+"_vakuum",cone,lcdd.vacuum());
	vol.setVisAttributes(vacuumVis);
        // if inner and outer radii are equal, then omit the tube wall
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) { // the wall solid: a tubular cone
	  ConeSegment wall(zHalf, start.inner_r, start.outer_r, start.inner_r, end.outer_r);
	  Volume      vol_wall(name+"_"+nam+"_wall",cone,mat);
	  vol.setVisAttributes(vis);
	  vol.placeVolume(vol_wall);
	}
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
        break;
      }
      case kPunchedCenter: {
        // a volume on the z-axis with one or two inner holes
        // (implemented as a cone from which tubes are punched out)
        double rUpstreamPunch = start.inner_r; // just alias names denoting what is meant here
        double rDnstreamPunch = end.inner_r;   // (the database entries are "abused" in this case)
	ConeSegment cone(zHalf, 0, start.outer_r, 0, end.outer_r);
	vol = Volume(name+"_"+nam+"_vakuum",cone,lcdd.vacuum());
	vol.setVisAttributes(vacuumVis);
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));

        // the wall solid and placeholders for possible G4SubtractionSolids
        ConeSegment wallCone(zHalf,0,start.outer_r,0,end.outer_r);
	Solid solid1 = cone, solid2 = cone;
        // the punched subtraction solids can be asymmetric and therefore have to be created twice:
        // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
        // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
        if ( start.inner_r ) { // do we need a hole on the upstream branch?
	  Tube punch(0,start.inner_r,5*zHalf);
	  solid1 = SubtractionSolid(wallCone,punch,Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
	  solid2 = SubtractionSolid(wallCone,punch,Position(zPos*std::tan(+crossingAngle),0,0),Rotation(0,+crossingAngle,0));
	}
        if ( end.inner_r ) { // do we need a hole on the downstream branch?
	  Tube punch(0,start.inner_r,5*zHalf);
	  solid1 = SubtractionSolid(solid1,punch,Position(zPos*std::tan(+crossingAngle),0,0),Rotation(0,+crossingAngle,0));
	  solid2 = SubtractionSolid(solid2,punch,Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
	}
        // the wall consists of the material given in the database
	wall = Volume(name+"_"+nam+"_pos",solid1,mat);
	wall.setVisAttributes(vis);
	assembly.placeVolume(wall,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
	wall = Volume(name+"_"+nam+"_neg",solid2,mat);
	wall.setVisAttributes(vis);
	assembly.placeVolume(wall,Position(0,0,zPos).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
        break;
      }
      case kPunchedUpstream:
      case kPunchedDnstream: {
        // a volume on the upstream or downstream branch with two inner holes
        // (implemented as a cone from which another tube is punched out)
	double rCenterPunch = (crossType == kPunchedUpstream) ? start.inner_r : end.inner_r; // radius of the central hole
	double rOffsetPunch = (crossType == kPunchedDnstream) ? start.inner_r : end.inner_r; // radius of the off-axis hole
	ConeSegment cone(zHalf, 0, start.outer_r, 0, end.outer_r);
	vol = Volume(name+"_"+nam+"_vakuum",cone,lcdd.vacuum());
	vol.setVisAttributes(vacuumVis);

	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(0,reflect_angle,0));

        // the wall solid and the piece (only a tube, for the moment) which will be punched out
	ConeSegment whole(zHalf,rCenterPunch,start.outer_r,rCenterPunch,end.outer_r);
	Tube        punch(0,rOffsetPunch,5*zHalf);    
        // the punched subtraction solids can be asymmetric and therefore have to be created twice:
        // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
        // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
        solid = SubtractionSolid(whole, punch, Position(zPos*std::tan(-crossingAngle),0,0),Rotation(0,-crossingAngle,0));
	wall  = Volume(name+"_"+nam+"_wall_pos",solid,mat);
	wall.setVisAttributes(vis);
	assembly.placeVolume(wall,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));

	solid = SubtractionSolid(whole, punch, Position(zPos*std::tan(crossingAngle),0,0),Rotation(0,crossingAngle,0));
	wall  = Volume(name+"_"+nam+"_wall_neg",solid,mat);
	wall.setVisAttributes(vis);
	assembly.placeVolume(wall,Position(0,0,zPos).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
	break;
      }
      case kUpstreamClippedFront:
      case kDnstreamClippedFront:
      case kUpstreamSlicedFront:
      case kDnstreamSlicedFront: {
        // a volume on the upstream or donwstream branch, but with the front face parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * angle)
        // (implemented as a slightly longer cone from which the end is clipped off)
      
        // the volume which will be used for clipping: a solid tube
        double clipSize = start.outer_r; // the right order of magnitude for the clipping volume (alias name)
	Tube clip(0, 2 * clipSize, clipSize);
        // relative transformations for the composition of the G4SubtractionVolumes
        double clipAngle = (crossType == kUpstreamClippedFront || crossType == kDnstreamClippedFront) ? (angle) : (2 * angle);
        double clipShift = (start.z - clipSize) / cos(clipAngle) - (zPos - clipSize / 2); // question: why is this correct?
	Position clip_pos(0,0,clipShift), clip_pos_pos(clip_pos), clip_pos_neg(clip_pos);
	clip_pos_pos.rotateY(-clipAngle);
	clip_pos_neg.rotateY(+clipAngle);

	ConeSegment wallCone, whole(zHalf+clipSize/2, 0, start.outer_r, 0, end.outer_r);
	solid = SubtractionSolid(whole, clip, clip_pos_pos,Rotation(0,-clipAngle,0));
	wall = Volume(name+"_"+nam+"_pos",solid,lcdd.vacuum());
	wall.setVisAttributes(vacuumVis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  wallCone = ConeSegment(zHalf+clipSize/2,start.inner_r,start.outer_r,end.inner_r,end.outer_r);
	  SubtractionSolid wallSolid(wallCone,clip,clip_pos_pos,Rotation(0,-clipAngle,0));
	  Volume           wall (name+"_"+nam+"_wall_solid",wallSolid,mat);
	  wall.setVisAttributes(vis);
	  wall.placeVolume(wall);
	}
	assembly.placeVolume(wall,Position(0,0,zPos-clipSize/2).rotateY(angle),Rotation(0,angle,0));
	
	solid = SubtractionSolid(whole, clip, clip_pos_neg,Rotation(0,clipAngle,0));
	wall  = Volume(name+"_"+nam+"_wall_neg",solid,lcdd.vacuum());
	wall.setVisAttributes(vis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  SubtractionSolid wallSolid(wallCone,clip, clip_pos_neg, Rotation(0,+clipAngle,0));
	  Volume           wall (name+"_"+nam+"_wall_solid",wallSolid,mat);
	  wall.placeVolume(wall);
	}
	assembly.placeVolume(wall,Position(0,0,zPos - clipSize / 2).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
	break;
      }
      case kUpstreamClippedRear:
      case kDnstreamClippedRear:
      case kUpstreamSlicedRear:
      case kDnstreamSlicedRear: {
        // a volume on the upstream or donwstream branch, but with the rear face parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * angle)
        // (implemented as a slightly longer cone from which the end is clipped off)
      
        // the volume which will be used for clipping: a solid tube
        double clipSize = start.outer_r; // the right order of magnitude for the clipping volume (alias name)
	Tube clip(0, 2 * clipSize, clipSize);
        // relative transformations for the composition of the G4SubtractionVolumes
        double clipAngle = (crossType == kUpstreamClippedRear || crossType == kDnstreamClippedRear) ? (angle) : (2 * angle);
        double clipShift = (end.z + clipSize) / cos(clipAngle) - (zPos + clipSize / 2); // question: why is this correct?
	Position clip_pos(0,0,clipShift), clip_pos_pos(clip_pos), clip_pos_neg(clip_pos);
	clip_pos_pos.rotateY(-clipAngle);
	clip_pos_neg.rotateY(+clipAngle);

	ConeSegment wallCone, whole(zHalf+clipSize/2,0, start.outer_r, 0, end.outer_r);
	solid = SubtractionSolid(whole, clip, clip_pos_pos,Rotation(0,-clipAngle,0));
	vol = Volume(name+"_"+nam+"_pos",solid,lcdd.vacuum());
	vol.setVisAttributes(vacuumVis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  wallCone = ConeSegment(zHalf + clipSize/2,start.inner_r,start.outer_r,end.inner_r,end.outer_r);
	  solid = SubtractionSolid(wallCone,clip,clip_pos_pos,Rotation(0,-clipAngle,0));
	  Volume  wall (name+"_"+nam+"_wall_solid",solid,mat);
	  wall.setVisAttributes(vis);
	  vol.placeVolume(wall);
	}
	assembly.placeVolume(vol,Position(0,0,zPos+clipSize/2).rotateY(angle),Rotation(0,angle,0));
	
	solid = SubtractionSolid(whole, clip, clip_pos_neg,Rotation(0,clipAngle,0));
	vol  = Volume(name+"_"+nam+"_wall_neg",solid,lcdd.vacuum());
	vol.setVisAttributes(vis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  solid = SubtractionSolid(wallCone, clip, clip_pos_neg, Rotation(0,+clipAngle,0));
	  Volume  wall (name+"_"+nam+"_wall_solid",solid,mat);
	  wall.setVisAttributes(vis);
	  vol.placeVolume(wall);
	}
	assembly.placeVolume(vol,Position(0,0,zPos+clipSize/2).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
	break;
      }
      case kUpstreamClippedBoth:
      case kDnstreamClippedBoth:
      case kUpstreamSlicedBoth:
      case kDnstreamSlicedBoth: {
        // a volume on the upstream or donwstream branch, but with both faces parallel to the xy-plane
        // or to a piece tilted in the other direction ("sliced" like a salami with 2 * angle)
        // (implemented as a slightly longer cone from which the end is clipped off)

        // the volume which will be used for clipping: a solid tube
        double clipSize = end.outer_r; // the right order of magnitude for the clipping volume (alias name)
	Tube clip(0, 2 * clipSize, clipSize);
        // relative transformations for the composition of the G4SubtractionVolumes
        double clipAngle = (crossType == kUpstreamClippedBoth || crossType == kDnstreamClippedBoth) ? (angle) : (2 * angle);
        double clipShiftFrnt = (start.z - clipSize) / cos(clipAngle) - zPos;
        double clipShiftRear = (end.z   + clipSize) / cos(clipAngle) - zPos;
        // solid for the tube (including vacuum and wall): a solid cone
	ConeSegment wallCone, whole(zHalf + clipSize, 0, start.outer_r, 0, end.outer_r);

	// Primary side:
	solid = SubtractionSolid(whole, clip, Position(0,0,clipShiftFrnt),Rotation(0,-clipAngle,0));
	solid = SubtractionSolid(whole, clip, Position(0,0,clipShiftRear),Rotation(0,-clipAngle,0));
	vol = Volume(name+"_"+nam+"_pos",solid,lcdd.vacuum());
	vol.setVisAttributes(vacuumVis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  wallCone = ConeSegment(zHalf + clipSize,start.inner_r,start.outer_r,end.inner_r,end.outer_r);
          // clip away the protruding ends
	  solid = SubtractionSolid(wallCone,clip,Position(0,0,clipShiftFrnt),Rotation(0,-clipAngle,0));
	  solid = SubtractionSolid(solid,   clip,Position(0,0,clipShiftRear),Rotation(0,-clipAngle,0));
	  wall  = Volume(name+"_"+nam+"_pos_wall",solid,mat);
	  wall.setVisAttributes(vis);
	  vol.placeVolume(wall);
	}
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(angle),Rotation(0,angle,0));

	// Mirror side:
	solid = SubtractionSolid(whole, clip, Position(0,0,clipShiftFrnt),Rotation(0,+clipAngle,0));
	solid = SubtractionSolid(whole, clip, Position(0,0,clipShiftRear),Rotation(0,+clipAngle,0));
	vol  = Volume(name+"_"+nam+"_wall_neg",solid,lcdd.vacuum());
	vol.setVisAttributes(vis);
        if (start.inner_r != start.outer_r || end.inner_r != end.outer_r) {
	  solid = SubtractionSolid(wallCone,clip,Position(0,0,clipShiftFrnt),Rotation(0,+clipAngle,0));
	  solid = SubtractionSolid(solid,   clip,Position(0,0,clipShiftRear),Rotation(0,+clipAngle,0));
	  Volume  wall (name+"_"+nam+"_neg_wall",solid,mat);
	  wall.setVisAttributes(vis);
	  vol.placeVolume(wall);
	}
	assembly.placeVolume(vol,Position(0,0,zPos).rotateY(reflect_angle),Rotation(M_PI,reflect_angle,0));
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
  sdet.setPlacement(lcdd.pickMotherVolume(sdet).placeVolume(assembly));
  return sdet;
}

DECLARE_DETELEMENT(Tesla_tubeX01,create_element);
