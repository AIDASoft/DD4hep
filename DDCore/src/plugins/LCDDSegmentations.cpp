// $Id$
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
#include "DD4hep/Segmentations.h"
#include "DD4hep/Factories.h"

using namespace DD4hep::Geometry;
using namespace DD4hep::DDSegmentation;

#include "DDSegmentation/CartesianGridXY.h"
static SegmentationObject* create_CartesianGridXY(BitField64* decoder)
{  return new SegmentationImplementation<CartesianGridXY>(decoder);    }
DECLARE_SEGMENTATION(CartesianGridXY,create_CartesianGridXY)

#include "DDSegmentation/CartesianGridXZ.h"
static SegmentationObject* create_CartesianGridXZ(BitField64* decoder)
{  return new SegmentationImplementation<CartesianGridXZ>(decoder);    }
DECLARE_SEGMENTATION(CartesianGridXZ,create_CartesianGridXZ)

#include "DDSegmentation/CartesianGridYZ.h"
static SegmentationObject* create_CartesianGridYZ(BitField64* decoder)
{  return new SegmentationImplementation<CartesianGridYZ>(decoder);    }
DECLARE_SEGMENTATION(CartesianGridYZ,create_CartesianGridYZ)

#include "DDSegmentation/CartesianGridXYZ.h"
static SegmentationObject* create_CartesianGridXYZ(BitField64* decoder)
{  return new SegmentationImplementation<CartesianGridXYZ>(decoder);    }
DECLARE_SEGMENTATION(CartesianGridXYZ,create_CartesianGridXYZ)

#include "DDSegmentation/TiledLayerGridXY.h"
static SegmentationObject* create_TiledLayerGridXY(BitField64* decoder)
{  return new SegmentationImplementation<TiledLayerGridXY>(decoder);    }
DECLARE_SEGMENTATION(TiledLayerGridXY,create_TiledLayerGridXY)

#include "DDSegmentation/MegatileLayerGridXY.h"
static SegmentationObject* create_MegatileLayerGridXY(BitField64* decoder)
{  return new SegmentationImplementation<MegatileLayerGridXY>(decoder);    }
DECLARE_SEGMENTATION(MegatileLayerGridXY,create_MegatileLayerGridXY)

#include "DDSegmentation/WaferGridXY.h"
static SegmentationObject* create_WaferGridXY(BitField64* decoder)
{  return new SegmentationImplementation<WaferGridXY>(decoder);    }
DECLARE_SEGMENTATION(WaferGridXY,create_WaferGridXY)

#include "DDSegmentation/PolarGridRPhi.h"
static SegmentationObject* create_PolarGridRPhi(BitField64* decoder)
{  return new SegmentationImplementation<PolarGridRPhi>(decoder);    }
DECLARE_SEGMENTATION(PolarGridRPhi,create_PolarGridRPhi)

#include "DDSegmentation/PolarGridRPhi2.h"
static SegmentationObject* create_PolarGridRPhi2(BitField64* decoder)
{  return new SegmentationImplementation<PolarGridRPhi2>(decoder);    }
DECLARE_SEGMENTATION(PolarGridRPhi2,create_PolarGridRPhi2)

#include "DDSegmentation/ProjectiveCylinder.h"
static SegmentationObject* create_ProjectiveCylinder(BitField64* decoder)
{  return new SegmentationImplementation<ProjectiveCylinder>(decoder);    }
DECLARE_SEGMENTATION(ProjectiveCylinder,create_ProjectiveCylinder)
