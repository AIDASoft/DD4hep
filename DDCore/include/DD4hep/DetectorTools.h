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

    // Forward declarations
    class LCDD;

    /** @class Detectortools  DetectorTools.h DD4hep/Detectortools.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class DetectorTools {
    public:
      typedef std::vector<DetElement>   ElementPath;
      typedef std::vector<PlacedVolume> PlacementPath;
      /// Assemble the path of a particular detector element
      std::string elementPath(DetElement element);
      /// Assemble the path of the PlacedVolume selection
      static std::string elementPath(const ElementPath& nodes);
      /// Collect detector elements to the top detector element (world)
      static void elementPath(DetElement elt, ElementPath& detectors);
      /// Collect detector elements to any parent detector element
      static void elementPath(DetElement parent, DetElement elt, ElementPath& detectors);
      /// Collect detector elements placements to the top detector element (world) [fast, but may have holes!]
      static void elementPath(DetElement elt, PlacementPath& nodes);

      /// Find DetElement as child of the top level volume by it's absolute path
      static DetElement findElement(LCDD& lcdd, const std::string& path);
      /// Find DetElement as child of a parent by it's relative or absolute path
      static DetElement findElement(DetElement parent, const std::string& subpath);
      /// Determine top level element (=world) for any element walking up the detector element tree
      static DetElement topElement(DetElement child);
      
      /// Assemble the placement path from a given detector element to the world volume
      static std::string placementPath(DetElement element);
      /// Assemble the path of the PlacedVolume selection
      static std::string placementPath(const PlacementPath& nodes);
      /// Collect detector elements placements to the top detector element (world) [no holes!]
      static void placementPath(DetElement elt, PlacementPath& nodes);
      /// Collect detector elements placements to the parent detector element [no holes!]
      static void placementPath(DetElement parent, DetElement child, PlacementPath& nodes);


      /// Find Child of PlacedVolume and assemble on the fly the path of PlacedVolumes
      static bool findChild(PlacedVolume parent, PlacedVolume child, PlacementPath& path);
      /// Find path between the child element and the parent element
      static bool findParent(DetElement parent, DetElement child, ElementPath& detectors);

      /// Create cached matrix to transform to positions to an upper level Placement
      static TGeoMatrix* placementTrafo(const PlacementPath& nodes, bool inverse);

    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_DETECTORTOOLS_H    */
