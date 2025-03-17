//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Created: Jun 28, 2013
//  Author:  Christian Grefe, CERN
//           David Blyth, ANL
//
//==========================================================================

/// Framework include files
#include <DDSegmentation/CartesianStrip.h>

namespace dd4hep {
namespace DDSegmentation {
/// Default constructor used by derived classes passing the encoding string
CartesianStrip::CartesianStrip(const std::string& cellEncoding) : Segmentation(cellEncoding) {}

/// Default constructor used by derived classes passing an existing decoder
CartesianStrip::CartesianStrip(const BitFieldCoder* decode) : Segmentation(decode) {}

/// Destructor
CartesianStrip::~CartesianStrip() {}
} /* namespace DDSegmentation */
} /* namespace dd4hep */
