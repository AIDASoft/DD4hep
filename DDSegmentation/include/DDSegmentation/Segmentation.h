/*
 * Segmentation.h
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef SEGMENTATION_H_
#define SEGMENTATION_H_

#include "DD4hep/BitField64.h"

#include <string>
#include <vector>

namespace DD4hep {
namespace DDSegmentation {

class Segmentation {
public:
	Segmentation(const std::string& cellEncoding);
	Segmentation(const BitField64& decoder);
	virtual ~Segmentation();

	virtual std::vector<double> getLocalPosition(const long64& cellID) const = 0;

	virtual long64 getCellID(double x, double y, double z) const = 0;
	long64 getCellID(const std::vector<double>& localPosition) const;
	long64 getCellID(const double* localPosition) const;

	std::string fieldDescription() const {
		return _decoder.fieldDescription();
	}

protected:
	mutable BitField64 _decoder;
};

} /* namespace Segmentation */
} /* namespace DD4hep */
#endif /* SEGMENTATION_H_ */
