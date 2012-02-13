// $Id:$
//====================================================================
//  AIDA Detector description for Sid Compact notation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_DETECTORS_COMPACTDETECTORS_H
#define DD4hep_DETECTORS_COMPACTDETECTORS_H

#include "DD4hep/Detector.h"

namespace DD4hep {
  namespace Geometry {
    struct SensitiveDetector;
  }
  using namespace Geometry;
  struct PolyconeSupport              : public DetElement {};
  struct TubeSegment                  : public DetElement {};
  struct MultiLayerTracker            : public DetElement {};
  struct DiskTracker                  : public DetElement {};
  struct SiVertexBarrel               : public DetElement {};
  struct SiVertexEndcap2              : public DetElement {};
  struct SiTrackerBarrel              : public DetElement {};
  struct SiTrackerEndcap              : public DetElement {};
  struct SiTrackerEndcap2             : public DetElement {};
  struct CylindricalBarrelCalorimeter : public DetElement {};
  struct CylindricalEndcapCalorimeter : public DetElement {};
  struct EcalBarrel                   : public DetElement {};
  struct PolyhedraEndcapCalorimeter2  : public DetElement {};
  struct PolyhedraBarrelCalorimeter2  : public DetElement {};
  struct ForwardDetector              : public DetElement {};

}
#endif // DD4hep_DETECTORS_COMPACTDETECTORS_H
