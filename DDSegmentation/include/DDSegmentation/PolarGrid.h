/*
 * PolarGrid.h
 *
 *  Created on: Sept 16, 2014
 *      Author: Marko Petric
 */

#ifndef DDSegmentation_POLARGRID_H_
#define DDSegmentation_POLARGRID_H_

#include "DDSegmentation/Segmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class PolarGrid: public Segmentation {
public:
	/// Destructor
	virtual ~PolarGrid();
protected:
	/// Default constructor used by derived classes passing the encoding string
	PolarGrid(const std::string& cellEncoding = "");
	/// Default constructor used by derived classes passing an existing decoder
	PolarGrid(BitField64* decoder);
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_POLARGRID_H_ */
