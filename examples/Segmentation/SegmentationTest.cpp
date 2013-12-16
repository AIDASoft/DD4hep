/*
 * SegmentationTest.cpp
 *
 *  Created on: Aug 28, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DD4hep/LCDD.h"

#include "DDSegmentation/SegmentationFactory.h"
#include "DDSegmentation/SegmentationParameter.h"

using namespace std;
using namespace DD4hep;
using namespace Geometry;
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
			cout << "\t\t" << it->first << " = " << it->second << endl;
		}
	}
	return 0;
};
