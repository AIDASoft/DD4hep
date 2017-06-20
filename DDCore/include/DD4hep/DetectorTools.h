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
#ifndef DD4HEP_DDCORE_DETECTORTOOLS_H
#define DD4HEP_DDCORE_DETECTORTOOLS_H

// Framework include files
#include "DD4hep/DetElement.h"

// Forward declarations
class TGeoHMatrix;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;

  namespace detail   {
    
    /// Helper namespace used to answer detector element specific questons
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CORE
     */
    namespace tools {
      typedef std::vector<DetElement>   ElementPath;
      typedef std::vector<PlacedVolume> PlacementPath;

      /// Determine top level element (=world) for any element walking up the detector element tree
      DetElement topElement(DetElement child);

      /// Assemble the path of a particular detector element
      std::string elementPath(DetElement element);
      /// Assemble the path of the PlacedVolume selection
      std::string elementPath(const ElementPath& nodes, bool reverse=true);

      /// Collect detector elements to the top detector element (world)
      void elementPath(DetElement elt, ElementPath& detectors);
      /// Find DetElement as child of the top level volume by it's absolute path
      DetElement findElement(Detector& description, const std::string& path);
      /// Find DetElement as child of a parent by it's relative or absolute path
      DetElement findDaughterElement(DetElement parent, const std::string& subpath);
      /// Find path between the child element and the parent element
      bool isParentElement(DetElement parent, DetElement child);

      /// Assemble the placement path from a given detector element to the world volume
      std::string placementPath(DetElement element);
      /// Assemble the path of the PlacedVolume selection
      std::string placementPath(const PlacementPath& nodes, bool reverse=true);
      /// Assemble the path of the PlacedVolume selection
      std::string placementPath(const std::vector<const TGeoNode*>& nodes, bool reverse=true);

      /// Collect detector elements placements to the top detector element (world) [no holes!]
      void placementPath(DetElement elt, PlacementPath& nodes);
      /// Collect detector elements placements to the parent detector element [no holes!]
      void placementPath(DetElement parent, DetElement child, PlacementPath& nodes);

      /// Find a given node in the hierarchy starting from the top node (absolute placement!)
      PlacedVolume findNode(PlacedVolume top_place, const std::string& place);
      /// Update cached matrix to transform to positions to an upper level Placement
      void placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix*& mat);
      /// Update cached matrix to transform to positions to an upper level Placement
      void placementTrafo(const PlacementPath& nodes, bool inverse, TGeoHMatrix& mat);


      /// Convert VolumeID to string
      std::string toString(const PlacedVolume::VolIDs& ids);
      /// Convert VolumeID to string
      std::string toString(const IDDescriptor& dsc, const PlacedVolume::VolIDs& ids, VolumeID code);
    }
  }
}         /* End namespace dd4hep                   */
#endif    /* DD4HEP_DDCORE_DETECTORTOOLS_H    */
