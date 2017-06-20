/*
 * NoSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/NoSegmentation.h"

namespace dd4hep {
  namespace DDSegmentation {

    NoSegmentation::NoSegmentation(const std::string& cellEncoding) :
      Segmentation(cellEncoding)
    {
      _type = "NoSegmentation";
      _description = "None Segmentation";
    }

    NoSegmentation::NoSegmentation(BitField64* decode) : Segmentation(decode)
    { 
      _type = "NoSegmentation";
      _description = "None Segmentation";
    }

    NoSegmentation::~NoSegmentation()
    { }

    Vector3D NoSegmentation::position(const CellID& /*cID*/) const
    {
      Vector3D cellPosition{0,0,0};
      return cellPosition;
    }

    CellID NoSegmentation::cellID(const Vector3D& /*localPosition*/, const Vector3D& /* globalPosition */, const VolumeID& vID) const {
      return vID;
    }

REGISTER_SEGMENTATION(NoSegmentation)

  } /* namespace DDSegmentation */
} /* namespace dd4hep */
