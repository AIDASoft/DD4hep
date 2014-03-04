// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_DETECTORTOOLS_H
#define DD4HEP_GEOMETRY_DETECTORTOOLS_H

// Framework include files
#include "DD4hep/Detector.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    /** @class Detectortools  DetectorTools.h DD4hep/Detectortools.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class DetectorTools {
    public:
      typedef std::vector<DetElement>   ElementPath;
      typedef std::vector<PlacedVolume> PlacementPath;
      /// Collect detector elements to the top detector element (world)
      static void elementPath(DetElement elt, ElementPath& detectors);
      /// Collect detector elements to any parent detector element
      static void elementPath(DetElement parent, DetElement elt, ElementPath& detectors);
      /// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
      static void elementPath(DetElement elt, PlacementPath& nodes);

      /// Collect detector elements placements to the top detector element (world) [no holes!]
      static void placementPath(DetElement elt, PlacementPath& nodes);
      /// Collect detector elements placements to the parent detector element [no holes!]
      static void placementPath(DetElement parent, DetElement child, PlacementPath& nodes);
      /// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
      static bool findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path);
      /// Create cached matrix to transform to positions to an upper level Placement
      static TGeoMatrix* placementTrafo(const PlacementPath& nodes, bool inverse);

      /// Find path between the child element and the parent element
      static bool findParent(DetElement parent, DetElement child, ElementPath& detectors);
      /// Assemble the path of the PlacedVolume selection
      static std::string elementPath(const ElementPath& nodes);
      /// Assemble the path of the PlacedVolume selection
      static std::string nodePath(const PlacementPath& nodes);

    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_DETECTORTOOLS_H    */
