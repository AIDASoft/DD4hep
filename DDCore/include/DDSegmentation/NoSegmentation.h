/*
 * NoSegmentation.h
 *
 *  Created on: Feb. 13 2017
 *      Author: Whitney Armstrong, ANL
 */

#ifndef DDSegmentation_NoSegmentation_H_
#define DDSegmentation_NoSegmentation_H_

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    /** This class exists to provide a segmenation when it is need but doesn't exist. */
    class NoSegmentation: public Segmentation {
      public:
        
        virtual ~NoSegmentation();

        NoSegmentation(const std::string& cellEncoding = "");
        NoSegmentation(const BitFieldCoder* decoder);
	
        virtual Vector3D position(const CellID& cellID) const;
        virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;

    };


  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_NoSegmentation */
