/*
 * CartesianStrip.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 *              David Blyth, ANL
 */

#ifndef DDSegmentation_CARTESIANSTRIP_H_
#define DDSegmentation_CARTESIANSTRIP_H_

#include "DDSegmentation/Segmentation.h"

namespace dd4hep {
namespace DDSegmentation {
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
#endif  // DDSegmentation_CARTESIANSTRIP_H_
