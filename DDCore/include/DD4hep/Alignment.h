// $Id: Readout.h 951 2013-12-16 23:37:56Z Christian.Grefe@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEOMETRY_ALIGNMENT_H
#define DD4HEP_GEOMETRY_ALIGNMENT_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/Volumes.h"
#include "TGeoPhysicalNode.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    class DetElement;

    /** @class Alignment  Alignment.h DD4hep/Alignment.h
     * 
     *  See the documentation about the TGeoPhysicalNode for further
     *  details.
     *
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Alignment: public Handle<TGeoPhysicalNode> {
    public:
      /// Default constructor
      Alignment();
      /// Copy constructor
      Alignment(const Alignment& c);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e)
          : Handle<TGeoPhysicalNode>(e) {
      }
      /// Initializing constructor
      Alignment(const std::string& path);
      /// Assignment operator
      Alignment& operator=(const Alignment& c);
      /// Number of nodes in this branch
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

      /// Align the PhysicalNode (translation only)
      int align(const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode (rotation only)
      int align(const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode (translation + rotation)
      int align(const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_ALIGNMENT_H    */
