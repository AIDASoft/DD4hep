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

using namespace std;
using namespace DD4hep::Geometry;

/// Standard constructor
Readout::Object::Object()  {
  InstanceCount::increment(this);
}

/// Default destructor
Readout::Object::~Object()  {
  destroyHandle(segmentation);
  InstanceCount::decrement(this);
}

/// Initializing constructor to create a new object
Readout::Readout(const string& nam)
{
  assign(new Object(),nam,"readout");
}

/// Assign IDDescription to readout structure
void Readout::setIDDescriptor(const Ref_t& new_descriptor)  const   {
  if ( isValid() )  {                    // Remember: segmentation is NOT owned by readout structure!
    if ( new_descriptor.isValid() )  {   // Do NOT delete!
      data<Object>()->id = new_descriptor;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign ID descriptor [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor Readout::idSpec() const   {
  return object<Object>().id;
}

/// Assign segmentation structure to readout
void Readout::setSegmentation(const Segmentation& seg)   const  {
  if ( isValid() )  {
    Object& ro = object<Object>();
    Segmentation::Implementation* e = ro.segmentation.ptr();
    if ( e )  { // Remember: segmentation is owned by readout structure!
      delete e; // Need to delete the segmentation object
    }
    if ( seg.isValid() )  {
      ro.segmentation = seg;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}

/// Access segmentation structure
Segmentation Readout::segmentation() const  {
  return object<Object>().segmentation;
}

/// Standard constructor
Alignment::Object::Object()  {
  InstanceCount::increment(this);
}

/// Default destructor
Alignment::Object::~Object()  {
  InstanceCount::decrement(this);
}

/// Initializing constructor to create a new object
Alignment::Alignment(const LCDD& /* lcdd */, const string& nam)
{
  assign(new Object(),nam,"alignment");
}

/// Standard constructor
Conditions::Object::Object()  {
  InstanceCount::increment(this);
}

/// Default destructor
Conditions::Object::~Object()  {
  InstanceCount::decrement(this);
}

/// Initializing constructor to create a new object
Conditions::Conditions(const LCDD& /* lcdd */, const string& nam)
{
  assign(new Object(),nam,"conditions");
}
