#include "Readout.h"
#include "Internals.h"

using namespace std;
using namespace DetDesc::Geometry;

/// Constructor to be used when reading the already parsed object
Readout::Readout(Handle_t e) : RefElement(e) {
}

/// Initializing constructor to create a new object
Readout::Readout(const Document& doc, const string& nam) : RefElement(doc,"readout",nam) 
{
}

/// Access IDDescription structure
RefElement Readout::idSpec() const   {
  return second_value<TNamed>(*this)->id;
}

/// Access segmentation structure
Element Readout::segmentation() const  {
  return second_value<TNamed>(*this)->segmentation;
}

/// Assign IDDescription to readout structure
void Readout::setIDDescriptor(RefElement new_descriptor)  const   {
  Object* ro = second_value<TNamed>(*this);
  if ( ro )  {                 // Remember: segmentation is NOT owned by readout structure!
    if ( new_descriptor.isValid() )  {   // Do NOT delete!
      ro->id = new_descriptor;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign ID descriptor [Invalid Handle]");
}

/// Assign segmentation structure to readout
void Readout::setSegmentation(Element seg)   const  {
  Object* ro = second_value<TNamed>(*this);
  if ( ro )  {
    Element_t* e = ro->segmentation.ptr();
    if ( e )  { // Remember: segmentation is owned by readout structure!
      delete e; // Need to delete the segmentation object
    }
    if ( seg.isValid() )  {
      ro->segmentation = seg;
      return;
    }
  }
  throw runtime_error("Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}
