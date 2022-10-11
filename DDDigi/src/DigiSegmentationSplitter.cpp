//==========================================================================
//  AIDA Detector description implementation 
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
#include <DD4hep/Detector.h>
#include <DD4hep/InstanceCount.h>

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSegmentationSplitter.h>

using dd4hep::digi::DigiSegmentationSplitter;

class DigiSegmentationSplitter::internals_t   {
public:
  /// Property: Identifier of the input repository
  std::string input_id;
  /// Property: Subdetector to handle
  std::string detector_name;
  /// Property: Input masks in the repository
  std::vector<int> masks;

  const DigiKernel& kernel;
  Detector&         description;
  DetElement        detector;
  SensitiveDetector sensitive;
  IDDescriptor      iddescriptor;
  DigiSegmentationSplitter* split { nullptr };

  /// Flag to check the initialization
  bool inited  { false };

  internals_t(const DigiKernel& krnl, DigiSegmentationSplitter* s) 
    : kernel(krnl), description(kernel.detectorDescription()), split(s)
  {

  }

  /// Initializing function: compute values which depend on properties
  void initialize(DigiContext& context)   {
    if ( !this->inited )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( !this->inited )   {
	this->inited = true;
	detector = description.detector(detector_name);
	if ( !detector.isValid() )   {
	  split->except("FAILED: Cannot access subdetector %s from the detector description.", detector_name.c_str());
	}
	sensitive = description.sensitiveDetector(detector_name);
	if ( !sensitive.isValid() )   {
	  split->except("FAILED: Cannot access sensitive detector for %s.", detector_name.c_str());
	}
	iddescriptor = sensitive.idSpec();
	if ( !iddescriptor.isValid() )   {
	  split->except("FAILED: Cannot access ID descriptor for detector %s.", detector_name.c_str());
	}
      }
    }
  }
};

/// Standard constructor
dd4hep::digi::DigiSegmentationSplitter::DigiSegmentationSplitter(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->internals = std::make_unique<internals_t>(m_kernel, this);
  declareProperty("input",    this->internals->input_id = "deposits");
  declareProperty("detector", this->internals->detector_name);
  declareProperty("masks",    this->internals->masks);
  InstanceCount::increment(this);
}

/// Default destructor
dd4hep::digi::DigiSegmentationSplitter::~DigiSegmentationSplitter() {
  InstanceCount::decrement(this);
}

/// Split actions according to the segmentation
void dd4hep::digi::DigiSegmentationSplitter::split_segments(DigiEvent& /* event */,
							    DigiEvent::container_map_t& /* data */)  const
{

}

/// Main functional callback
void dd4hep::digi::DigiSegmentationSplitter::execute(DigiContext& context)  const    {
  this->internals->initialize(context);
  auto& data = context.event->get_segment(this->internals->input_id);
  this->split_segments(*context.event, data);
}
