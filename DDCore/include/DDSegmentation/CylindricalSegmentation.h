/*
 * CylindricalSegmentation.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CYLINDRICALSEGMENTATION_H_
#define DDSegmentation_CYLINDRICALSEGMENTATION_H_

#include "DDSegmentation/Segmentation.h"

#include <map>

namespace dd4hep {
namespace DDSegmentation {

class CylindricalSegmentation: public Segmentation {
public:
	/// destructor
	virtual ~CylindricalSegmentation();

protected:
	/// default constructor using an arbitrary type
	CylindricalSegmentation(const std::string& cellEncoding);
	/// Default constructor used by derived classes passing an existing decoder
	CylindricalSegmentation(const BitFieldCoder* decoder);
};


} /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_CYLINDRICALSEGMENTATION_H_ */
