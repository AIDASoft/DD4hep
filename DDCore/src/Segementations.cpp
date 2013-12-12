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
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {
  void not_implemented_call(const char* tag) {
    throw runtime_error(tag);
  }
  template <typename T> T& _obj(const DDSegmentation::Segmentation* ptr)  {
    // ---> This would be so much more efficient, but sissies refused....
    // T* p = (T*)(((const char*)ptr) - sizeof(Segmentation::Object));
    // return *p;
    const T* p = dynamic_cast<const T*>(ptr);
    return *(T*)p;
  }
}

/// Standard constructor
Segmentation::Object::Object()
  : magic(magic_word()), useForHitPosition(0), segmentation(0) {
  InstanceCount::increment(this);
}

/// Default destructor
Segmentation::Object::~Object() {
  InstanceCount::decrement(this);
}

/// Accessor: Segmentation type
std::string Segmentation::type() const {
  return segmentation()->type();
}

bool Segmentation::useForHitPosition() const {
  return _obj<Object>(ptr()).useForHitPosition != 0;
}

/// Segmentation type
string SegmentationParams::type() const {
  return _obj<Object>(ptr()).segmentation->type();
}

/// Access to the parameters
SegmentationParams::Parameters SegmentationParams::parameters() const {
  Object& o = _obj<Object>(ptr());
  if (o.segmentation != 0)  {
    return o.segmentation->parameters();
  }
  throw runtime_error("DD4hep: The segmentation object " + string(type()) + " is invalid!");
}

/// Access segmentation object
DD4hep::DDSegmentation::Segmentation* Segmentation::segmentation() const {
  Object& o = _obj<Object>(ptr());
  if (o.segmentation != 0)
    return o.segmentation;
  throw runtime_error(
      "DD4hep: The segmentation object " + string(type())
          + " knows no implementation object [This is no longer allowed in the presence of DDSegmentation]");
}

/// Default destructor
ProjectiveCylinder::Data::~Data() {
}

/// determine the local position based on the cell ID
std::vector<double> ProjectiveCylinder::Data::getPosition(const long64& cellID) const {
  not_implemented_call("ProjectiveCylinder::Data::getLocalPosition");
  return vector<double>();
}

/// determine the local position based on the cell ID
std::vector<double> ProjectiveCylinder::Data::getLocalPosition(const long64& cellID) const {
  not_implemented_call("ProjectiveCylinder::Data::getLocalPosition");
  return vector<double>();
}

/// determine the cell ID based on the local position
long64 ProjectiveCylinder::Data::getCellID(double x, double y, double z) const {
  not_implemented_call("ProjectiveCylinder::Data::getCellID");
  return 0;
}

/// Constructor to create a new segmentation object
ProjectiveCylinder::ProjectiveCylinder(LCDD&)
    : Segmentation(new Data(), "segmentation", "projective_cylinder") {
}

/// Accessors: get number of bins in theta
int ProjectiveCylinder::thetaBins() const {
  return _obj<Data>(ptr()).ntheta;
}

/// Accessors: get number of bins in phi
int ProjectiveCylinder::phiBins() const {
  return _obj<Data>(ptr()).nphi;
}

/// Accessors: get number of bins in z
int ProjectiveCylinder::zBins() const {
  return _obj<Data>(ptr()).nz;
}

