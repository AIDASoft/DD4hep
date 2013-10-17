// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Segmentations.h"
#include "DD4hep/InstanceCount.h"
#include <iostream>

using namespace std;
using namespace DD4hep::Geometry;

/// Standard constructor
Segmentation::Object::Object() 
: magic(magic_word()), type(REGULAR), useForHitPosition(0) 
{
  segmentation = 0;
  InstanceCount::increment(this);
  ::memset(data.values,0,sizeof(data.values));
  _spare[5]=_spare[4]=_spare[3]=_spare[2]=_spare[1]=_spare[0]=0;
}

/// Default destructor
Segmentation::Object::~Object() {
  if ( type == EXTENDED && data.extension.ptr != 0 ) {
    if ( data.extension.destructor ) {
      (*data.extension.destructor)(data.extension.ptr);
      data.extension.destructor = 0;
      data.extension.info = 0;
      data.extension.ptr = 0;
    }
  }
  if (segmentation != 0) {
	  delete segmentation;
  }
  InstanceCount::decrement(this);
}

Segmentation::Segmentation(const string& type)  {
  assign(new Object(),"segmentation",type);
}

bool Segmentation::useForHitPosition() const   {
  return object<Object>().useForHitPosition != 0;
}

/// Segmentation type
const string Segmentation::type() const   {
  return object<Object>().segmentation->type();
}

/// Assign segmentation object
void Segmentation::setSegmentation(DD4hep::DDSegmentation::Segmentation* segmentation) {
  Object& o = object<Object>();
  if (o.segmentation != 0) {
    delete o.segmentation;
  }
  o.segmentation = segmentation;
}

/// Access segmentation object
DD4hep::DDSegmentation::Segmentation* Segmentation::segmentation() {
  Object& o = object<Object>();
  if (o.segmentation != 0)
    return o.segmentation;
  throw runtime_error("DD4hep: The segmentation object "+string(type())+
		      " knows no implementation object [This is no longer allowed in the presence of DDSegmentation]");
}

/// Add an extension object to the detector element
void* Segmentation::i_setExtension(void* ptr, const type_info& info, void (*destruct)(void*)) {
  Object& o = object<Object>();
  o.type = EXTENDED;
  o.data.extension.ptr = ptr;
  o.data.extension.info = &info;
  o.data.extension.destructor = destruct;
  return ptr;
}

/// Access an existing extension object from the detector element
void* Segmentation::i_extension(const type_info& info)   const {
  if ( isValid() ) {
    Object::Data::Extension& o = object<Object>().data.extension;
    if ( o.ptr )   {
      if ( &info == o.info ) {
	return o.ptr;
      }
      throw runtime_error("DD4hep: extension: The segmentation object "+string(type())+
			  " has the wrong type!");
    }
    throw runtime_error("DD4hep: extension: The segmentation object "+string(type())+
			" has no extension defined.");
  }
  throw runtime_error("DD4hep: extension: The segmentation object is not valid!");
}
 
/// Segmentation type
const string SegmentationParams::type() const   {
  return m_element->GetTitle();
}

/// Access to the parameters
SegmentationParams::Parameters SegmentationParams::parameters() const  {
  return object<Object>().segmentation->parameters();
}

ProjectiveCylinder::ProjectiveCylinder() : Segmentation("projective_cylinder")   
{
}

/// Accessors: get number of bins in theta
int ProjectiveCylinder::thetaBins() const    {
  return object<Object>().data.cylindrical_binning.ntheta;
}

/// Accessors: get number of bins in phi
int ProjectiveCylinder::phiBins() const    {
  return object<Object>().data.cylindrical_binning.nphi;
}

/// Accessors: set number of bins in theta
void ProjectiveCylinder::setThetaBins(int value)    {
  object<Object>().data.cylindrical_binning.ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveCylinder::setPhiBins(int value)   {
  object<Object>().data.cylindrical_binning.nphi = value;
}

NonProjectiveCylinder::NonProjectiveCylinder()
: Segmentation("nonprojective_cylinder")
{
}

double NonProjectiveCylinder::gridSizeZ()  const   {
  return object<Object>().data.cylindrical_grid.grid_size_z;
}

double NonProjectiveCylinder::gridSizePhi()  const   {
  return object<Object>().data.cylindrical_grid.grid_size_phi;
}

/// Accessors: set number of bins in theta
void NonProjectiveCylinder::setThetaBinSize(double value)   {
  object<Object>().data.cylindrical_grid.grid_size_phi = value;
}

/// Accessors: set grid size in Y
void NonProjectiveCylinder::setPhiBinSize(double value)   {
  object<Object>().data.cylindrical_grid.grid_size_z = value;
}


/// Constructor to be used when creating a new DOM tree.
ProjectiveZPlane::ProjectiveZPlane() 
: Segmentation("projective_zplane")
{
}

/// Accessors: get number of bins in phi
int ProjectiveZPlane::phiBins() const  {
  return object<Object>().data.cylindrical_binning.nphi;
}

/// Accessors: get number of bins in theta
int ProjectiveZPlane::thetaBins() const  {
  return object<Object>().data.cylindrical_binning.ntheta;
}

/// Accessors: set number of bins in theta
void ProjectiveZPlane::setThetaBins(int value)  {
  object<Object>().data.cylindrical_binning.ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveZPlane::setPhiBins(int value)  {
  object<Object>().data.cylindrical_binning.nphi = value;
}

GridXY::GridXY() 
: Segmentation("grid_xy")   {}

/// Constructor to be used when creating a new object. Data are taken from the input handle
GridXY::GridXY(const string& tag) 
: Segmentation(tag) 
{
}

/// Constructor to be used when creating a new object.
GridXY::GridXY(const string& tag, double size_x, double size_y)
: Segmentation(tag)
{
  object<Object>().data.cartesian_grid.grid_size_x = size_x;
  object<Object>().data.cartesian_grid.grid_size_y = size_y;
}

/// Accessors: get grid size in X
double GridXY::getGridSizeX() const {
  return object<Object>().data.cartesian_grid.grid_size_x;
}

/// Accessors: get grid size in Y
double GridXY::getGridSizeY() const {
  return object<Object>().data.cartesian_grid.grid_size_y;
}

/// Accessors: set grid size in X
void GridXY::setGridSizeX(double value)  {
  object<Object>().data.cartesian_grid.grid_size_x = value;
}

/// Accessors: set grid size in Y
void GridXY::setGridSizeY(double value)  {
  object<Object>().data.cartesian_grid.grid_size_y = value;
}

/// Constructor to be used when creating a new DOM tree.
GridXYZ::GridXYZ()   
: GridXY("grid_xyz")
{
}

/// Constructor to be used when creating a new object.
GridXYZ::GridXYZ(double size_x, double size_y, double size_z)
: GridXY("grid_xyz", size_x, size_y)
{
  object<Object>().data.cartesian_grid.grid_size_z = size_z;
}

void GridXYZ::setGridSizeZ(double value)  {
  object<Object>().data.cartesian_grid.grid_size_z = value;
}
