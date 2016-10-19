/*
 * CylindricalSegmentation.cpp
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/CylindricalSegmentation.h"

namespace DD4hep {
  namespace DDSegmentation {

    /// default constructor using an encoding string
    CylindricalSegmentation::CylindricalSegmentation(const std::string& cellEncoding) :
      Segmentation(cellEncoding) {
    }

    /// Default constructor used by derived classes passing an existing decoder
    CylindricalSegmentation::CylindricalSegmentation(BitField64* decode) :
      Segmentation(decode) {
    }

    /// destructor
    CylindricalSegmentation::~CylindricalSegmentation() {

    }

  } /* namespace DDSegmentation */
} /* namespace DD4hep */
