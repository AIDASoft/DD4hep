// $Id$
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
#ifndef DD4HEP_ALIGNMENT_GLOBALALIGNMENT_H
#define DD4HEP_ALIGNMENT_GLOBALALIGNMENT_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Alignments.h"
#include "TGeoPhysicalNode.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {

    /// Main handle class to hold a TGeo alignment object of type TGeoPhysicalNode
    /**
     *  See the ROOT documentation about the TGeoPhysicalNode for further details:
     *  @see http://root.cern.ch/root/html/TGeoPhysicalNode.html
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class GlobalAlignment : public Handle<TGeoPhysicalNode> {

      typedef Geometry::RotationZYX RotationZYX;
      typedef Geometry::Transform3D Transform3D;
      typedef Geometry::Position    Position;
      
    public:
      /// Default constructor
      GlobalAlignment();
      /// Default constructor
      GlobalAlignment(TGeoPhysicalNode* p)
        : Handle<TGeoPhysicalNode>(p)  {
      }
      /// Copy constructor
      GlobalAlignment(const GlobalAlignment& c);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> GlobalAlignment(const Handle<Q>& e)
        : Handle<TGeoPhysicalNode>(e) {
      }
      /// Initializing constructor
      GlobalAlignment(const std::string& path);
      /// Assignment operator
      GlobalAlignment& operator=(const GlobalAlignment& c);
      /// Number of nodes in this branch (=depth of the placement hierarchy from the top level volume)
      int numNodes() const;
      /// Access the placement of this node
      PlacedVolume placement()   const;
      /// Access the placement of the mother of this node
      PlacedVolume motherPlacement(int level_up = 1)   const;
      /// Access the placement of a node in the chain of placements for this branch
      /** Remeber the special cases:
       *  nodePlacement(-1) == nodePlacement(numNodes()) = placement()
       *  nodePlacement(numNodes()-1)  == motherPlacement() = TGeoPhysicalNode::GetMother()
       *                    == direct mother of placement()
       */
      PlacedVolume nodePlacement(int level=-1)   const;
      /// Access the currently applied alignment/placement matrix with respect to the world
      Transform3D toGlobal(int level=-1) const;
      /// Transform a point from local coordinates of a given level to global coordinates
      Position toGlobal(const Position& localPoint, int level=-1) const;
      /// Transform a point from global coordinates to local coordinates of a given level
      Position globalToLocal(const Position& globalPoint, int level=-1) const;

      /// Access the currently applied alignment/placement matrix with respect to mother volume
      Transform3D toMother(int level=-1) const;

      /// Access the currently applied alignment/placement matrix (mother to daughter)
      Transform3D nominal() const;
      /// Access the currently applied correction matrix (delta) (mother to daughter)
      Transform3D delta() const;
      /// Access the inverse of the currently applied correction matrix (delta) (mother to daughter)
      Transform3D invDelta() const;

    };

  } /* End namespace Alignments               */
} /* End namespace DD4hep                     */
#endif    /* DD4HEP_ALIGNMENT_GLOBALALIGNMENT_H      */
