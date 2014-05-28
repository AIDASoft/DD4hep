// $Id: VXDData.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"

//fixme: VXDData should not depend on this internal header  
#include "DD4hep/objects/DetectorInterna.h"

#include <vector>

namespace DD4hep {

  /**@class VXDData
   *
   *   @author  M.Frank
   *   @version 1.0
   */
  struct VXDData : public Geometry::DetElement::Object {
    /// Helper class for layer properties
    struct Layer {
      double internalPhi0 ;
      double Distance ;
      double Offset ;
      double Thickness ;
      double Length ;
      double Width ;
      double RadLength ;
      int    NLadders ;
    };
    typedef std::vector<Layer> LayerVec ;
    typedef Geometry::Ref_t Ref_t;

    // Ladder
    LayerVec _lVec;
    // Sensitive
    LayerVec _sVec;
  };
}
