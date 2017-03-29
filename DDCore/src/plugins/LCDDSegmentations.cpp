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

// Framework includes
#include "DD4hep/objects/SegmentationsInterna.h"
#include "DD4hep/Factories.h"

using namespace DD4hep::Geometry;
using namespace DD4hep::DDSegmentation;

namespace {
  template<typename T> DD4hep::Geometry::SegmentationObject*
  create_segmentation(DD4hep::Geometry::BitField64* decoder)  {
    return new DD4hep::Geometry::SegmentationWrapper<T>(decoder);
  }
}

#include "DDSegmentation/NoSegmentation.h"
DECLARE_SEGMENTATION(NoSegmentation,create_segmentation<DD4hep::DDSegmentation::NoSegmentation>)

#include "DDSegmentation/CartesianGridXY.h"
DECLARE_SEGMENTATION(CartesianGridXY,create_segmentation<DD4hep::DDSegmentation::CartesianGridXY>)

#include "DDSegmentation/CartesianGridXZ.h"
DECLARE_SEGMENTATION(CartesianGridXZ,create_segmentation<DD4hep::DDSegmentation::CartesianGridXZ>)

#include "DDSegmentation/CartesianGridYZ.h"
DECLARE_SEGMENTATION(CartesianGridYZ,create_segmentation<DD4hep::DDSegmentation::CartesianGridYZ>)

#include "DDSegmentation/CartesianGridXYZ.h"
DECLARE_SEGMENTATION(CartesianGridXYZ,create_segmentation<DD4hep::DDSegmentation::CartesianGridXYZ>)

#include "DDSegmentation/TiledLayerGridXY.h"
DECLARE_SEGMENTATION(TiledLayerGridXY,create_segmentation<DD4hep::DDSegmentation::TiledLayerGridXY>)

#include "DDSegmentation/MegatileLayerGridXY.h"
DECLARE_SEGMENTATION(MegatileLayerGridXY,create_segmentation<DD4hep::DDSegmentation::MegatileLayerGridXY>)

#include "DDSegmentation/WaferGridXY.h"
DECLARE_SEGMENTATION(WaferGridXY,create_segmentation<DD4hep::DDSegmentation::WaferGridXY>)

#include "DDSegmentation/PolarGridRPhi.h"
DECLARE_SEGMENTATION(PolarGridRPhi,create_segmentation<DD4hep::DDSegmentation::PolarGridRPhi>)

#include "DDSegmentation/GridPhiEta.h"
DECLARE_SEGMENTATION(GridPhiEta,create_segmentation<DD4hep::DDSegmentation::GridPhiEta>)

#include "DDSegmentation/GridRPhiEta.h"
DECLARE_SEGMENTATION(GridRPhiEta,create_segmentation<DD4hep::DDSegmentation::GridRPhiEta>)

#include "DDSegmentation/PolarGridRPhi2.h"
DECLARE_SEGMENTATION(PolarGridRPhi2,create_segmentation<DD4hep::DDSegmentation::PolarGridRPhi2>)

#include "DDSegmentation/ProjectiveCylinder.h"
DECLARE_SEGMENTATION(ProjectiveCylinder,create_segmentation<DD4hep::DDSegmentation::ProjectiveCylinder>)

#include "DDSegmentation/MultiSegmentation.h"
DECLARE_SEGMENTATION(MultiSegmentation,create_segmentation<DD4hep::DDSegmentation::MultiSegmentation>)
