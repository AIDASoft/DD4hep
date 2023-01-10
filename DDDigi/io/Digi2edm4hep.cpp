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
#ifndef DIGI_DIGI2EDM4HEP_H
#define DIGI_DIGI2EDM4HEP_H

// Framework include files
#include <DDDigi/DigiContainerProcessor.h>

/// C/C++ include files

namespace edm4hep  {
  class TrackerHitCollection;
  class CalorimeterHitCollection;
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Event action to support edm4hep output format from DDDigi
    /**
     *  Supported output containers types are:
     *  - edm4hep::MCParticles aka "MCParticles"
     *  - edm4hep::CalorimeterHitCollection  aka "CalorimeterHits"
     *  - edm4hep::TrackerHitCollection aka "TracketHits"
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2edm4hepWriter : public  DigiContainerSequenceAction  {
    public:
      class internals_t;

    protected:
      /// Property: Container names to be loaded
      std::string m_output;
      /// Property: Processor type to manage containers
      std::string m_processor_type;
      /// Property: Container / data type mapping
      std::map<std::string, std::string> m_containers  { };
      /// Reference to internals
      std::shared_ptr<internals_t> internals;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(Digi2edm4hepWriter);
      /// Default destructor
      virtual ~Digi2edm4hepWriter();

    public:
      /// Standard constructor
      Digi2edm4hepWriter(const kernel_t& kernel, const std::string& nam);

      /// Initialization callback
      virtual void initialize();

      /// Finalization callback
      virtual void finalize();

      /// Adopt new parallel worker
      virtual void adopt_processor(DigiContainerProcessor* action,
                                   const std::string& container)  override final;

      /// Adopt new parallel worker acting on multiple containers
      virtual void adopt_processor(DigiContainerProcessor* action,
                                   const std::vector<std::string>& containers);

      /// Callback to store the run information
      void beginRun();

      /// Callback to store the run information
      void endRun();

      /// Callback to store the Geant4 run information
      void saveRun();

      /// Main functional callback
      virtual void execute(context_t& context)  const;
    };

    /// Actor to save individual data containers to edm4hep
    /** Actor to save individual data containers to edm4hep
     *
     *  This is a typical worker action of the Digi2edm4hepWriter
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2edm4hepProcessor : public DigiContainerProcessor   {
      friend class Digi2edm4hepWriter;

    protected:
      /// Reference to the edm4hep engine
      std::shared_ptr<Digi2edm4hepWriter::internals_t> internals;
      /// Property: RPhi resolution
      float m_pointResoutionRPhi = 0.004;
      /// Property: Z resolution
      float m_pointResoutionZ = 0.004;
      /// Hit type for hit processor
      int   m_hit_type = 0;

    public:
      /// Standard constructor
      Digi2edm4hepProcessor(const DigiKernel& krnl, const std::string& nam);

      /// Standard destructor
      virtual ~Digi2edm4hepProcessor() = default;

      template <typename T> void
      convert_depos(const T& cont, const predicate_t& predicate, edm4hep::TrackerHitCollection* collection)  const;

      template <typename T> void
      convert_depos(const T& cont, const predicate_t& predicate, edm4hep::CalorimeterHitCollection* collection)  const;

      template <typename T> void
      convert_deposits(DigiContext& context, const T& cont, const predicate_t& predicate)  const;

      void convert_history(DigiContext& context, const DepositsHistory& cont, work_t& work, const predicate_t& predicate)  const;

      void convert_particles(DigiContext& context, const ParticleMapping& cont)  const;

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DIGI_DIGI2EDM4HEP_H

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

/// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiPlugins.h>
#include <DDDigi/DigiKernel.h>
#include "DigiIO.h"

/// edm4hep include files
#include <podio/EventStore.h>
#include <podio/ROOTWriter.h>
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/TrackerHitCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/CaloHitContributionCollection.h>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Helper class to create output in edm4hep format
    /** Helper class to create output in edm4hep format
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2edm4hepWriter::internals_t {
    public:
      Digi2edm4hepWriter* m_parent                    { nullptr };
      /// Reference to podio store
      std::unique_ptr<podio::EventStore>  m_store     { };
      /// Reference to podio writer
      std::unique_ptr<podio::ROOTWriter>  m_file      { };
      /// edm4hep event header collection
      edm4hep::EventHeaderCollection*     m_header    { nullptr };
      /// MC particle collection
      edm4hep::MCParticleCollection*      m_particles { nullptr };
      /// Collection of all edm4hep object collections
      std::map<std::string, podio::CollectionBase*> m_collections;

      /// Total numbe rof events to be processed
      long num_events  { -1 };
      /// Running event counter
      long event_count {  0 };

    private:
      /// Helper to register single collection
      template <typename T> T* register_collection(const std::string& name, T* collection);

    public:
      /// Default constructor
      internals_t() = default;
      /// Default destructor
      ~internals_t() = default;

      /// Commit data at end of filling procedure
      void commit();
      /// Commit data to disk and close output stream
      void close();

      /// Create all collections according to the parent setup (locked)
      void create_collections();
      /// Access named collection: throws exception ifd the collection is not present (unlocked!)
      template <typename T> podio::CollectionBase* get_collection(const T&);
    };

    template <typename T> T* Digi2edm4hepWriter::internals_t::register_collection(const std::string& nam, T* coll)   {
      m_collections.emplace(nam, coll);
      m_store->registerCollection(nam, coll);
      m_file->registerForWrite(nam);
      m_parent->debug("+++ created collection %s <%s>", nam.c_str(), coll->getTypeName().c_str());
      return coll;
    }

    /// Create all collections according to the parent setup
    void Digi2edm4hepWriter::internals_t::create_collections()    {
      if ( nullptr == m_header )   {
        std::string fname = m_parent->m_output;
        m_store  = std::make_unique<podio::EventStore>();
        m_file   = std::make_unique<podio::ROOTWriter>(fname, m_store.get());
        m_header = register_collection("EventHeader", new edm4hep::EventHeaderCollection());
        for( auto& cont : m_parent->m_containers )   {
          const std::string& nam = cont.first;
          const std::string& typ = cont.second;
          if ( typ == "MCParticles" )   {
            m_particles = register_collection(nam, new edm4hep::MCParticleCollection());
          }
          else if ( typ == "TrackerHits" )   {
            register_collection(nam, new edm4hep::TrackerHitCollection());
          }
          else if ( typ == "CalorimeterHits" )   {
            register_collection(nam, new edm4hep::CalorimeterHitCollection());
          }
        }
        m_parent->info("+++ Will save %ld events to %s", num_events, m_parent->m_output.c_str());
      }
    }

    /// Access named collection: throws exception ifd the collection is not present
    template <typename T> 
    podio::CollectionBase* Digi2edm4hepWriter::internals_t::get_collection(const T& cont)  {
      auto iter = m_collections.find(cont.name);
      if ( iter == m_collections.end() )    {
        m_parent->except("Error");
      }
      return iter->second;
    }

    /// Commit data at end of filling procedure
    void Digi2edm4hepWriter::internals_t::commit()   {
      if ( m_file )   {
        m_file->writeEvent();
        m_store->clearCollections();
        return;
      }
      m_parent->except("+++ Failed to write output file. [Stream is not open]");
    }

    /// Commit data to disk and close output stream
    void Digi2edm4hepWriter::internals_t::close()   {
      if ( m_file )   {
        m_file->finish();
      }
      m_file.reset();
      m_store.reset();
    }

    /// Standard constructor
    Digi2edm4hepWriter::Digi2edm4hepWriter(const DigiKernel& krnl, const std::string& nam)
      : DigiContainerSequenceAction(krnl, nam)
    {
      internals = std::make_shared<internals_t>();
      declareProperty("output",         m_output);
      declareProperty("containers",     m_containers);
      declareProperty("processor_type", m_processor_type = "Digi2edm4hepProcessor");
      internals->m_parent = this;
      InstanceCount::increment(this);
    }

    /// Default destructor
    Digi2edm4hepWriter::~Digi2edm4hepWriter()   {{
        std::lock_guard<std::mutex> lock(m_kernel.global_io_lock());
        internals->close();
      }
      internals.reset();
      InstanceCount::decrement(this);
    }

    /// Initialization callback
    void Digi2edm4hepWriter::initialize()   {
      if ( m_containers.empty() )   {
        warning("+++ No input containers given for attenuation action -- no action taken");
        return;
      }
      internals->num_events = m_kernel.property("numEvents").value<long>();
      for ( const auto& c : m_containers )   {
        Key key(c.first, 0, 0);
        auto it = m_registered_processors.find(key);
        if ( it == m_registered_processors.end() )   {
          std::string nam = name() + ".E4H." + c.first;
          auto* conv = createAction<DigiContainerProcessor>(m_processor_type, m_kernel, nam);
          if ( !conv )   {
            except("+++ Failed to create edm4hep processor: %s of type: %s",
                   nam.c_str(), m_processor_type.c_str());
          }
          conv->property("OutputLevel").set(int(outputLevel()));
          adopt_processor(conv, c.first);
          conv->release(); // Release processor **after** adoption.
        }
      }
      std::lock_guard<std::mutex> lock(m_kernel.global_io_lock());
      m_parallel = false;
      internals->create_collections();
      this->DigiContainerSequenceAction::initialize();
    }

    /// Finalization callback
    void Digi2edm4hepWriter::finalize()   {
      internals->close();
    }

    /// Adopt new parallel worker
    void Digi2edm4hepWriter::adopt_processor(DigiContainerProcessor* action,
                                             const std::string& container)
    {
      std::size_t idx = container.find('/');
      if ( idx != std::string::npos )   {
        std::string nam = container.substr(0, idx);
        std::string typ = container.substr(idx+1);
        auto* act = dynamic_cast<Digi2edm4hepProcessor*>(action);
        if ( act )   { // This is not nice! Need to think about something better.
          act->internals = this->internals;
        }
        this->DigiContainerSequenceAction::adopt_processor(action, nam);
        m_containers.emplace(nam, typ);
        return;
      }
      except("+++ Invalid container specification: %s. %s",
             container.c_str(), "Specify container as tuple: \"<name>/<type>\" !");
    }

    /// Adopt new parallel worker acting on multiple containers
    void Digi2edm4hepWriter::adopt_processor(DigiContainerProcessor* action, 
                                             const std::vector<std::string>& containers)
    {
      DigiContainerSequenceAction::adopt_processor(action, containers);
    }

    /// Main functional callback
    void Digi2edm4hepWriter::execute(DigiContext& context)  const    {
      std::lock_guard<std::mutex> lock(context.global_io_lock());
      this->DigiContainerSequenceAction::execute(context);
      this->internals->commit();
      if ( ++internals->event_count == internals->num_events )  {
        internals->close();
      }
    }

    /// Callback to store the run information
    void Digi2edm4hepWriter::beginRun()  {
      saveRun();
    }

    /// Callback to store the run information
    void Digi2edm4hepWriter::endRun()  {
      // saveRun(run);
    }

    /// Callback to store the Geant4 run information
    void Digi2edm4hepWriter::saveRun()  {
      warning("saveRun(): RunHeader not implemented in EDM4hep, nothing written ...");
    }


    /// Standard constructor
    Digi2edm4hepProcessor::Digi2edm4hepProcessor(const DigiKernel& krnl, const std::string& nam)
      : DigiContainerProcessor(krnl, nam)
    {
      declareProperty("point_resolution_RPhi", m_pointResoutionRPhi);
      declareProperty("point_resolution_Z",    m_pointResoutionZ);
      declareProperty("hit_type",              m_hit_type = 0);
    }

    void Digi2edm4hepProcessor::convert_particles(DigiContext& ctxt,
                                                  const ParticleMapping& cont)  const
    {
      auto* coll = internals->m_particles;
      std::size_t start = coll->size();
      digi_io()._to_edm4hep(cont, coll);
      std::size_t end = internals->m_particles->size();
      info("%s+++ %-24s added %6ld/%6ld entries from mask: %04X to %s",
           ctxt.event->id(), cont.name.c_str(), end-start, end, cont.key.mask(),
           coll->getTypeName().c_str());
    }

    template <typename T> void
    Digi2edm4hepProcessor::convert_depos(const T& cont,
                                         const predicate_t& predicate,
                                         edm4hep::TrackerHitCollection* collection)  const
    {
      std::array<float,6> covMat = {0., 0., m_pointResoutionRPhi*m_pointResoutionRPhi, 
        0., 0., m_pointResoutionZ*m_pointResoutionZ
      };
      for ( const auto& depo : cont )   {
        if ( predicate(depo) )   {
          digi_io::_to_edm4hep(depo, covMat, m_hit_type /* edm4hep::SIMTRACKERHIT */, collection);
        }
      }
    }

    template <typename T> void
    Digi2edm4hepProcessor::convert_depos(const T& cont,
                                         const predicate_t& predicate,
                                         edm4hep::CalorimeterHitCollection* collection)  const
    {
      for ( const auto& depo : cont )   {
        if ( predicate(depo) )   {
          digi_io::_to_edm4hep(depo, m_hit_type /* edm4hep::SIMCALORIMETERHIT */, collection);
        }
      }
    }

    template <typename T> void
    Digi2edm4hepProcessor::convert_deposits(DigiContext&       ctxt,
                                            const T&           cont,
                                            const predicate_t& predicate)  const
    {
      podio::CollectionBase* coll = internals->get_collection(cont);
      std::size_t start = coll->size();
      if ( !cont.empty() )   {
        switch(cont.data_type)    {
        case SegmentEntry::TRACKER_HITS:
          convert_depos(cont, predicate, static_cast<edm4hep::TrackerHitCollection*>(coll));
          break;
        case SegmentEntry::CALORIMETER_HITS:
          convert_depos(cont, predicate, static_cast<edm4hep::CalorimeterHitCollection*>(coll));
          break;
        default:
          except("Error: Unknown energy deposit type: %d", int(cont.data_type));
          break;
        }
      }
      std::size_t end = coll->size();
      info("%s+++ %-24s added %6ld/%6ld entries from mask: %04X to %s",
           ctxt.event->id(), cont.name.c_str(), end-start, end, cont.key.mask(),
           coll->getTypeName().c_str());
    }

    void Digi2edm4hepProcessor::convert_history(DigiContext&           ctxt,
                                                const DepositsHistory& cont,
                                                work_t&                work,
                                                const predicate_t&     predicate)  const
    {
      info("%s+++ %-32s Segment: %d Predicate:%s Conversion to edm4hep not implemented!",
           ctxt.event->id(), cont.name.c_str(), int(work.input.segment->id),
           typeName(typeid(predicate)).c_str());
    }

    /// Main functional callback
    void Digi2edm4hepProcessor::execute(DigiContext& ctxt, work_t& work, const predicate_t& predicate)  const  {
      if ( const auto* p = work.get_input<ParticleMapping>() )
        convert_particles(ctxt, *p);
      else if ( const auto* m = work.get_input<DepositMapping>() )
        convert_deposits(ctxt, *m, predicate);
      else if ( const auto* v = work.get_input<DepositVector>() )
        convert_deposits(ctxt, *v, predicate);
      else if ( const auto* h = work.get_input<DepositsHistory>() )
        convert_history(ctxt, *h, work, predicate);
      else
        except("Request to handle unknown data type: %s", work.input_type_name().c_str());
    }

  }    // End namespace digi
}      // End namespace dd4hep

/// Factory instantiation:
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,Digi2edm4hepWriter)
DECLARE_DIGIACTION_NS(dd4hep::digi,Digi2edm4hepProcessor)
