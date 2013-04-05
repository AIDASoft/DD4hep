// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A. Muennich
//
//====================================================================
#ifndef DD4HEP_VERSATILEDISKROWLAYOUTSEG_H
#define DD4HEP_VERSATILEDISKROWLAYOUTSEG_H

#include <vector>
#include "DD4hep/Segmentations.h"

namespace DD4hep {
  
  namespace Geometry  {
    
    struct Row{
      int _rowNumber;
      int _nPads ;
      double _padPitch;
      double _rowHeight;
      double _offset;
      double _rCentre;
    };
    
    struct VersatileDiskRowLayoutData{
      VersatileDiskRowLayoutData();
      void addRow(int nPads, double padPitch, double rowHeight, double offset);
      void setRMin(int rmin);
      std::vector<Row> _rows ;
      double _rMin;
      int _nPads;
    };
    
    struct VersatileDiskRowLayoutSeg : public Segmentation  {
      /// Constructor to be used when reading the already parsed object
      template <typename Q> VersatileDiskRowLayoutSeg(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      VersatileDiskRowLayoutSeg();
    };
      
      
      
  }//Geometry
}//DD4hep




#endif
