/*
 * SegmentationTest.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DD4hep/LCDD.h"

#include "DDSegmentation/BitField64.h"

using namespace std;
using namespace DD4hep;
using namespace Geometry;
using namespace DDSegmentation;

int main(int argc, char** argv) {

	LCDD& lcdd = LCDD::getInstance();
	lcdd.fromCompact(argv[1]);
	lcdd.apply("DD4hepVolumeManager",0,0);

	Readout ro = lcdd.readout("MuonBarrelHits");

	BitField64 idEncoder(ro.idSpec().fieldDescription());
	idEncoder["system"] = 10;
	idEncoder["barrel"] = 0;
	idEncoder["layer"] = 15;
	idEncoder["module"] = 8;
	idEncoder["slice"] = 11;
	idEncoder["x"] = 13;
	idEncoder["y"] = -10;
	long64 cellID = idEncoder.getValue();
	Position gp = ro.getPosition(cellID);
	std::cout << "Global position: " << gp.x() << ", " << gp.y() << ", " << gp.z() << std::endl;

	return 0;
};
