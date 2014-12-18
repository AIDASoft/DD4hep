// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_DETECTORALIGNMENT_H
#define DD4HEP_GEOMETRY_DETECTORALIGNMENT_H

// Framework include files
#include "DD4hep/Detector.h"

// Forward declarations
class TGeoHMatrix;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class DetElement;

    /// DetectorAlignment. DetElement Handle supporting alignment operations.
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class DetectorAlignment : public DetElement {
    protected:
    public:
      /// Initializing constructor
      DetectorAlignment(DetElement e);
      /// Collect all placements from the detector element up to the world volume
      void collectNodes(std::vector<PlacedVolume>& nodes);
      /// Access to the alignment block
      Alignment alignment() const;
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      std::vector<Alignment>& volumeAlignments();
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      const std::vector<Alignment>& volumeAlignments() const;

      /** @DetElement alignment: Calls to align the detector element itself  */
      /// Align the PhysicalNode of the placement of the detector element (translation only)
      Alignment align(const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (rotation only)
      Alignment align(const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (translation + rotation)
      Alignment align(const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic Transform3D
      Alignment align(const Transform3D& tr, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic TGeo matrix
      Alignment align(TGeoHMatrix* matrix, bool check = false, double overlap = 0.001);

      /** @Volume alignment: Calls to align the volumes within on detector element  */
      /// Align the PhysicalNode of the placement of the detector element (translation only)
      Alignment align(const std::string& volume_path, const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (rotation only)
      Alignment align(const std::string& volume_path, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (translation + rotation)
      Alignment align(const std::string& volume_path, const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic Transform3D
      Alignment align(const std::string& volume_path, const Transform3D& tr, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic TGeo matrix
      Alignment align(const std::string& volume_path, TGeoHMatrix* matrix, bool check = false, double overlap = 0.001);
    };

  } /* End namespace Geometry                       */
} /* End namespace DD4hep                           */
#endif    /* DD4HEP_GEOMETRY_DETECTORALIGNMENT_H    */
