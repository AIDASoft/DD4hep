//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#include "DDDetectors/OtherDetectorHelpers.h"

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepUnits.h"

#include <cmath>
#include <string>

using dd4hep::Transform3D;
using dd4hep::Position;
using dd4hep::RotationX;
using dd4hep::RotationY;
using dd4hep::RotateY;
using dd4hep::RotateX;
using dd4hep::ConeSegment;
using dd4hep::SubtractionSolid;
using dd4hep::Material;
using dd4hep::Volume;
using dd4hep::Solid;
using dd4hep::Tube;
using dd4hep::PlacedVolume;
using dd4hep::Assembly;
namespace units = dd4hep;

static dd4hep::Ref_t create_element(dd4hep::Detector& description,
					      xml_h xmlHandle,
					      dd4hep::SensitiveDetector /*sens*/) {

  printout(dd4hep::DEBUG,"DD4hep_Mask", "Creating Mask" ) ;

  //Access to the XML File
  xml_det_t xmlMask = xmlHandle;
  const std::string name = xmlMask.nameStr();

  //--------------------------------
  Assembly envelope( name + "_assembly"  ) ;
  //--------------------------------

  dd4hep::DetElement tube(  name, xmlMask.id()  ) ;

  bool rotationX= false;

  //Parameters we have to know about
  dd4hep::xml::Component xmlParameter = xmlMask.child(_Unicode(parameter));
  const double crossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle))*0.5; //  only half the angle

  if (xmlParameter.hasAttr(_Unicode(rotationX)))
    rotationX = xmlParameter.attr< bool >(_Unicode(rotationX));

  for(xml_coll_t c( xmlMask ,Unicode("section")); c; ++c) {

    xml_comp_t xmlSection( c );

    ODH::ECrossType crossType = ODH::getCrossType(xmlSection.attr< std::string >(_Unicode(type)));
    const double zStart       = xmlSection.attr< double > (_Unicode(start));
    const double zEnd         = xmlSection.attr< double > (_Unicode(end));
    const double rInnerStart  = xmlSection.attr< double > (_Unicode(rMin1));
    const double rInnerEnd    = xmlSection.attr< double > (_Unicode(rMin2));
    const double rOuterStart  = xmlSection.attr< double > (_Unicode(rMax1));
    const double rOuterEnd    = xmlSection.attr< double > (_Unicode(rMax2));
    const double thickness    = rOuterStart - rInnerStart;
    Material sectionMat  = description.material(xmlSection.materialStr());
    const std::string volName      = "tube_" + xmlSection.nameStr();

    double phi1 = 0 ;
    double phi2 = 360.0*units::degree;
    if (xmlSection.hasAttr(_U(phi1)))
      phi1 = xmlSection.attr< double > (_U(phi1));
    if (xmlSection.hasAttr(_U(phi2)))
      phi2 = xmlSection.attr< double > (_U(phi2));

    std::stringstream pipeInfo;
    pipeInfo << std::setw(8) << zStart      /units::mm
	     << std::setw(8) << zEnd        /units::mm
	     << std::setw(8) << rInnerStart /units::mm
	     << std::setw(8) << rInnerEnd   /units::mm
	     << std::setw(8) << rOuterStart /units::mm
	     << std::setw(8) << rOuterEnd   /units::mm
	     << std::setw(8) << thickness   /units::mm
	     << std::setw(8) << crossType
	     << std::setw(35) << volName
	     << std::setw(15) << sectionMat.name()
	     << std::setw(8) << phi1
	     << std::setw(8) << phi2;

    printout(dd4hep::INFO, "DD4hep_Mask", pipeInfo.str() );

    // things which can be calculated immediately
    const double zHalf       = fabs(zEnd - zStart) * 0.5; // half z length of the cone
    const double zPosition   = fabs(zEnd + zStart) * 0.5; // middle z position
    Material material    = sectionMat;

    // this could mess up your geometry, so better check it
    if (not ODH::checkForSensibleGeometry(crossingAngle, crossType)){
      throw std::runtime_error( " Mask_o1_v01_geo.cpp : checkForSensibleGeometry() failed " ) ;
    }

    const double rotateAngle = getCurrentAngle(crossingAngle, crossType); // for the placement at +z (better make it const now)
    const double mirrorAngle = M_PI - rotateAngle; // for the "mirrored" placement at -z
    // the "mirroring" in fact is done by a rotation of (almost) 180 degrees around the y-axis

    switch (crossType) {
    case ODH::kCenter:
    case ODH::kUpstream:
    case ODH::kDnstream: {
      // a volume on the z-axis, on the upstream branch, or on the downstream branch
      Transform3D transformer, transmirror;
      if( rotationX == true) {
        // absolute transformations for the placement in the world, rotate over X
        transformer = Transform3D(RotationX(rotateAngle), RotateX( Position(0, 0, zPosition), rotateAngle) );
        transmirror = Transform3D(RotationX(mirrorAngle), RotateX( Position(0, 0, zPosition), mirrorAngle) );
      } else{
	// absolute transformations for the placement in the world
	transformer = Transform3D(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition), rotateAngle) );
	transmirror = Transform3D(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition), mirrorAngle) );
      }
      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid( zHalf, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd , phi1, phi2);

      // tube consists of vacuum
      Volume tubeLog( volName, tubeSolid, material ) ;
      tubeLog.setVisAttributes(description, xmlMask.visStr() );

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog,  transformer );
      envelope.placeVolume( tubeLog,  transmirror );

    }
      break;

    case ODH::kPunchedCenter: {
      // a cone with one or two inner holes (two tubes are punched out)

      const double rUpstreamPunch = rInnerStart; // just alias names denoting what is meant here
      const double rDnstreamPunch = rInnerEnd; // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D upstreamTransformer(RotationY(-crossingAngle), Position(zPosition * tan(-crossingAngle), 0, 0));
      Transform3D dnstreamTransformer(RotationY(+crossingAngle), Position(zPosition * tan(+crossingAngle), 0, 0));

      // absolute transformations for the final placement in the world (angles always equal zero and 180 deg)
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // the main solid and the two pieces (only tubes, for the moment) which will be punched out
      ConeSegment wholeSolid(  zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2 );
      Solid tmpSolid0, tmpSolid1, finalSolid0, finalSolid1;

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x

      if ( rUpstreamPunch > 1e-6 ) { // do we need a hole on the upstream branch?
	Tube upstreamPunch( 0, rUpstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	tmpSolid0 = SubtractionSolid( wholeSolid, upstreamPunch, upstreamTransformer);
	tmpSolid1 = SubtractionSolid( wholeSolid, upstreamPunch, dnstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	tmpSolid0 = wholeSolid;
	tmpSolid1 = wholeSolid;
      }

      if (rDnstreamPunch > 1e-6 ) { // do we need a hole on the downstream branch?
	Tube dnstreamPunch( 0, rDnstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	finalSolid0 = SubtractionSolid( tmpSolid0, dnstreamPunch, dnstreamTransformer);
	finalSolid1 = SubtractionSolid( tmpSolid1, dnstreamPunch, upstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	finalSolid0 = tmpSolid0;
	finalSolid1 = tmpSolid1;
      }

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", finalSolid0, material );
      Volume tubeLog1( volName + "_1", finalSolid1, material );
      tubeLog0.setVisAttributes(description, xmlMask.visStr() );
      tubeLog1.setVisAttributes(description, xmlMask.visStr() );

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog0, placementTransformer );
      envelope.placeVolume( tubeLog1, placementTransmirror );

      break;
    }

    case ODH::kPunchedUpstream:
    case ODH::kPunchedDnstream: {
      // a volume on the upstream or downstream branch with two inner holes
      // (implemented as a cone from which another tube is punched out)

      const double rCenterPunch = (crossType == ODH::kPunchedUpstream) ? (rInnerStart) : (rInnerEnd); // just alias names denoting what is meant here
      const double rOffsetPunch = (crossType == ODH::kPunchedDnstream) ? (rInnerStart) : (rInnerEnd); // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D punchTransformer(RotationY(-2 * rotateAngle), Position(zPosition * tan(-2 * rotateAngle), 0, 0));
      Transform3D punchTransmirror(RotationY(+2 * rotateAngle), Position(zPosition * tan(+2 * rotateAngle), 0, 0));

      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // the main solid and the piece (only a tube, for the moment) which will be punched out
      ConeSegment wholeSolid( zHalf, rCenterPunch , rOuterStart, rCenterPunch, rOuterEnd, phi1, phi2);
      Tube punchSolid( 0, rOffsetPunch, 5 * zHalf, phi1, phi2); // a bit longer

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      SubtractionSolid finalSolid0( wholeSolid, punchSolid, punchTransformer);
      SubtractionSolid finalSolid1( wholeSolid, punchSolid, punchTransmirror);

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", finalSolid0, material );
      Volume tubeLog1( volName + "_1", finalSolid1, material );
      tubeLog0.setVisAttributes(description, xmlMask.visStr() );
      tubeLog1.setVisAttributes(description, xmlMask.visStr() );

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog0, placementTransformer );
      envelope.placeVolume( tubeLog1, placementTransmirror );

      break;
    }
    default: {
      throw std::runtime_error( " Mask_o1_v01_geo.cpp : fatal failure !! ??  " ) ;
    }

    }//end switch
  }//for all xmlSections

  //--------------------------------------
  Volume mother =  description.pickMotherVolume( tube ) ;
  PlacedVolume pv(mother.placeVolume(envelope));
  pv.addPhysVolID( "system", xmlMask.id() ) ; //.addPhysVolID("side", 0 ) ;

  tube.setVisAttributes( description, xmlMask.visStr(), envelope );

  tube.setPlacement(pv);

  return tube;
}
DECLARE_DETELEMENT(DD4hep_Mask_o1_v01,create_element)
