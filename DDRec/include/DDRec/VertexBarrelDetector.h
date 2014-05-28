/*
 * VertexBarrelDetector.h
 *
 *  Created on: 17/02/2014
 *      Author: Markus Frank, CERN
 */
#ifndef VERTEXBARRELDETECTOR_H_
#define VERTEXBARRELDETECTOR_H_

#include "DDRec/BarrelDetector.h"
#include "DDRec/LayeredSubdetector.h"

namespace DD4hep {
  namespace DDRec {

    class VertexBarrelDetector
      : virtual public BarrelDetector, 
      virtual public LayeredSubdetector
      {
    public:
	typedef Geometry::DetElement DetElement;
      /// Constructor from handle
      VertexBarrelDetector(const DetElement& e);
      /// Standard destructor
      virtual ~VertexBarrelDetector();
    };

  } /* namespace DDRec */
} /* namespace DD4hep */
#endif /* VERTEXBARRELDETECTOR_H_ */
