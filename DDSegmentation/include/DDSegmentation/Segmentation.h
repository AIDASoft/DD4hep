/*
 * Segmentation.h
 *
 *  Created on: Jun 27, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATION_H_
#define DDSegmentation_SEGMENTATION_H_

#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/SegmentationFactory.h"
#include "DDSegmentation/SegmentationParameter.h"

#include <map>
#include <utility>
#include <string>
#include <vector>

namespace DD4hep {
namespace DDSegmentation {

typedef SegmentationParameter* Parameter;
typedef std::vector<Parameter> Parameters;

/// Useful typedefs to differentiate cell IDs and volume IDs
typedef long long int CellID;
typedef long long int VolumeID;

/// Simple container for a physics vector
struct Position {
	/// Default constructor
	Position(double x = 0., double y = 0., double z = 0.) :
			X(x), Y(y), Z(z) {
	}
	/// Constructor using a foreign vector class. Requires methods x(), y() and z()
	template <typename T> Position(const T& v) {
		X = v.x();
		Y = v.y();
		Z = v.Z();
	}
	/// Access to x value (required for use with ROOT GenVector)
	double x() const {
		return X;
	}
	/// Access to y value (required for use with ROOT GenVector)
	double y() const {
		return Y;
	}
	/// Access to z value (required for use with ROOT GenVector)
	double z() const {
		return Z;
	}
	double X, Y, Z;
};

/// Base class for all segmentations
class Segmentation {
public:
	/// Destructor
	virtual ~Segmentation();

	/// Determine the local position based on the cell ID
	virtual Position position(const CellID& cellID) const = 0;
	/// Determine the cell ID based on the position
	virtual CellID cellID(const Position& localPosition, const Position& globalPosition,
			const VolumeID& volumeID) const = 0;
	/// Access the encoding string
	virtual std::string fieldDescription() const {
		return _decoder->fieldDescription();
	}
	/// Access the segmentation name
	virtual const std::string& name() const {
		return _name;
	}
	/// Set the segmentation name
	virtual void setName(const std::string& value) {
		_name = value;
	}
	/// Access the segmentation type
	virtual const std::string& type() const {
		return _type;
	}
	/// Access the description of the segmentation
	virtual const std::string& description() const {
		return _description;
	}
	/// Access the underlying decoder
	virtual BitField64* decoder() {
		return _decoder;
	}
	/// Set the underlying decoder
	virtual void setDecoder(BitField64* decoder);
	/// Access to parameter by name
	virtual Parameter parameter(const std::string& parameterName) const;
	/// Access to all parameters
	virtual Parameters parameters() const;
	/// Set all parameters from an existing set of parameters
	virtual void setParameters(const Parameters& parameters);

protected:
	/// Default constructor used by derived classes passing the encoding string
	Segmentation(const std::string& cellEncoding = "");

	/// Add a parameter to this segmentation. Used by derived classes to define their parameters
	virtual void registerParameter(const std::string& name, const std::string& description, double& parameter, const double& defaultValue, bool isOptional = false);
	/// Helper method to convert a bin number to a 1D position
	static double binToPosition(CellID bin, double cellSize, double offset = 0.);
	/// Helper method to convert a 1D position to a cell ID
	static int positionToBin(double position, double cellSize, double offset = 0.);

	/// The cell ID encoder and decoder
	mutable BitField64* _decoder;
	/// Keeps track of the decoder ownership
	bool _ownsDecoder;
	/// The segmentation name
	std::string _name;
	/// The segmentation type
	std::string _type;
	/// The description of the segmentation
	std::string _description;
	/// The parameters for this segmentation
	std::map<std::string, Parameter> _parameters;

private:
	/// No copy constructor allowed
	Segmentation(const Segmentation&);
};

/// Macro to instantiate a new SegmentationCreator by its type name
#define REGISTER_SEGMENTATION(classname) \
	static const SegmentationCreator<classname> classname##_creator(#classname);

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATION_H_ */
