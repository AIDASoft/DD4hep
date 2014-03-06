/*
 * SegmentationTest.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DD4hep/LCDD.h"
#include "DD4hep/VolumeManager.h"

#include "DDReconstruction/API/IDDecoder.h"

//#include <set>
//#include <string>

using namespace std;
using namespace DD4hep;
using namespace Geometry;

int main(int argc, char** argv) {
	LCDD& lcdd = LCDD::getInstance();
	lcdd.fromCompact(argv[1]);

	string collectionName = "MuonBarrelHits";
	Readout r = lcdd.readout(collectionName);
	BitField64& d = *(r.segmentation()->decoder());

	d.reset();
	d["system"] = 10;
	d["barrel"] = 0;
	d["layer"] = 18;
	d["module"] = 8;
	//d["slice"] = 11;
	//d["x"] = -32;
	//d["y"] = 25;

	CellID cellID = d.getValue();

	cout << "CellID: " << cellID << endl;
	IDDecoder id(collectionName);
	cout << id.detectorElement(cellID).name() << endl;
	cout << id.position(cellID) << endl;
};
