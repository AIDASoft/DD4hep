// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Segmentations.h"
#include <iostream>

using namespace std;
using namespace DD4hep::Geometry;

Segmentation::Object::Object() : magic(magic_word()), type(REGULAR), useForHitPosition(0) {
  ::memset(data.values,0,sizeof(data.values));
}

Segmentation::Object::~Object() {
  if ( type == EXTENDED && data.extension.ptr != 0 ) {
    if ( data.extension.destructor ) {
      (*data.extension.destructor)(data.extension.ptr);
      data.extension.destructor = 0;
      data.extension.info = 0;
      data.extension.ptr = 0;
    }
  }
}

Segmentation::Segmentation(const string& type)  {
  assign(new Value<TNamed,Segmentation::Object>(),"segmentation",type);
}

bool Segmentation::useForHitPosition() const   {
  return _data().useForHitPosition != 0;
}

const string Segmentation::type() const   {
  return m_element->GetTitle();
}

/// Add an extension object to the detector element
void* Segmentation::i_setExtension(void* ptr, const type_info& info, void (*destruct)(void*)) {
  Object& o = _data();
  o.type = EXTENDED;
  o.data.extension.ptr = ptr;
  o.data.extension.info = &info;
  o.data.extension.destructor = destruct;
  return ptr;
}

/// Access an existing extension object from the detector element
void* Segmentation::i_extension(const type_info& info)   const {
  if ( isValid() ) {
    Object::Data::Extension& o = _data().data.extension;
    if ( o.ptr )   {
      if ( &info == o.info ) {
	return o.ptr;
      }
      throw runtime_error("extension: The segmentation object "+string(type())+
			  " has the wrong type!");
    }
    throw runtime_error("extension: The segmentation object "+string(type())+
			" has no extension defined.");
  }
  throw runtime_error("extension: The segmentation object is not valid!");
}
 
ProjectiveCylinder::ProjectiveCylinder() 
: Segmentation("projective_cylinder")   {}

/// Accessors: get number of bins in theta
int ProjectiveCylinder::thetaBins() const    {
  return _data().data.cylindrical_binning.ntheta;
}

/// Accessors: get number of bins in phi
int ProjectiveCylinder::phiBins() const    {
  return _data().data.cylindrical_binning.nphi;
}

/// Accessors: set number of bins in theta
void ProjectiveCylinder::setThetaBins(int value)    {
  _data().data.cylindrical_binning.ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveCylinder::setPhiBins(int value)   {
  _data().data.cylindrical_binning.nphi = value;
}

NonProjectiveCylinder::NonProjectiveCylinder()
: Segmentation("nonprojective_cylinder")
{
}

double NonProjectiveCylinder::gridSizeZ()  const   {
  return _data().data.cylindrical_grid.grid_size_z;
}

double NonProjectiveCylinder::gridSizePhi()  const   {
  return _data().data.cylindrical_grid.grid_size_phi;
}

/// Accessors: set number of bins in theta
void NonProjectiveCylinder::setThetaBinSize(double value)   {
  _data().data.cylindrical_grid.grid_size_phi = value;
}

/// Accessors: set grid size in Y
void NonProjectiveCylinder::setPhiBinSize(double value)   {
  _data().data.cylindrical_grid.grid_size_z = value;
}


/// Constructor to be used when creating a new DOM tree.
ProjectiveZPlane::ProjectiveZPlane() 
: Segmentation("projective_zplane")
{
}

/// Accessors: get number of bins in phi
int ProjectiveZPlane::phiBins() const  {
  return _data().data.cylindrical_binning.nphi;
}

/// Accessors: get number of bins in theta
int ProjectiveZPlane::thetaBins() const  {
  return _data().data.cylindrical_binning.ntheta;
}

/// Accessors: set number of bins in theta
void ProjectiveZPlane::setThetaBins(int value)  {
  _data().data.cylindrical_binning.ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveZPlane::setPhiBins(int value)  {
  _data().data.cylindrical_binning.nphi = value;
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
  _data().data.cartesian_grid.grid_size_x = size_x;
  _data().data.cartesian_grid.grid_size_y = size_y;
}

/// Accessors: get grid size in X
double GridXY::getGridSizeX() const {
  return _data().data.cartesian_grid.grid_size_x;
}

/// Accessors: get grid size in Y
double GridXY::getGridSizeY() const {
  return _data().data.cartesian_grid.grid_size_y;
}

/// Accessors: set grid size in X
void GridXY::setGridSizeX(double value)  {
  _data().data.cartesian_grid.grid_size_x = value;
}

/// Accessors: set grid size in Y
void GridXY::setGridSizeY(double value)  {
  _data().data.cartesian_grid.grid_size_y = value;
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
  _data().data.cartesian_grid.grid_size_z = size_z;
}

void GridXYZ::setGridSizeZ(double value)  {
  _data().data.cartesian_grid.grid_size_z = value;
}
