//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

/*
 * NoSegmentation.h
 *
 *  Created on: Feb. 13 2017
 *      Author: Whitney Armstrong, ANL
 */

#ifndef DDSEGMENTATION_NOSEGMENTATION_H
#define DDSEGMENTATION_NOSEGMENTATION_H

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// This class exists to provide a segmenation when it is need but doesn't exist.
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
#endif // DDSEGMENTATION_NOSEGMENTATION_H
