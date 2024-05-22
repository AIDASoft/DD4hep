//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
/*
 * NoSegmentation.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include <DDSegmentation/NoSegmentation.h>

namespace dd4hep {
  namespace DDSegmentation {

    NoSegmentation::NoSegmentation(const std::string& cellEncoding) :
      Segmentation(cellEncoding)
    {
      _type = "NoSegmentation";
      _description = "None Segmentation";
    }

    NoSegmentation::NoSegmentation(const BitFieldCoder* decode) : Segmentation(decode)
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


  } /* namespace DDSegmentation */
} /* namespace dd4hep */

