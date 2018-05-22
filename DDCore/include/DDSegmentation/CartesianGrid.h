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
 * CartesianGrid.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRID_H_
#define DDSegmentation_CARTESIANGRID_H_

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian grid segmentation
    class CartesianGrid: public Segmentation {
    public:
      /// Destructor
      virtual ~CartesianGrid();
    protected:
      /// Default constructor used by derived classes passing the encoding string
      CartesianGrid(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      CartesianGrid(const BitFieldCoder* decoder);
    };

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_CARTESIANGRID_H_ */
