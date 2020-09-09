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
 * CartesianStrip.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#ifndef DDSEGMENTATION_CARTESIANSTRIP_H
#define DDSEGMENTATION_CARTESIANSTRIP_H

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    /// Segmentation base class describing cartesian strip segmentation
    class CartesianStrip : public Segmentation {
    public:
      /// Destructor
      virtual ~CartesianStrip();

    protected:
      /// Default constructor used by derived classes passing the encoding string
      CartesianStrip(const std::string& cellEncoding = "");
      /// Default constructor used by derived classes passing an existing decoder
      CartesianStrip(const BitFieldCoder* decoder);
    };
  } /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif // DDSEGMENTATION_CARTESIANSTRIP_H
