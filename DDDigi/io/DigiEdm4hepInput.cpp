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
#include <DDDigi/DigiInputAction.h>
#include <DDDigi/DigiData.h>
#include "DigiIO.h"

// C/C++ include files

/// Forward declarations
namespace podio {  class CollectionBase; }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {
    
    /// EDM4HEP Digi input reader
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput : public DigiInputAction    {
    public:
      static constexpr double epsilon = std::numeric_limits<double>::epsilon();

      /// Forward declarations
      class internals_t;
      class inputsource_t;
      class collection_t;
      class work_t;
      using podio_coll_t = const podio::CollectionBase;
      using descriptor_t = std::pair<const Key, collection_t>;

      /// Reference to internal data
      std::unique_ptr<internals_t> internals;
      /// Type of tracker hit container
      std::string m_trackerHitType { };
      /// Type of calorimenter hit container
      std::string m_caloHitType    { };
      /// Type of particles container
      std::string m_particlesType  { };
      /// Property to generate extra history records
      bool        m_keep_raw       { true };

    public:
      /// Initializing constructor
      DigiEdm4hepInput(const DigiKernel& krnl, const std::string& nam);

      template <typename T, typename COLL>
      void from_edm4hep(DigiContext&       context,
			DataSegment&       segment,
			Key::mask_type     mask,
			const std::string& nam,
			const COLL*        collection)   const
      {
	DepositVector out(nam, mask, SegmentEntry::UNKNOWN);
	std::map<CellID, T> hits;
	std::size_t len = 0;
	const DepositPredicate<EnergyCut> predicate ( { this->epsilon } );
	len = collection->size();
	data_io<edm4hep_input>()._to_digi_if(*collection, hits, predicate);
	data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), hits, out);
	info("%s+++ %-24s Converted %6ld Edm4hep %-14s to %6ld cell deposits",
	     context.event->id(), nam.c_str(), len, collection->getValueTypeName().c_str(), out.size());
	put_data(segment, Key(out.name, mask), out);
	if ( m_keep_raw )   {
	  put_data(segment, Key(nam+".edm4hep", mask, segment.id), hits);
	}
      }

      template <typename COLL>
      void from_edm4hep(DigiContext&       context,
			DataSegment&       segment,
			int                mask,
			const std::string& nam,
			const COLL*        collection)   const
      {
	ParticleMapping particles(nam, mask);
	data_io<edm4hep_input>()._to_digi(Key(nam, segment.id, mask), *collection, particles);
	debug("%s+++ Converted %ld Edm4hep particles", context.event->id(), particles.size());
	put_data(segment, Key(nam, mask), particles);
      }

      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const;
      /// Event action callback
      virtual void execute(DigiContext& context)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include "DigiFrame.h"
#include <podio/ROOTFrameReader.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/CaloHitContributionCollection.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    using reader_t = podio::ROOTFrameReader;
    using frame_t  = podio::Frame;

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
      DigiContext&  context;
      descriptor_t& descriptor;
      DataSegment&  segment;
      podio_coll_t* collection;
    };

    /// EDM4HEP Digi input reader: Input source definition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepInput::inputsource_t   {
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
      inputsource_t(const std::string& s, std::unique_ptr<reader_t>&& str) 
	: stream(std::move(str)), section(s)  {
      }
      /// Default destructor
      ~inputsource_t()   {
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
      using input_t = std::unique_ptr<inputsource_t>;

      /// Reference to the parent object
      DigiEdm4hepInput* m_parent { nullptr };
      /// Handle to input source
      input_t           m_source { };
      /// Pointer to current input source
      int               m_curr_input   { INPUT_START };
      std::size_t       m_curr_event   { 0 };

    public:
      /// Initializing constructor
      internals_t(DigiEdm4hepInput* parent);
      /// Open new input stream
      std::unique_ptr<inputsource_t> open_next_data_source();
      /// Access the next event from the sequence of input files
      std::shared_ptr<frame_t> next();
    };

    /// Initializing constructor
    DigiEdm4hepInput::internals_t::internals_t(DigiEdm4hepInput* parent)
      : m_parent(parent)
    {
    }

    std::unique_ptr<DigiEdm4hepInput::inputsource_t>
    DigiEdm4hepInput::internals_t::open_next_data_source()   {
      const auto& inputs = m_parent->inputs();
      int len = inputs.size();
      if ( inputs.empty() ) m_curr_input = 0;
      while ( (m_curr_input+1) < len )   {
	const auto& fname = inputs[++m_curr_input];
	auto stream = std::make_unique<reader_t>();
	try  {
	  auto sec = m_parent->input_section();
	  stream->openFile(fname);
	  auto source = std::make_unique<inputsource_t>(sec, std::move(stream));
	  m_parent->info("+++ Opened EDM4HEP input file %s.", fname.c_str());
	  return source;
	}
	catch (const std::runtime_error& e)   {
	  m_parent->error("OpenInput ++ Failed to open input source %s [%s]", fname.c_str(), e.what());
	}
      }
      m_parent->except("+++ No open file present. Configuration error?");
      throw std::runtime_error("+++ No open file present");
    }

    std::shared_ptr<frame_t> DigiEdm4hepInput::internals_t::next()   {
      if ( !m_source || m_source->done() )    {
	int mask = m_parent->input_mask();
	m_source = open_next_data_source();
	if ( m_source )   {
	  auto frame = m_source->next();
	  if ( frame )   {
	    auto table = frame->getIDTable();
	    const auto& ids = table.ids();
	    for( int id : ids )   {
	      std::string nam = table.name(id);
	      m_parent->info("+++ Collection id: %04X --> '%s'", id, nam.c_str());
	      if ( m_parent->object_loading_is_enabled(nam) )   {
		Key key(nam, mask);
		m_source->collections.emplace( key, collection_t(id, nam) );
	      }
	    }
	    return frame;
	  }
	  m_parent->except("+++ No valid frame present in file.");
	}
	m_parent->except("+++ No open file present. Aborting processing");
      }
      return m_source->next();
    }

    /// Initializing constructor
    DigiEdm4hepInput::DigiEdm4hepInput(const DigiKernel& krnl, const std::string& nam)
      : DigiInputAction(krnl, nam)
    {
      internals        = std::make_unique<internals_t>(this);
      m_trackerHitType = typeName(typeid(edm4hep::SimTrackerHitCollection));
      m_caloHitType    = typeName(typeid(edm4hep::SimCalorimeterHitCollection));
      m_particlesType  = typeName(typeid(edm4hep::MCParticleCollection));
      declareProperty("keep_raw",              m_keep_raw);
      declareProperty("tracker_hits_type",     m_trackerHitType);
      declareProperty("calorimeter_hits_type", m_caloHitType);
      declareProperty("particles_hits_type",   m_particlesType);
      m_input_section = "events";
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
	  work_t work { context, coll, segment, collection };
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
	from_edm4hep<edm4hep::SimCalorimeterHit>(context, seg, msk, nam, static_cast<const edm4hep::SimCalorimeterHitCollection*>(col));
      else if ( typ == m_trackerHitType )
	from_edm4hep<edm4hep::SimTrackerHit>(context, seg, msk, nam, static_cast<const edm4hep::SimTrackerHitCollection*>(col));
      else if ( typ == m_particlesType )
	from_edm4hep(context, seg, msk, nam, static_cast<const edm4hep::MCParticleCollection*>(col));
      else
	except("Unknown data type encountered in branch: %s", nam.c_str());
    }

  }    // End namespace digi
}      // End namespace dd4hep

/// Factory instantiation
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiEdm4hepInput)