/// Accessors: set number of bins in theta
void ProjectiveCylinder::setThetaBins(int value) {
  _obj<Data>(ptr()).ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveCylinder::setPhiBins(int value) {
  _obj<Data>(ptr()).nphi = value;
}

/// Accessors: set number of bins in Z
void ProjectiveCylinder::setZBins(int value) {
  _obj<Data>(ptr()).nz = value;
}

/// Default destructor
NonProjectiveCylinder::Data::~Data() {
}

/// determine the local position based on the cell ID
std::vector<double> NonProjectiveCylinder::Data::getPosition(const long64& cellID) const {
  not_implemented_call("NonProjectiveCylinder::Data::getLocalPosition");
  return vector<double>();
}

/// determine the local position based on the cell ID
std::vector<double> NonProjectiveCylinder::Data::getLocalPosition(const long64& cellID) const {
  not_implemented_call("NonProjectiveCylinder::Data::getLocalPosition");
  return vector<double>();
}

/// determine the cell ID based on the local position
long64 NonProjectiveCylinder::Data::getCellID(double x, double y, double z) const {
  not_implemented_call("NonProjectiveCylinder::Data::getCellID");
  return 0;
}

/// Constructor to create a new segmentation object
NonProjectiveCylinder::NonProjectiveCylinder(LCDD&)
    : Segmentation(new Data(), "segmentation", "nonprojective_cylinder") {
}

/// Accessors: get size of bins in Z
double NonProjectiveCylinder::gridSizeZ() const {
  return _obj<Data>(ptr()).grid_size_z;
}

/// Accessors: get size of bins in phi
double NonProjectiveCylinder::gridSizePhi() const {
  return _obj<Data>(ptr()).grid_size_phi;
}

/// Accessors: set number of bins in theta
void NonProjectiveCylinder::setThetaBinSize(double value) {
  _obj<Data>(ptr()).grid_size_phi = value;
}

/// Accessors: set grid size in Y
void NonProjectiveCylinder::setPhiBinSize(double value) {
  _obj<Data>(ptr()).grid_size_z = value;
}

/// Default destructor
ProjectiveZPlane::Data::~Data() {
}

/// determine the local position based on the cell ID
std::vector<double> ProjectiveZPlane::Data::getLocalPosition(const long64& cellID) const {
  not_implemented_call("ProjectiveZPlane::Data::getLocalPosition");
  return vector<double>();
}

/// determine the local position based on the cell ID
std::vector<double> ProjectiveZPlane::Data::getPosition(const long64& cellID) const {
  not_implemented_call("ProjectiveZPlane::Data::getLocalPosition");
  return vector<double>();
}

/// determine the cell ID based on the local position
long64 ProjectiveZPlane::Data::getCellID(double x, double y, double z) const {
  not_implemented_call("ProjectiveZPlane::Data::getCellID");
  return 0;
}

/// Constructor to be used when creating a new object.
ProjectiveZPlane::ProjectiveZPlane(LCDD&)
    : Segmentation(new Data(), "segmentation", "projective_zplane") {
}

/// Accessors: get number of bins in phi
int ProjectiveZPlane::phiBins() const {
  return _obj<Data>(ptr()).nphi;
}

/// Accessors: get number of bins in theta
int ProjectiveZPlane::thetaBins() const {
  return _obj<Data>(ptr()).ntheta;
}

/// Accessors: set number of bins in theta
void ProjectiveZPlane::setThetaBins(int value) {
  _obj<Data>(ptr()).ntheta = value;
}

/// Accessors: set grid size in Y
void ProjectiveZPlane::setPhiBins(int value) {
  _obj<Data>(ptr()).nphi = value;
}

/// Default destructor
GridXY::Data::~Data() {
}

/// Constructor to be used when creating a new object.
GridXY::GridXY(LCDD&, const std::string& typ)
    : Segmentation(new Data(), "segmentation", typ) {
}

/// Accessors: get grid size in X
double GridXY::getGridSizeX() const {
  return _obj<Data>(ptr()).getGridSizeX();
}

/// Accessors: get grid size in Y
double GridXY::getGridSizeY() const {
  return _obj<Data>(ptr()).getGridSizeY();
}

/// Accessors: set grid size in X
void GridXY::setGridSizeX(double value) {
  _obj<Data>(ptr()).setGridSizeX(value);
}

/// Accessors: set grid size in Y
void GridXY::setGridSizeY(double value) {
  _obj<Data>(ptr()).setGridSizeY(value);
}

/// Default destructor
GridXYZ::Data::~Data() {
}

/// Constructor to be used when creating a new object.
GridXYZ::GridXYZ(LCDD&, const std::string& typ)
    : Segmentation(new Data(), "segmentation", typ) {
  assign(new Data(), "segmentation", typ);
}

/// Accessors: get grid size in X
double GridXYZ::getGridSizeX() const {
  return _obj<Data>(ptr()).getGridSizeX();
}

/// Accessors: get grid size in Y
double GridXYZ::getGridSizeY() const {
  return _obj<Data>(ptr()).getGridSizeY();
}

/// Accessors: get grid size in Z
double GridXYZ::getGridSizeZ() const {
  return _obj<Data>(ptr()).getGridSizeZ();
}

/// Accessors: set grid size in X
void GridXYZ::setGridSizeX(double value) {
  _obj<Data>(ptr()).setGridSizeX(value);
}

/// Accessors: set grid size in Y
void GridXYZ::setGridSizeY(double value) {
  _obj<Data>(ptr()).setGridSizeY(value);
}

/// Accessors: set grid size in Z
void GridXYZ::setGridSizeZ(double value) {
  _obj<Data>(ptr()).setGridSizeZ(value);
}
