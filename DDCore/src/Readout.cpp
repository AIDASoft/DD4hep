// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Readout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/TGeoUnits.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using tgeo::mm;

/// Standard constructor
Readout::Object::Object() {
  InstanceCount::increment(this);
}

/// Default destructor
Readout::Object::~Object() {
  //destroyHandle(segmentation);
  InstanceCount::decrement(this);
  if ( segmentation.isValid() )  {
    delete segmentation.ptr();
    segmentation = Segmentation();
  }
}

/// Initializing constructor to create a new object
Readout::Readout(const string& nam) {
  assign(new Object(), nam, "readout");
}

/// Assign IDDescription to readout structure
void Readout::setIDDescriptor(const Ref_t& new_descriptor) const {
  if (isValid()) {                    // Remember: segmentation is NOT owned by readout structure!
    if (new_descriptor.isValid()) {   // Do NOT delete!
      data<Object>()->id = new_descriptor;
      Segmentation seg = data<Object>()->segmentation;
      IDDescriptor id = new_descriptor;
      if (seg.isValid()) {
        seg.segmentation()->setDecoder(id.decoder());
      }
      return;
    }
  }
  throw runtime_error("DD4hep: Readout::setIDDescriptor: Cannot assign ID descriptor [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor Readout::idSpec() const {
  return object<Object>().id;
}

/// Assign segmentation structure to readout
void Readout::setSegmentation(const Segmentation& seg) const {
  if (isValid()) {
    Object& ro = object<Object>();
    Segmentation::Implementation* e = ro.segmentation.ptr();
    if (e) {   // Remember: segmentation is owned by readout structure!
      delete e;   // Need to delete the segmentation object
    }
    if (seg.isValid()) {
      ro.segmentation = seg;
      return;
    }
  }
  throw runtime_error("DD4hep: Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}

/// Access segmentation structure
Segmentation Readout::segmentation() const {
  return object<Object>().segmentation;
}
