/*
 * SegmentationFactory.h
 *
 * Factory and helper classes to allow instantiation of segmentations by name.
 *
 *  Created on: Dec 15, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_SEGMENTATIONFACTORY_H_
#define DDSegmentation_SEGMENTATIONFACTORY_H_

#include "DDSegmentation/Segmentation.h"

#include <map>
#include <vector>
#include <string>

namespace DD4hep {
namespace DDSegmentation {

/// Forward declaration required because of circular dependency
class Segmentation;
class SegmentationCreatorBase;

/// Base class for the SegmentationCreator objects. Allows to use the factory without template.
class SegmentationCreatorBase {
public:
	/// Default constructor. Takes the class name as argument and takes care of registration with the factory
	SegmentationCreatorBase(const std::string& name);
	/// Destructor
	virtual ~SegmentationCreatorBase() {};
	/// Create a new object
	virtual Segmentation* create(const std::string& identifier) const = 0;
};

/// Concrete class to create segmentation objects. Every segmentation needs to instantiate a static instance of this.
template<class TYPE> class SegmentationCreator : public SegmentationCreatorBase {
public:
	/// Default constructor. Takes the class name as argument which should match the class type
	SegmentationCreator<TYPE>(const std::string& name) : SegmentationCreatorBase(name) {};
	/// Destructor
	virtual ~SegmentationCreator() {};
	/// Create a new object of the given type.
	Segmentation* create(const std::string& identifier) const {return new TYPE(identifier);};
};

/// Factory for creating segmentation objects by type name
class SegmentationFactory {
	/// Allow SegmentationCreators to register themselves with the factory
	friend class SegmentationCreatorBase;
public:
	/// Access to the global factory instance
	static SegmentationFactory* instance();
	/// Create a new segmentation object with the given type name. Returns NULL if type name is unknown.
	Segmentation* create(const std::string& name, const std::string& identifier = "") const;
	/// Access to the list of registered segmentations
	std::vector<std::string> registeredSegmentations() const;
protected:
	/// Default constructor
	SegmentationFactory() {};
	/// Copy constructor
	SegmentationFactory(const SegmentationFactory&) {};
	/// Destructor
	virtual ~SegmentationFactory() {};
	/// Registers a new SegmentationCreator with the factory
	void registerSegmentation(const std::string& name, SegmentationCreatorBase* creator);
	/// Map to store SegmentationCreators by name
	std::map<std::string, SegmentationCreatorBase*> _segmentations;
private:
	/// The global factory instance
	static SegmentationFactory* _instance;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_SEGMENTATIONFACTORY_H_ */
