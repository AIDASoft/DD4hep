/*
 * CartesianStrip.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#include "DDSegmentation/CartesianStrip.h"

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
