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
#include "DigiEdm4hepInput.h"
#include "DigiIO.h"

// podio/edm4hep include files
#include <podio/Frame.h>
#include <podio/ROOTFrameReader.h>

#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/CaloHitContributionCollection.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// EDM4HEP event frame wrapper
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class edm4hep_read_frame_t : public DigiInputAction::event_frame   {
    public:
      podio::Frame frame { };
      edm4hep_read_frame_t(podio::Frame&& frm) : frame(std::move(frm)) { }
      const podio::CollectionBase* get(const std::string& nam) const { return frame.get(nam); }
    };
    
    using reader_t = podio::ROOTFrameReader;
    using frame_t  = edm4hep_read_frame_t;

    /// EDM4HEP Digi input reader: Collection descriptor definition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput::collection_t   {
    public:
      int           id;
      std::string   name;
      collection_t(int i, const std::string& n) : id(i), name(n) { }
    };

    /// EDM4HEP Digi input reader: Work item definition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput::work_t   {
    public:
      DigiContext&   context;
      DataSegment&   segment;
      frame_t&       frame;
      descriptor_t&  descriptor;
      podio_coll_t*  collection;
    };

    /// EDM4HEP Digi input reader: Input source definition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput::source_t : public DigiInputAction::input_source   {
    public:
      /// Reference to the reader object
      std::unique_ptr<reader_t>   stream      { };
      /// Collections present in the current file
      std::map<Key, collection_t> collections { };
      /// Input source section
      std::string                 section     { };
      /// Current entry inside the current input source
      uint64_t                    entry       { 0 };

    public:
      /// Initializing constructor
      source_t(const std::string& s, std::unique_ptr<reader_t>&& str) 
	: stream(std::move(str)), section(s)  {
      }
      /// Default destructor
      ~source_t()   {
	if ( stream )    {
	  stream.reset();
	}
      }
      /// Check for end of the event sequence
      bool done() const   {
	auto total = stream->getEntries(section);
	if ( (1+entry) >= total )   {
	  return true;
	}
	return false;
      }
      /// Access the next data frame in the input sequence
      std::shared_ptr<frame_t> next()   {
	auto data = stream->readNextEntry(section);
	if ( data )   {
	  ++entry;
	  return std::make_shared<frame_t>(std::move(data));
	}
	return {};
      }
    };

    /// EDM4HEP Digi input reader: Internal data definition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput::internals_t  {
    public:
      using input_t = std::unique_ptr<source_t>;

      /// Reference to the parent object
      DigiEdm4hepInput* parent { nullptr };
      /// Handle to input source
      input_t           m_source { };
      /// Pointer to current input source
      int               m_curr_input   { INPUT_START };

    public:
      /// Initializing constructor
      internals_t() = default;
      /// Open the next input source from the input list
      std::unique_ptr<source_t> open_source();
      /// Access the next event from the sequence of input files
      std::shared_ptr<frame_t> next();
    };

    /// Open the next input source from the input list
    std::unique_ptr<DigiEdm4hepInput::source_t>
    DigiEdm4hepInput::internals_t::open_source()   {
      const auto& inputs = parent->inputs();
      int len = inputs.size();
      if ( inputs.empty() ) m_curr_input = 0;
      while ( (m_curr_input+1) < len )   {
	const auto& fname = inputs[++m_curr_input];
	auto stream = std::make_unique<reader_t>();
	try  {
	  auto sec = parent->input_section();
	  stream->openFile(fname);
	  auto source = std::make_unique<source_t>(sec, std::move(stream));
	  parent->info("+++ Opened EDM4HEP input file %s.", fname.c_str());
	  parent->onOpenFile(*source);
	  return source;
	}
	catch (const std::runtime_error& e)   {
	  parent->error("OpenInput ++ Failed to open input source %s [%s]", fname.c_str(), e.what());
	}
      }
      parent->except("+++ No open file present. Configuration error?");
      throw std::runtime_error("+++ No open file present");
    }

    /// Access the next event record. If the courrent source is exhausted, open next source
    std::shared_ptr<frame_t> DigiEdm4hepInput::internals_t::next()   {
      if ( !m_source || m_source->done() || parent->fileLimitReached(*m_source) )    {
	m_source = open_source();
	if ( m_source )   {
	  auto frame = m_source->next();
	  if ( frame )   {
	    auto table = frame->frame.getAvailableCollections();
	    int id = 0, mask = parent->input_mask();
	    for( const auto& nam : table )   {
	      parent->info("+++ Collection id: %04X --> '%s'", id, nam.c_str());
	      if ( parent->object_loading_is_enabled(nam) )   {
		Key key(nam, mask);
		m_source->collections.emplace( key, collection_t(id, nam) );
	      }
	      ++id;
	    }
	    parent->onProcessEvent(*m_source, *frame);
	    return frame;
	  }
	  parent->except("+++ No valid frame present in file.");
	}
	parent->except("+++ No open file present. Aborting processing");
      }
      auto frame = m_source->next();
      parent->onProcessEvent(*m_source, *frame);
      return frame;
    }

    /// Initializing constructor
    DigiEdm4hepInput::DigiEdm4hepInput(const DigiKernel& krnl, const std::string& nam)
      : DigiInputAction(krnl, nam)
    {
      internals         = std::make_unique<internals_t>();
      internals->parent = this;
      m_trackerHitType  = typeName(typeid(edm4hep::SimTrackerHitCollection));
      m_caloHitType     = typeName(typeid(edm4hep::SimCalorimeterHitCollection));
      m_particlesType   = typeName(typeid(edm4hep::MCParticleCollection));
      m_evtHeaderType   = typeName(typeid(edm4hep::EventHeaderCollection));
      declareProperty("tracker_hits_type",     m_trackerHitType);
      declareProperty("calorimeter_hits_type", m_caloHitType);
      declareProperty("particles_hits_type",   m_particlesType);
      declareProperty("event_header_type",     m_evtHeaderType);
      m_input_section = "events";
    }

    template <typename HIT_TYPE, typename EDM4HEP_COLLECTION_TYPE>
    void DigiEdm4hepInput::hits_from_edm4hep(DigiContext&                   context,
					     DataSegment&                   segment,
					     Key::mask_type                 mask,
					     const std::string&             nam,
					     const EDM4HEP_COLLECTION_TYPE* collection)   const
    {
      DepositVector out(nam, mask, SegmentEntry::UNKNOWN);
      std::map<CellID, HIT_TYPE> hits;
      std::size_t len = 0;
      const DepositPredicate<EnergyCut> predicate ( { this->epsilon } );
      len = collection->size();
      data_io<edm4hep_input>()._to_digi_if(*collection, hits, predicate);
      data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), hits, out);
      info("%s+++ %-24s Converted %6ld Edm4hep %-14s to %6ld cell deposits",
	   context.event->id(), nam.c_str(), len, collection->getValueTypeName().data(), out.size());
      put_data(segment, Key(out.name, mask), std::move(out));
      if ( m_keep_raw )   {
	put_data(segment, Key(nam+".edm4hep", mask, segment.id), std::move(hits));
      }
    }

    /// Generic conversion function for MC particles
    void DigiEdm4hepInput::parts_from_edm4hep(DigiContext&       context,
					      DataSegment&       segment,
					      int                mask,
					      const std::string& nam,
					      const edm4hep::MCParticleCollection* collection)   const
    {
      ParticleMapping transient(nam, mask);
      data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), *collection, transient);
      debug("%s+++ Converted %ld Edm4hep transient", context.event->id(), transient.size());
      put_data(segment, Key(nam, mask), std::move(transient));
    }

    /// Generic conversion function for event parameter settings
    void DigiEdm4hepInput::params_from_edm4hep(DigiContext&        context,
					       DataSegment&        segment,
					       int                 mask,
					       const std::string&  nam,
					       const podio::Frame& frame,
					       const edm4hep::EventHeaderCollection* collection)   const
    {
      DataParameters parameters(nam, mask);
      data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), *collection, parameters);
      data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), frame.getParameters(), parameters);
      debug("%s+++ Converted %ld Edm4hep transient", context.event->id(), parameters.size());
      put_data(segment, Key(nam, mask), std::move(parameters));
    }

    /// Pre-track action callback
    void DigiEdm4hepInput::execute(DigiContext& context)  const   {
      //  Lock all ROOT based actions. SEGV otherwise.
      std::lock_guard<std::mutex> lock(context.global_io_lock());
      auto& event = context.event;
      auto  frame = internals->next();
      DataSegment& segment = event->get_segment(m_input_segment);

      for( auto& coll : internals->m_source->collections )    {
	const auto& nam = coll.second.name;
	const podio::CollectionBase* collection = frame->get(nam);
	if ( collection )   {
	  work_t work { context, segment, *frame, coll, collection };
	  (*this)(context, work);
	  continue;
	}
	error("%s+++ Failed to load collection %s from edm4hep frame.",
	      event->id(), nam.c_str());
      }
      /// Add frame to segment: Need to keep reference of the input data!
      std::any frm(std::move(frame));
      segment.emplace_any(Key("podio_frame", input_mask()), std::move(frm));
      info("%s+++ Read event ", event->id());
    }

    /// Callback to handle single branch
    void DigiEdm4hepInput::operator()(DigiContext& context, work_t& work)  const  {
      auto& seg  = work.segment;
      auto& nam  = work.descriptor.second.name;
      int   msk  = work.descriptor.first.mask();
      const auto* col = work.collection;
      const auto& typ = col->getTypeName();

      if ( typ == m_caloHitType )
	hits_from_edm4hep<edm4hep::SimCalorimeterHit>(context, seg, msk, nam, static_cast<const edm4hep::SimCalorimeterHitCollection*>(col));
      else if ( typ == m_trackerHitType )
	hits_from_edm4hep<edm4hep::SimTrackerHit>(context, seg, msk, nam, static_cast<const edm4hep::SimTrackerHitCollection*>(col));
      else if ( typ == m_particlesType )
	parts_from_edm4hep(context, seg, msk, nam, static_cast<const edm4hep::MCParticleCollection*>(col));
      else if ( typ == m_evtHeaderType )
	params_from_edm4hep(context, seg, msk, nam, work.frame.frame, static_cast<const edm4hep::EventHeaderCollection*>(col));
      else
	except("Unknown data type encountered in branch: %s", nam.c_str());
    }
  }    // End namespace digi
}      // End namespace dd4hep

/// Factory instantiation
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiEdm4hepInput)
