/*
 * Segmentation.h
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATION_H_
#define DDSegmentation_SEGMENTATION_H_

#include "DDSegmentation/BitField64.h"

#include <utility>
#include <string>
#include <vector>

namespace DD4hep {
namespace DDSegmentation {

/// Segmentation parameter definition
typedef std::pair<std::string,double> Parameter;
/// Segmentation parameter container definition
typedef std::vector<Parameter>        Parameters;

class Segmentation {
public:
	/// default constructor using an arbitrary type
	template <typename TYPE> Segmentation(TYPE cellEncoding);
	/// default constructor using an existing decoder
	Segmentation(BitField64* decoder);
	/// destructor
	virtual ~Segmentation();

	/// determine the local position based on the cell ID
	virtual std::vector<double> getLocalPosition(const long64& cellID) const = 0;
	/// determine the cell ID based on the local position
	virtual long64 getCellID(double x, double y, double z) const = 0;
	/// determine the cell ID based on the local position
	long64 getCellID(const std::vector<double>& localPosition) const;
	/// determine the cell ID based on the local position
	long64 getCellID(const double* localPosition) const;
	/// access the encoding string
	std::string fieldDescription() const {
		return _decoder->fieldDescription();
	}
	/// access the segmentation type
	std::string type() const {
		return _type;
	}
	/// access the underlying decoder
	BitField64* decoder() {
		return _decoder;
	}
	/// set the underlying decoder
	void setDecoder(BitField64* decoder);
	/// access the set of parameters for this segmentation
	Parameters parameters() const;

protected:
	/// the cell ID encoder and decoder
	mutable BitField64* _decoder;
	/// keeps track of the decoder ownership
	bool _ownsDecoder;
	/// the segmentation type
	std::string _type;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATION_H_ */
