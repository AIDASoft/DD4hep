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
#ifndef DIGI_DIGI2ROOT_H
#define DIGI_DIGI2ROOT_H

// Framework include files
#include <DDDigi/DigiContainerProcessor.h>
#include <DDDigi/DigiData.h>

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
    class Digi2ROOTWriter : public  DigiContainerSequenceAction  {
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
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(Digi2ROOTWriter);
      /// Default destructor
      virtual ~Digi2ROOTWriter();

    public:
      /// Standard constructor
      Digi2ROOTWriter(const kernel_t& kernel, const std::string& nam);

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
     *  This is a typical worker action of the Digi2ROOTWriter
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2ROOTProcessor : public DigiContainerProcessor   {
      friend class Digi2ROOTWriter;

    protected:
      /// Reference to the edm4hep engine
      std::shared_ptr<Digi2ROOTWriter::internals_t> internals;

    public:
      /// Standard constructor
      Digi2ROOTProcessor(const DigiKernel& krnl, const std::string& nam);

      /// Standard destructor
      virtual ~Digi2ROOTProcessor() = default;

      void convert_particles(DigiContext& context, const ParticleMapping& cont)  const;
      void convert_deposits(DigiContext& context, const DepositVector& cont, const predicate_t& predicate)  const;
      void convert_deposits(DigiContext& context, const DepositMapping& cont, const predicate_t& predicate)  const;
      void convert_history(DigiContext& context, const DepositsHistory& cont, work_t& work, const predicate_t& predicate)  const;

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DIGI_DIGI2ROOT_H

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

/// ROOT include files
#include <TClass.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include <vector>

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
    class Digi2ROOTWriter::internals_t {
    public:
      struct BranchWrapper   {
	TBranch* branch = nullptr;
	void*    address = nullptr;
	void (*fcn_clear)(void* addr) = 0;
	void clear()  { this->fcn_clear(this->address); }
	template <typename T> T* get() { return (T*)this->address; }
      };
      Digi2ROOTWriter* m_parent                       { nullptr };
      typedef std::map<std::string, BranchWrapper> Collections;
      typedef std::map<std::string, TTree*> Sections;
      /// Known file sections
      Sections m_sections;
      /// Collections in the event tree
      Collections m_collections;
      /// Reference to the ROOT file to open
      std::unique_ptr<TFile> m_file;
      /// Reference to the event data tree
      TTree* m_tree;
      /// Property: name of the event tree
      std::string m_section;
      /// Property: vector with disabled collections
      std::vector<std::string> m_disabledCollections;

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
      /// Clear collection content: Store is still owner!
      void clearCollections();
      /// Access named collection: throws exception ifd the collection is not present (unlocked!)
      template <typename T> BranchWrapper& get_collection(const T&);
    };

    template <typename T> T* Digi2ROOTWriter::internals_t::register_collection(const std::string& nam, T* coll)   {
      struct _do_clear  {
	static void clear(void* ptr)  {  T* c = (T*)ptr; c->clear();  }
      };
      BranchWrapper bw = { nullptr, (void*)coll, _do_clear::clear };
      m_collections.emplace(nam, bw);
      m_parent->debug("+++ created collection %s <%s>", nam.c_str(), typeName(typeid(T)).c_str());
      return coll;
    }

    /// Create all collections according to the parent setup
    void Digi2ROOTWriter::internals_t::create_collections()    {
      if ( m_collections.empty() )   {
        std::string fname = m_parent->m_output;
        m_file.reset(new TFile(fname.c_str(), "RECREATE", "DDDigi data"));
        for( auto& cont : m_parent->m_containers )   {
          const std::string& nam = cont.first;
          const std::string& typ = cont.second;
          if ( typ == "MCParticles" )
            register_collection(nam, new std::vector<Particle*>());
          else
            register_collection(nam, new std::vector<EnergyDeposit*>());
        }
        m_parent->info("+++ Will save %ld events to %s", num_events, m_parent->m_output.c_str());
      }
    }

    /// Access named collection: throws exception ifd the collection is not present
    template <typename T> 
    Digi2ROOTWriter::internals_t::BranchWrapper& Digi2ROOTWriter::internals_t::get_collection(const T& cont)  {
      auto iter = m_collections.find(cont.name);
      if ( iter == m_collections.end() )    {
        m_parent->except("Error");
      }
      return iter->second;
    }

    /// Clear collection content: Store is still owner!
    void Digi2ROOTWriter::internals_t::clearCollections()   {
      for( auto& coll : m_collections )
	coll.second.clear();
    }

    /// Commit data at end of filling procedure
    void Digi2ROOTWriter::internals_t::commit()   {
      if ( m_file )   {
	for( auto& coll : m_collections )    {
	  coll.second.branch->Write();
	}
        clearCollections();
        return;
      }
      m_parent->except("+++ Failed to write output file. [Stream is not open]");
    }

    /// Commit data to disk and close output stream
    void Digi2ROOTWriter::internals_t::close()   {
      if ( m_file )    {
	TDirectory::TContext ctxt(m_file.get());
	Sections::iterator i = m_sections.find(m_section);
	m_parent->info("+++ Closing ROOT output file %s", m_file->GetName());
	if ( i != m_sections.end() )
	  m_sections.erase(i);
	m_collections.clear();
	m_tree->Write();
	m_file->Close();
	m_tree = nullptr;
      }
      m_file.reset();
    }

    /// Standard constructor
    Digi2ROOTWriter::Digi2ROOTWriter(const DigiKernel& krnl, const std::string& nam)
      : DigiContainerSequenceAction(krnl, nam)
    {
      internals = std::make_shared<internals_t>();
      declareProperty("output",         m_output);
      declareProperty("containers",     m_containers);
      declareProperty("processor_type", m_processor_type = "Digi2ROOTProcessor");
      internals->m_parent = this;
      InstanceCount::increment(this);
    }

    /// Default destructor
    Digi2ROOTWriter::~Digi2ROOTWriter()   {{
        std::lock_guard<std::mutex> lock(m_kernel.global_io_lock());
        internals->close();
      }
      internals.reset();
      InstanceCount::decrement(this);
    }

    /// Initialization callback
    void Digi2ROOTWriter::initialize()   {
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
    void Digi2ROOTWriter::finalize()   {
      internals->close();
    }

    /// Adopt new parallel worker
    void Digi2ROOTWriter::adopt_processor(DigiContainerProcessor* action,
					  const std::string& container)
    {
      std::size_t idx = container.find('/');
      if ( idx != std::string::npos )   {
        std::string nam = container.substr(0, idx);
        std::string typ = container.substr(idx+1);
        auto* act = dynamic_cast<Digi2ROOTProcessor*>(action);
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
    void Digi2ROOTWriter::adopt_processor(DigiContainerProcessor* action, 
					  const std::vector<std::string>& containers)
    {
      DigiContainerSequenceAction::adopt_processor(action, containers);
    }

    /// Main functional callback
    void Digi2ROOTWriter::execute(DigiContext& context)  const    {
      std::lock_guard<std::mutex> lock(context.global_io_lock());
      this->DigiContainerSequenceAction::execute(context);
      this->internals->commit();
      if ( ++internals->event_count == internals->num_events )  {
        internals->close();
      }
    }

    /// Callback to store the run information
    void Digi2ROOTWriter::beginRun()  {
      saveRun();
    }

    /// Callback to store the run information
    void Digi2ROOTWriter::endRun()  {
      // saveRun(run);
    }

    /// Callback to store the Geant4 run information
    void Digi2ROOTWriter::saveRun()  {
      warning("saveRun(): RunHeader not implemented in EDM4hep, nothing written ...");
    }


    /// Standard constructor
    Digi2ROOTProcessor::Digi2ROOTProcessor(const DigiKernel& krnl, const std::string& nam)
      : DigiContainerProcessor(krnl, nam)
    {
    }

    void Digi2ROOTProcessor::convert_particles(DigiContext& ctxt,
					       const ParticleMapping& cont)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec = coll.get<std::vector<const ParticleMapping::value_type*> >();
      vec->reserve(cont.size());
      for( const auto& p : cont )   {
	vec->emplace_back(&p);
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_deposits(DigiContext&          ctxt,
					      const DepositMapping& cont,
					      const predicate_t&    predicate)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec  = coll.get<std::vector<const DepositMapping::value_type*> >();
      vec->reserve(cont.size());
      for ( const auto& depo : cont )   {
	if ( predicate(depo) )   {
	  vec->emplace_back(&depo);
	}
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_deposits(DigiContext&          ctxt,
					      const DepositVector&  cont,
					      const predicate_t&    predicate)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec  = coll.get<std::vector<const DepositVector::value_type*> >();
      vec->reserve(cont.size());
      for ( const auto& depo : cont )   {
	if ( predicate(depo) )   {
	  vec->emplace_back(&depo);
	}
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_history(DigiContext&           ctxt,
					     const DepositsHistory& cont,
					     work_t&                work,
					     const predicate_t&     predicate)  const
    {
      info("%s+++ %-32s Segment: %d Predicate:%s Conversion to edm4hep not implemented!",
           ctxt.event->id(), cont.name.c_str(), int(work.input.segment->id),
           typeName(typeid(predicate)).c_str());
    }

    /// Main functional callback
    void Digi2ROOTProcessor::execute(DigiContext& ctxt, work_t& work, const predicate_t& predicate)  const  {
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
DECLARE_DIGIACTION_NS(dd4hep::digi,Digi2ROOTWriter)
DECLARE_DIGIACTION_NS(dd4hep::digi,Digi2ROOTProcessor)
