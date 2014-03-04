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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry {

    class DetElement;

    /** @class Alignment  Readoutn.h DD4hep/Alignment.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class DetectorAlignment : public DetElement {
    protected:
    public:
      /// Initializing constructor
      DetectorAlignment(DetElement e);
      /// Collect all placements from the detector element up to the world volume
      void collectNodes(std::vector<PlacedVolume>& nodes);
      /// Align the PhysicalNode of the placement of the detector element (translation only)
      int align(const Position& pos, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (rotation only)
      int align(const RotationZYX& rot, bool check = false, double overlap = 0.001);
      /// Align the PhysicalNode of the placement of the detector element (translation + rotation)
      int align(const Position& pos, const RotationZYX& rot, bool check = false, double overlap = 0.001);
    };

  } /* End namespace Geometry                       */
} /* End namespace DD4hep                           */
#endif    /* DD4HEP_GEOMETRY_DETECTORALIGNMENT_H    */
