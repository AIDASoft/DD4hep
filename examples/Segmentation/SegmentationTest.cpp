/*
 * SegmentationTest.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DD4hep/Detector.h"

#include "DDSegmentation/SegmentationFactory.h"
#include "DDSegmentation/SegmentationParameter.h"

#include <set>

using namespace std;
using namespace dd4hep;
using namespace detail;
using namespace DDSegmentation;

int main(int argc, char** argv) {

	SegmentationFactory* f = SegmentationFactory::instance();

	cout << "Registered Segmentations:" << std::endl;
	vector<string> segmentations = f->registeredSegmentations();
	vector<string>::const_iterator it;
	for (it = segmentations.begin(); it != segmentations.end(); ++it) {
		string typeName = *it;
		DDSegmentation::Segmentation* s = f->create(typeName);
		cout << "\t" << typeName << ", " << s->type() << endl;
		Parameters parameters = s->parameters();
		Parameters::iterator it;
		for (it = parameters.begin(); it != parameters.end(); ++it) {
			Parameter p = *it;
			cout << "\t\t" << p->name() << " = " << p->value() << endl;
		}
		delete s;
	}

	DDSegmentation::Segmentation* s = f->create("CartesianGridXY", "system:8,barrel:3,module:4,layer:8,slice:5,x:32:-16,y:-16");
	BitField64& d = *s->decoder();
	d["system"] = 1;
	d["barrel"] = 0;
	d["module"] = 5;
	d["layer"] = 12;
	d["x"] = 10;
	d["y"] = -30;
	cout << "Neighbours of " << d.valueString() << ": "<< endl;
	CellID id = d.getValue();
	set<CellID> neighbours;
	s->neighbours(id, neighbours);
	set<CellID>::iterator itNeighbour;
	for (itNeighbour = neighbours.begin(); itNeighbour != neighbours.end(); ++itNeighbour) {
		d.setValue(*itNeighbour);
		cout << "\t" << d.valueString() << std::endl;
	}
	delete s;
	return 0;
};
