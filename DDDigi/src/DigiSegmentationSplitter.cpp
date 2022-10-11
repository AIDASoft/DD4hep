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
#include "DD4hep/InstanceCount.h"

#include "DDDigi/DigiData.h"
#include "DDDigi/DigiContext.h"
#include "DDDigi/DigiSegmentationSplitter.h"

using dd4hep::digi::DigiSegmentationSplitter;

class DigiSegmentationSplitter::internals_t   {
public:
  /// Property: Identifier of the input repository
  std::string input_id;
  /// Property: Subdetector to handle
  std::string detector_name;
  /// Property: Input masks in the repository
  std::vector<int> masks;

  /// Flag to check the initialization
  bool inited  { false };

  /// Initializing function: compute values which depend on properties
  void initialize(DigiContext& context)   {
    if ( !this->inited )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( !this->inited )   {
	this->inited = true;
      }
    }
  }
};

/// Standard constructor
dd4hep::digi::DigiSegmentationSplitter::DigiSegmentationSplitter(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->internals = std::make_unique<internals_t>();
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
void dd4hep::digi::DigiSegmentationSplitter::split_segments(DigiEvent& event,
							    DigiEvent::container_map_t& data)  const
{

}

/// Main functional callback
void dd4hep::digi::DigiSegmentationSplitter::execute(DigiContext& context)  const    {
  this->internals->initialize(context);
  auto& data = context.event->get_segment(this->internals->input_id);
  this->split_segments(*context.event, data);
}
