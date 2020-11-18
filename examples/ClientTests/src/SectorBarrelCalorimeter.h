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
 * PolyhedralBarrelCalorimeter.h
 *
 *  Created on: May 19, 2014
 *      Author: Nikiforos Nikiforou, CERN
 */

#ifndef EXAMPLES_CLIENTTESTS_SRC_SECTORBARRELCALORIMETER_H
#define EXAMPLES_CLIENTTESTS_SRC_SECTORBARRELCALORIMETER_H

#include "BarrelDetector.h"
#include "PolyhedralCalorimeter.h"

namespace dd4hep {
namespace detail {

class SectorBarrelCalorimeter: public BarrelDetector, public PolyhedralCalorimeter {
public:
	SectorBarrelCalorimeter(const DetElement& e) : DetElement(e), BarrelDetector(e), PolyhedralCalorimeter(e) {}
	virtual ~SectorBarrelCalorimeter() {}
};

} /* namespace detail */
} /* namespace dd4hep */
#endif // EXAMPLES_CLIENTTESTS_SRC_SECTORBARRELCALORIMETER_H
