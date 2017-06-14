/*
 * CalorimeterExtension.h
 *
 * Abstract extension used by the Calorimeter class.
 *
 *  Created on: Dec 11, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDRec_CALORIMETEREXTENSION_H_
#define DDRec_CALORIMETEREXTENSION_H_

namespace DD4hep {
namespace DDRec {

class  [[gnu::deprecated(" unmaintained code ")]] CalorimeterExtension {
public:
	CalorimeterExtension();
	virtual ~CalorimeterExtension();
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* DDRec_CALORIMETEREXTENSION_H_ */
