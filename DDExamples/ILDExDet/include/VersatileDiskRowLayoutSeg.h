// $Id:$
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
    

    struct VersatileDiskRowLayoutSeg : public Segmentation  {
      // public:
      /// Constructor to be used when reading the already parsed object
      template <typename Q> VersatileDiskRowLayoutSeg(const Handle<Q>& e) : Segmentation(e) {}
      /// Constructor to create a new segmentation object
      VersatileDiskRowLayoutSeg();
      void addRow(int nPads, double padPitch, double rowHeight, double offset);
      int setRMin(int rmin);

      //protected:
      struct Row{
	int _rowNumber;
	int _nPads ;
	double _padPitch;
	double _rowHeight;
	double _offset;
      };
      
      std::vector<Row> _rows ;
      int _rMin;
      int _nPads;
      
    };
  }//Geometry
}//DD4hep




#endif
