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
#include <DD4hep/Plugins.h>
#include <DD4hep/Detector.h>
#include <DD4hep/InstanceCount.h>

#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSegmentationSplitter.h>

#include <sstream>

using namespace dd4hep::digi;

class DigiSegmentAction::internals_t  {
public:
  const DigiSegmentContext* split { nullptr };
  const DepositMapping*     input { nullptr };
};

class DigiSegmentationSplitter::internals_t   {
public:
  /// Property: Identifier of the input repository
  std::string input_id;
  /// Property: Subdetector to handle
  std::string detector_name;
  /// Property: Factory name of the processor type
  std::string split_by;
  /// Property: Split element of the ID descriptor
  std::string processor_type;
  /// Property: Flag if processors should be shared
  bool        share_processor   { true };
  /// Property: Input mask in the repository
  int         input_mask;

  std::vector<Key::key_type> data_keys;

  DigiSegmentContext splitter;

  const DigiKernel& kernel;
  Detector&         description;
  DetElement        detector;
  SensitiveDetector sensitive;
  IDDescriptor      iddescriptor;
  DigiSegmentationSplitter* split { nullptr };

  std::map<uint64_t, DigiSegmentContext> split_processors;

  /// Flag to check the initialization
  bool inited  { false };

  internals_t(const DigiKernel& krnl, DigiSegmentationSplitter* s) 
    : kernel(krnl), description(kernel.detectorDescription()), split(s)
  {
  }

  void scan_detector(DetElement de, VolumeID vid, VolumeID mask)   {
    const auto& new_ids = de.placement().volIDs();
    VolumeID    new_vid = vid;
    VolumeID    new_msk = mask;
    if ( !new_ids.empty() )   {
      new_vid |= this->iddescriptor.encode(new_ids);
      new_msk |= this->iddescriptor.get_mask(new_ids);
      for (const auto& id : new_ids)   {
	if ( id.first == this->split_by )   {
	  DigiSegmentContext context = this->splitter;
	  context.detector = de;
	  context.id = id.second;
	  this->split_processors.emplace(new_vid, std::move(context));
	  return;
	}
      }
    }
    for ( const auto& c : de.children() )
      scan_detector(c.second, new_vid, new_msk);
  }

  void init_basics()   {
    const char* det = this->detector_name.c_str();
    this->detector = this->description.detector(det);
    if ( !this->detector.isValid() )   {
      split->except("FAILED: Cannot access subdetector %s from the detector description.", det);
    }
    this->sensitive = this->description.sensitiveDetector(det);
    if ( !sensitive.isValid() )   {
      split->except("FAILED: Cannot access sensitive detector for %s.", det);
    }
    this->iddescriptor = this->sensitive.idSpec();
    if ( !this->iddescriptor.isValid() )   {
      split->except("FAILED: Cannot access ID descriptor for detector %s.", det);
    }
    Readout rd = this->sensitive.readout();
    auto colls = rd.collectionNames();
    if ( colls.empty() ) colls.emplace_back(rd.name());
    for( const auto& c : colls )   {
      this->data_keys.emplace_back(Key(this->input_mask, c).key);
    }
  }

  void init_splitting()   {
    const char* det = this->detector_name.c_str();
    /// Setup the splitter
    this->splitter.cell_mask = ~0x0UL;
    this->splitter.detector  = this->detector;
    this->splitter.idspec    = this->iddescriptor;
    for(const auto& f : this->iddescriptor.fields())   {
      const BitFieldElement* e = f.second;
      if ( e->name() == this->split_by )   {
	this->splitter.field      = e;
	this->splitter.split_mask = e->mask();
	this->splitter.width      = e->width();
	this->splitter.offset     = e->offset();
	this->splitter.max_split  = 1 << e->width();
      }
      else   {
	this->splitter.det_mask |= e->mask();
      }
      this->splitter.cell_mask = (splitter.cell_mask << e->width());
      split->info("%-24s %-8s [%3d,%7d] width:%2d offset:%2d mask:%016lX Split:%016lX Det:%016lX Cells:%016lX",
		  iddescriptor.name(), e->name().c_str(),
		  e->minValue(), e->maxValue(), e->width(), e->offset(),
		  e->mask(), this->splitter.split_mask, this->splitter.det_mask, this->splitter.cell_mask);
      if ( this->splitter.field ) break;
    }
    if ( !this->splitter.field )   {
      split->except("FAILED: The ID descriptor for detector %s has no field %s.",
		    det, split_by.c_str());
    }
    this->split_processors.clear();
    const auto& ids = this->detector.placement().volIDs();
    VolumeID    vid = this->iddescriptor.encode(ids);
    VolumeID    msk = this->iddescriptor.get_mask(ids);
    this->scan_detector(this->detector, vid, msk);
    split->info("%-24s has %ld parallel entries in when splitting by \"%s\"",
		det, this->split_processors.size(), this->split_by.c_str());
    std::stringstream str;
    for( auto id : this->split_processors )
      str << std::setw(16) << std::hex << std::setfill('0') << id.first << " ";
    split->info("%-24s --> Parallel Vid: %s", det, str.str().c_str());
    str.str("");
    for( auto id : this->split_processors )
      str << std::setw(16) << std::dec << std::setfill(' ') << this->splitter.split_id(id.first) << " ";
    split->info("%-24s --> Parallel ID:  %s", det, str.str().c_str());
  }

