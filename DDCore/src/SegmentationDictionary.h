//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  RootDictionary.h
//
//
//  M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDCORE_ROOTDICTIONARY_H
#define DD4HEP_DDCORE_ROOTDICTIONARY_H

// Framework include files
#define __HAVE_DDSEGMENTATION__
// -------------------------------------------------------------------------
#ifdef __HAVE_DDSEGMENTATION__
#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/NoSegmentation.h"
#include "DDSegmentation/CartesianGrid.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "DDSegmentation/CartesianGridYZ.h"
#include "DDSegmentation/CartesianStripX.h"
#include "DDSegmentation/CartesianStripY.h"
#include "DDSegmentation/CartesianStripZ.h"
#include "DDSegmentation/CylindricalSegmentation.h"
#include "DDSegmentation/GridPhiEta.h"
#include "DDSegmentation/GridRPhiEta.h"
#include "DDSegmentation/MegatileLayerGridXY.h"
#include "DDSegmentation/MultiSegmentation.h"
#include "DDSegmentation/NoSegmentation.h"
#include "DDSegmentation/PolarGrid.h"
#include "DDSegmentation/PolarGridRPhi2.h"
#include "DDSegmentation/PolarGridRPhi.h"
#include "DDSegmentation/ProjectiveCylinder.h"

#include "DDSegmentation/SegmentationParameter.h"
#include "DDSegmentation/TiledLayerGridXY.h"
#include "DDSegmentation/TiledLayerSegmentation.h"
#include "DDSegmentation/WaferGridXY.h"
typedef dd4hep::DDSegmentation::VolumeID VolumeID;
typedef dd4hep::DDSegmentation::CellID CellID;

// -------------------------------------------------------------------------
// DDSegmentation dictionaries
#ifdef DD4HEP_DICTIONARY_MODE
#pragma link C++ class dd4hep::DDSegmentation::SegmentationParameter+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<int>+;

#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<float>+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<double>+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<string>+;
#pragma link C++ class map<string,dd4hep::DDSegmentation::TypedSegmentationParameter<string>* >+;
#pragma link C++ class map<string,dd4hep::DDSegmentation::TypedSegmentationParameter<double>* >+;
#pragma link C++ class map<string,dd4hep::DDSegmentation::TypedSegmentationParameter<float>* >+;

/// Severe problem due to template specialization!
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<int> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<float> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<double> >+;
#pragma link C++ class dd4hep::DDSegmentation::TypedSegmentationParameter<vector<string> >+;

#pragma link C++ class dd4hep::DDSegmentation::Segmentation+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGrid+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXY+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXYZ+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridXZ+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianGridYZ+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianStripX+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianStripY+;
#pragma link C++ class dd4hep::DDSegmentation::CartesianStripZ+;
#pragma link C++ class dd4hep::DDSegmentation::CylindricalSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::GridPhiEta+;
#pragma link C++ class dd4hep::DDSegmentation::GridRPhiEta+;
#pragma link C++ class dd4hep::DDSegmentation::MegatileLayerGridXY+;
#pragma link C++ class dd4hep::DDSegmentation::MultiSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::NoSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::PolarGrid+;
#pragma link C++ class dd4hep::DDSegmentation::PolarGridRPhi2+;
#pragma link C++ class dd4hep::DDSegmentation::PolarGridRPhi+;
#pragma link C++ class dd4hep::DDSegmentation::ProjectiveCylinder+;
#pragma link C++ class dd4hep::DDSegmentation::TiledLayerGridXY+;
#pragma link C++ class dd4hep::DDSegmentation::TiledLayerSegmentation+;
#pragma link C++ class dd4hep::DDSegmentation::WaferGridXY+;

#pragma link C++ class dd4hep::DDSegmentation::BitFieldElement+;
#pragma link C++ class dd4hep::DDSegmentation::BitFieldCoder+;

#endif  // __CINT__
#endif  // __HAVE_DDSEGMENTATION__

#endif  /* DD4HEP_DDCORE_ROOTDICTIONARY_H  */
