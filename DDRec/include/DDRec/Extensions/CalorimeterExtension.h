/*
 * CalorimeterExtension.h
 *
 * Abstract extension used by the Calorimeter class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef rec_CALORIMETEREXTENSION_H_
#define rec_CALORIMETEREXTENSION_H_

namespace dd4hep {
namespace rec {

class [[gnu::deprecated(" unmaintained code ")]] CalorimeterExtension {
public:
	CalorimeterExtension();
	virtual ~CalorimeterExtension();
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* rec_CALORIMETEREXTENSION_H_ */
