/*
 * SegmentationTest.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DD4hep/LCDD.h"
#include "DD4hep/BitField64.h"

#include "DDSegmentation/CellIDDecoder.h"
#include "DDSegmentation/CellIDDecoderFactory.h"

using namespace std;
using namespace DD4hep;
using namespace Geometry;
using namespace DDSegmentation;

int main(int argc, char** argv) {

	LCDD& lcdd = LCDD::getInstance();
	lcdd.fromCompact(argv[1]);
	lcdd.apply("DD4hepVolumeManager",0,0);

	CellIDDecoder idDecoder = CellIDDecoderFactory::getCellIDDecoder("MuonBarrelHits");

	BitField64 idEncoder(idDecoder.fieldDescription());
	idEncoder["system"] = 10;
	idEncoder["barrel"] = 0;
	idEncoder["layer"] = 15;
	idEncoder["module"] = 8;
	idEncoder["slice"] = 11;
	idEncoder["x"] = 13;
	idEncoder["y"] = -10;
	long64 cellID = idEncoder.getValue();
	Position lp = idDecoder.getLocalPosition(cellID);
	Position gp = idDecoder.getPosition(cellID);
	std::cout << "Local position: " << lp.x() << ", " << lp.y() << ", " << lp.z() << std::endl;
	std::cout << "Global position: " << gp.x() << ", " << gp.y() << ", " << gp.z() << std::endl;

	return 0;
};