  /// Initializing function: compute values which depend on properties
  void initialize(DigiContext& context)   {
    if ( !this->inited )   {
      std::lock_guard<std::mutex> lock(context.initializer_lock());
      if ( !this->inited )   {
	const std::string det = this->detector_name;
	this->inited = true;
	this->init_basics();
	this->init_splitting();
	this->split->init_processors();
	split->info("+++ Detector splitter is now fully initialized!");
      }
    }
  }
};

/// Standard constructor
DigiSegmentationSplitter::DigiSegmentationSplitter(const DigiKernel& krnl, const std::string& nam)
  : DigiActionSequence(krnl, nam)
{
  this->internals = std::make_unique<internals_t>(m_kernel, this);
  declareProperty("input",           this->internals->input_id = "deposits");
  declareProperty("detector",        this->internals->detector_name);
  declareProperty("mask",            this->internals->input_mask);
  declareProperty("split_by",        this->internals->split_by);
  declareProperty("processor_type",  this->internals->processor_type);
  declareProperty("share_processor", this->internals->share_processor = true);
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentationSplitter::~DigiSegmentationSplitter() {
  InstanceCount::decrement(this);
}

void DigiSegmentationSplitter::init_processors()   {
  char text[256];
  std::size_t count = 0;
  auto& imp   = *this->internals;
  std::string typ = imp.processor_type;
  DigiSegmentAction* proc = nullptr;
  /// Create the processors:
  for( const auto& p : imp.split_processors )   {
    auto& split = p.second;
    ::snprintf(text, sizeof(text), "_%05X", split.split_id(p.first));
    std::string nam = this->name() + text;
    proc = PluginService::Create<DigiSegmentAction*>(typ, &m_kernel, nam);
    proc->internals = std::make_unique<DigiSegmentAction::internals_t>();
    proc->internals->split = &split;
    this->DigiActionSequence::adopt(proc);
    ++count;
  }
}

/// Main functional callback
void DigiSegmentationSplitter::execute(DigiContext& context)  const    {
  auto& imp   = *this->internals;
  auto& event = *context.event;
  auto& input = event.get_segment(imp.input_id);

  this->internals->initialize(context);
  for( auto k : imp.data_keys )   {
    auto* hits = input.pointer<DepositMapping>(k);
    if ( hits )    {
      /// prepare processors for execution
      for ( auto* a : this->m_actors )   {
	auto* proc  = (DigiSegmentAction*)a;
	proc->internals->input = hits;
      }
      /// Now submit them
      this->DigiActionSequence::execute(context);
    }
  }
}

/// Standard constructor
DigiSegmentAction::DigiSegmentAction(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  this->internals = std::make_unique<internals_t>();
  InstanceCount::increment(this);
}

/// Default destructor
DigiSegmentAction::~DigiSegmentAction() {
  InstanceCount::decrement(this);
}

void DigiSegmentAction::execute(DigiContext& context)  const  {
  auto& input = *this->internals->input;
  auto& split = *this->internals->split;
  auto ret = this->handleSegment(context, split, input);
  if ( !ret.empty() )    {
    //auto& segment = context.event->get_segment();
  }
}

/// Main functional callback
std::map<Key::key_type, std::any> 
DigiSegmentAction::handleSegment(DigiContext&              context,
				 const DigiSegmentContext& segment,
				 const DepositMapping&     deposits)  const   {
  for( const auto& depo : deposits )   {
    if ( segment.split_id(depo.first) == segment.id )   {
      auto cell = depo.first;
      const auto& d = depo.second;
      info("%s[%s] %s-id: %d [processor:%d] Cell: %016lX mask: %016lX  hist:%4ld entries deposit: %f", 
	   context.event->id(), segment.idspec.name(), segment.cname(), 
	   segment.split_id(cell), segment.id, cell, segment.split_mask, d.history.size(), d.deposit);
    }
  }
  return {};
}
