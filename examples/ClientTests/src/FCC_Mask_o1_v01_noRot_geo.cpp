//====================================================================
//  LCGeo - LC detector models in DD4hep
//--------------------------------------------------------------------
// Mask based on the Beampipe driver (based on TubeX from Mokka), but not
// filling the centre with Beam vaccum, just places cylinders in arbitrary
// places with different alignments
//  A.Sailer, CERN
//  $Id$
//====================================================================
#include "FCC_OtherDetectorHelpers.h"

#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetFactoryHelper.h"
#include <cmath>
#include <string>

using dd4hep::Assembly;
using dd4hep::ConeSegment;
using dd4hep::DetElement;
using dd4hep::Detector;
using dd4hep::Material;
using dd4hep::PlacedVolume;
using dd4hep::Position;
using dd4hep::Ref_t;
using dd4hep::RotateY;
using dd4hep::RotationY;
using dd4hep::RotateX;
using dd4hep::RotationX;
using dd4hep::SensitiveDetector;
using dd4hep::Solid;
using dd4hep::SubtractionSolid;
using dd4hep::Transform3D;
using dd4hep::Tube;
using dd4hep::Volume;

static Ref_t create_element(Detector& theDetector, xml_h xmlHandle, SensitiveDetector /*sens*/) {

  //------------------------------------------
  //  See comments starting with '//**' for
  //     hints on porting issues
  //------------------------------------------

  std::cout << "This is the Mask:" << std::endl;

  // Access to the XML File
  xml_det_t xmlMask = xmlHandle;
  const std::string name = xmlMask.nameStr();

  //--------------------------------
  Assembly envelope(name + "_assembly");
  //--------------------------------

  DetElement tube(name, xmlMask.id());

  //  const double phi1 = 0 ;
  //  const double phi2 = 360.0*dd4hep::degree;

  // Parameters we have to know about
  dd4hep::xml::Component xmlParameter = xmlMask.child(_Unicode(parameter));
  const double crossingAngle = xmlParameter.attr<double>(_Unicode(crossingangle)) * 0.5;  //  only half the angle

  for (xml_coll_t c(xmlMask, Unicode("section")); c; ++c) {

    xml_comp_t xmlSection(c);

    ODH::ECrossType crossType = ODH::getCrossType(xmlSection.attr<std::string>(_Unicode(type)));
    const double zStart = xmlSection.attr<double>(_Unicode(start));
    const double zEnd = xmlSection.attr<double>(_Unicode(end));
    const double rInnerStart = xmlSection.attr<double>(_Unicode(rMin1));
    const double rInnerEnd = xmlSection.attr<double>(_Unicode(rMin2));
    const double rOuterStart = xmlSection.attr<double>(_Unicode(rMax1));
    const double rOuterEnd = xmlSection.attr<double>(_Unicode(rMax2));
    const double phi1 = xmlSection.attr<double>(_Unicode(Phi1));
    const double phi2 = xmlSection.attr<double>(_Unicode(Phi2));
    const double thickness = rOuterStart - rInnerStart;
    Material sectionMat = theDetector.material(xmlSection.materialStr());
    const std::string volName = "tube_" + xmlSection.nameStr();

    std::cout << std::setw(8) << zStart << std::setw(8) << zEnd << std::setw(8) << rInnerStart << std::setw(8)
              << rInnerEnd << std::setw(8) << rOuterStart << std::setw(8) << rOuterEnd << std::setw(8) << thickness
              << std::setw(8) << crossType << std::setw(8) << phi1 << std::setw(8) << phi2 << std::setw(15) << volName
              << std::setw(15) << sectionMat.name() << std::endl;

    // things which can be calculated immediately
    const double zHalf = fabs(zEnd - zStart) * 0.5;      // half z length of the cone
    const double zPosition = fabs(zEnd + zStart) * 0.5;  // middle z position
    Material material = sectionMat;

    // this could mess up your geometry, so better check it
    if (not ODH::checkForSensibleGeometry(crossingAngle, crossType)) {
      throw std::runtime_error(" Mask_o1_v01_noRot_geo.cpp : checkForSensibleGeometry() failed ");
    }

    const double rotateAngle =
        getCurrentAngle(crossingAngle, crossType);  // for the placement at +z (better make it const now)
    const double mirrorAngle = M_PI - rotateAngle;  // for the "mirrored" placement at -z
    // the "mirroring" in fact is done by a rotation of (almost) 180 degrees around the y-axis

    switch (crossType) {
    case ODH::kCenter:
    case ODH::kUpstream:
    case ODH::kDnstream: {
      // a volume on the z-axis, on the upstream branch, or on the downstream branch

      // absolute transformations for the placement in the world, rotate over X
      Transform3D transformer(RotationX(rotateAngle), RotateX(Position(0, 0, zPosition), rotateAngle));
      Transform3D transmirror(RotationX(mirrorAngle), RotateX(Position(0, 0, zPosition), mirrorAngle));

      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid(zHalf, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, phi1, phi2);

      // tube consists of vacuum
      Volume tubeLog(volName, tubeSolid, material);
      tubeLog.setVisAttributes(theDetector, xmlMask.visStr());

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume(tubeLog, transformer);
      envelope.placeVolume(tubeLog, transmirror);

    } break;

    case ODH::kPunchedCenter: {
      // a cone with one or two inner holes (two tubes are punched out)

      const double rUpstreamPunch = rInnerStart;  // just alias names denoting what is meant here
      const double rDnstreamPunch = rInnerEnd;    // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D upstreamTransformer(RotationY(-crossingAngle), Position(zPosition * tan(-crossingAngle), 0, 0));
      Transform3D dnstreamTransformer(RotationY(+crossingAngle), Position(zPosition * tan(+crossingAngle), 0, 0));

      // absolute transformations for the final placement in the world (angles always equal zero and 180 deg)
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY(Position(0, 0, zPosition), rotateAngle));
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY(Position(0, 0, zPosition), mirrorAngle));

      // the main solid and the two pieces (only tubes, for the moment) which will be punched out
      ConeSegment wholeSolid(zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2);
      Solid tmpSolid0, tmpSolid1, finalSolid0, finalSolid1;

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x

      if (rUpstreamPunch > 1e-6) {                                     // do we need a hole on the upstream branch?
        Tube upstreamPunch(0, rUpstreamPunch, 5 * zHalf, phi1, phi2);  // a bit longer
        tmpSolid0 = SubtractionSolid(wholeSolid, upstreamPunch, upstreamTransformer);
        tmpSolid1 = SubtractionSolid(wholeSolid, upstreamPunch, dnstreamTransformer);  // [sic]
      } else {  // dont't do anything, just pass on the unmodified shape
        tmpSolid0 = wholeSolid;
        tmpSolid1 = wholeSolid;
      }

      if (rDnstreamPunch > 1e-6) {                                     // do we need a hole on the downstream branch?
        Tube dnstreamPunch(0, rDnstreamPunch, 5 * zHalf, phi1, phi2);  // a bit longer
        finalSolid0 = SubtractionSolid(tmpSolid0, dnstreamPunch, dnstreamTransformer);
        finalSolid1 = SubtractionSolid(tmpSolid1, dnstreamPunch, upstreamTransformer);  // [sic]
      } else {  // dont't do anything, just pass on the unmodified shape
        finalSolid0 = tmpSolid0;
        finalSolid1 = tmpSolid1;
      }

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0(volName + "_0", finalSolid0, material);
      Volume tubeLog1(volName + "_1", finalSolid1, material);
      tubeLog0.setVisAttributes(theDetector, xmlMask.visStr());
      tubeLog1.setVisAttributes(theDetector, xmlMask.visStr());

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume(tubeLog0, placementTransformer);
      envelope.placeVolume(tubeLog1, placementTransmirror);

      break;
    }

    case ODH::kPunchedUpstream:
    case ODH::kPunchedDnstream: {
      // a volume on the upstream or downstream branch with two inner holes
      // (implemented as a cone from which another tube is punched out)

      const double rCenterPunch = (crossType == ODH::kPunchedUpstream)
          ? (rInnerStart)
          : (rInnerEnd);  // just alias names denoting what is meant here
      const double rOffsetPunch = (crossType == ODH::kPunchedDnstream)
          ? (rInnerStart)
          : (rInnerEnd);  // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D punchTransformer(RotationY(-2 * rotateAngle), Position(zPosition * tan(-2 * rotateAngle), 0, 0));
      Transform3D punchTransmirror(RotationY(+2 * rotateAngle), Position(zPosition * tan(+2 * rotateAngle), 0, 0));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY(Position(0, 0, zPosition), rotateAngle));
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY(Position(0, 0, zPosition), mirrorAngle));

      // the main solid and the piece (only a tube, for the moment) which will be punched out
      ConeSegment wholeSolid(zHalf, rCenterPunch, rOuterStart, rCenterPunch, rOuterEnd, phi1, phi2);
      Tube punchSolid(0, rOffsetPunch, 5 * zHalf, phi1, phi2);  // a bit longer

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      SubtractionSolid finalSolid0(wholeSolid, punchSolid, punchTransformer);
      SubtractionSolid finalSolid1(wholeSolid, punchSolid, punchTransmirror);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0(volName + "_0", finalSolid0, material);
      Volume tubeLog1(volName + "_1", finalSolid1, material);
      tubeLog0.setVisAttributes(theDetector, xmlMask.visStr());
      tubeLog1.setVisAttributes(theDetector, xmlMask.visStr());

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume(tubeLog0, placementTransformer);
      envelope.placeVolume(tubeLog1, placementTransmirror);

      break;
    }
    default: { throw std::runtime_error(" Mask_o1_v01_geo.cpp : fatal failure !! ??  "); }

    }  // end switch
  }    // for all xmlSections

  //--------------------------------------
  Volume mother = theDetector.pickMotherVolume(tube);
  PlacedVolume pv(mother.placeVolume(envelope));
  pv.addPhysVolID("system", xmlMask.id());  //.addPhysVolID("side", 0 ) ;

  tube.setVisAttributes(theDetector, xmlMask.visStr(), envelope);

  tube.setPlacement(pv);

  return tube;
}

DECLARE_DEPRECATED_DETELEMENT(DD4hep_FCC_Mask_o1_v01_noRot, create_element)

