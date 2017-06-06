//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Segmentations.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/SegmentationsInterna.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(SegmentationObject);

/// Constructor to used when creating a new object
Segmentation::Segmentation(const string& typ, const string& nam, BitField64* dec) : Handle<Object>()
{
  string seg_type = "segmentation_constructor__"+typ;
  SegmentationObject* obj = PluginService::Create<SegmentationObject*>(seg_type, dec);
  if ( obj != 0 )  {
    assign(obj, nam, typ);
    if ( !nam.empty() ) obj->setName(nam);
    return;
  }
  // This is fatal and cannot be recovered. We need to throw an exception here.
  except("Segmentation","FAILED to create segmentation: %s. [Missing factory]",typ.c_str());
}

/// Accessor: Segmentation type
std::string Segmentation::type() const {
  return data<Object>()->type();
}

bool Segmentation::useForHitPosition() const {
  return data<Object>()->useForHitPosition != 0;
}

/// Access to the parameters
Segmentation::Parameters Segmentation::parameters() const {
  return data<Object>()->parameters();
}

/// Access to parameter by name
Segmentation::Parameter  Segmentation::parameter(const std::string& parameterName) const   {
  return data<Object>()->parameter(parameterName);
}

/// determine the local position based on the cell ID
Position Segmentation::position(const long64& cell) const {
  return Position(data<Object>()->segmentation->position(cell));
}

/// determine the cell ID based on the local position
long64 Segmentation::cellID(const Position& localPosition, const Position& globalPosition, const long64& volID) const {
  return data<Object>()->segmentation->cellID(localPosition, globalPosition, volID);
}

/// Determine the volume ID from the full cell ID by removing all local fields
VolumeID Segmentation::volumeID(const CellID& cell) const   {
  return data<Object>()->segmentation->volumeID(cell);
}

/// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
void Segmentation::neighbours(const CellID& cell, std::set<CellID>& nb) const  {
  data<Object>()->segmentation->neighbours(cell, nb);
}

/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
 *  in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
 *
 *   \param cellID cellID of the cell for which parameters are returned
 *   \return vector<double> in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi
 */
std::vector<double> Segmentation::cellDimensions(const CellID& cell) const  {
  return data<Object>()->segmentation->cellDimensions(cell);
}

/// Access to the base DDSegmentation object. WARNING: Deprecated call!
Segmentation::BaseSegmentation* Segmentation::segmentation() const  {
  return data<Object>()->segmentation;
}

/// Access the underlying decoder
BitField64* Segmentation::decoder()  const {
  return data<Object>()->segmentation->decoder();
}

/// Set the underlying decoder
void Segmentation::setDecoder(BitField64* decode) const  {
  data<Object>()->segmentation->setDecoder(decode);
}

/// Access the main detector element using this segmetnation object
Handle<DetElementObject> Segmentation::detector() const  {
  return data<Object>()->detector;
}

/// Access the sensitive detector using this segmetnation object
Handle<SensitiveDetectorObject> Segmentation::sensitive() const  {
  return data<Object>()->sensitive;
}

#define IMPLEMENT_SEGMENTATION_HANDLE(X)                                \
  DD4HEP_INSTANTIATE_HANDLE_UNNAMED(SegmentationWrapper<DDSegmentation::X>);

#include "DDSegmentation/NoSegmentation.h"
IMPLEMENT_SEGMENTATION_HANDLE(NoSegmentation)

#include "DDSegmentation/CartesianGrid.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGrid)

#include "DDSegmentation/CartesianGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXY)

#include "DDSegmentation/CartesianGridXZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXZ)

#include "DDSegmentation/CartesianGridYZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridYZ)

#include "DDSegmentation/CartesianGridXYZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXYZ)

#include "DDSegmentation/TiledLayerGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(TiledLayerGridXY)

#include "DDSegmentation/MegatileLayerGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(MegatileLayerGridXY)

#include "DDSegmentation/WaferGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(WaferGridXY)

#include "DDSegmentation/PolarGridRPhi.h"
IMPLEMENT_SEGMENTATION_HANDLE(PolarGridRPhi)

#include "DDSegmentation/PolarGridRPhi2.h"
IMPLEMENT_SEGMENTATION_HANDLE(PolarGridRPhi2)

#include "DDSegmentation/GridPhiEta.h"
IMPLEMENT_SEGMENTATION_HANDLE(GridPhiEta)

#include "DDSegmentation/GridRPhiEta.h"
IMPLEMENT_SEGMENTATION_HANDLE(GridRPhiEta)

#include "DDSegmentation/ProjectiveCylinder.h"
IMPLEMENT_SEGMENTATION_HANDLE(ProjectiveCylinder)

#include "DDSegmentation/MultiSegmentation.h"
IMPLEMENT_SEGMENTATION_HANDLE(MultiSegmentation)
