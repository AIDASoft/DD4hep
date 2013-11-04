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
	/// destructor
	virtual ~Segmentation();

	/// determine the position based on the cell ID
	virtual std::vector<double> getPosition(const long64& cellID) const = 0;
	/// determine the cell ID based on the position
	virtual long64 getCellID(double x, double y, double z) const = 0;
	/// determine the cell ID based on the position
	long64 getCellID(const std::vector<double>& position) const;
	/// determine the cell ID based on the position
	long64 getCellID(const double* position) const;
	/// check if it is a local segmentation
	bool isLocal() const {
		return _isLocal;
	}
	/// set if the segmentation is using local coordinates
	void setLocal(bool isLocal) {
		_isLocal = isLocal;
	}
	/// access the encoding string
	std::string fieldDescription() const {
		return _decoder->fieldDescription();
	}
	/// access the segmentation name
	std::string name() const {
		return _name;
	}
	/// Set the segmentation name
	void setName(const std::string& value) {
		_name = value;
	}
	/// access the segmentation type
	std::string type() const {
		return _type;
	}
	/// Set the segmentation type
	void setType(const std::string& value)  {
		_type = value;
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
	/// the segmentation name
	std::string _name;
	/// the segmentation type
	std::string _type;
	/// distinguish between local and global coordinate systems
	bool _isLocal;

	/// helper method to convert a bin number to a 1D position
	double binToPosition(long64 bin, double cellSize, double offset = 0.) const;
	/// helper method to convert a 1D position to a cell ID
	int positionToBin(double position, double cellSize, double offset = 0.) const;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATION_H_ */
