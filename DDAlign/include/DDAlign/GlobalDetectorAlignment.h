//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_GEOMETRY_GLOBALDETECTORALIGNMENT_H
#define DD4HEP_GEOMETRY_GLOBALDETECTORALIGNMENT_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/GlobalAlignment.h"

// Forward declarations
class TGeoHMatrix;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {

    /// GlobalDetectorAlignment. DetElement Handle supporting alignment operations.
    /**
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_ALIGN
     */
    class GlobalDetectorAlignment : public DetElement {
    protected:
    public:
      /// Initializing constructor
      GlobalDetectorAlignment(DetElement e);
      /// Access debugging flag
      static bool debug();
      /// Set debugging flag
      static bool debug(bool value);
      /// Collect all placements from the detector element up to the world volume
      void collectNodes(std::vector<PlacedVolume>& nodes);
      /// Access to the alignment block
      GlobalAlignment alignment() const;
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      std::vector<GlobalAlignment>& volumeAlignments();
      /// Alignment entries for lower level volumes, which are NOT attached to daughter DetElements
      const std::vector<GlobalAlignment>& volumeAlignments() const;

      /** @DetElement alignment: Calls to align the detector element itself  */
      /// Align the PhysicalNode of the placement of the detector element (translation only)
      GlobalAlignment align(const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (rotation only)
      GlobalAlignment align(const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (translation + rotation)
      GlobalAlignment align(const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic Transform3D
      GlobalAlignment align(const Transform3D& tr, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic TGeo matrix
      GlobalAlignment align(TGeoHMatrix* matrix, bool check = false, double overlap = 0.001);

      /** @Volume alignment: Calls to align the volumes within on detector element  */
      /// Align the PhysicalNode of the placement of the detector element (translation only)
      GlobalAlignment align(const std::string& volume_path, const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (rotation only)
      GlobalAlignment align(const std::string& volume_path, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (translation + rotation)
      GlobalAlignment align(const std::string& volume_path, const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic Transform3D
      GlobalAlignment align(const std::string& volume_path, const Transform3D& tr, bool check = false, double overlap = 0.001);
      /// Align the physical node according to a generic TGeo matrix
      GlobalAlignment align(const std::string& volume_path, TGeoHMatrix* matrix, bool check = false, double overlap = 0.001);
    };

  } /* End namespace Alignments                           */
} /* End namespace DD4hep                                 */
#endif    /* DD4HEP_GEOMETRY_GLOBALDETECTORALIGNMENT_H    */
