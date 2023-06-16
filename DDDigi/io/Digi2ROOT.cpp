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
#include <DDDigi/DigiOutputAction.h>
#include <DDDigi/DigiData.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Event action to support edm4hep output format from DDDigi
    /**
     *  Supported output containers types are:
     *  - MCParticles aka "MCParticles"
     *  - EnergyDeposits
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2ROOTWriter : public  DigiOutputAction  {
    public:
      class internals_t;

    protected:
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
      virtual void initialize()  override;
      /// Check for valid output stream
      virtual bool have_output()  const  override final;
      /// Open new output stream
      virtual void open_output() const  override final;
      /// Close possible open stream
      virtual void close_output()  const  override final;
      /// Commit event data to output stream
      virtual void commit_output() const  override final;
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

      void convert_particles(DigiContext& context, ParticleMapping& cont)  const;
      void convert_deposits(DigiContext& context, DepositVector& cont, const predicate_t& predicate)  const;
      void convert_deposits(DigiContext& context, DepositMapping& cont, const predicate_t& predicate)  const;
      void convert_history(DigiContext& context, DepositsHistory& cont, work_t& work, const predicate_t& predicate)  const;

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
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TClass.h>
#include <TBranch.h>

#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    using persistent_particles_t = std::vector<std::pair<Key::key_type, Particle*> >;
    using persistent_deposits_t  = std::vector<std::pair<CellID, EnergyDeposit*> >;

    /// Helper class to create output in edm4hep format
    /** Helper class to create output in edm4hep format
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2ROOTWriter::internals_t  final {
    public:
      struct BranchWrapper   {
	TClass*  clazz = nullptr;
	TBranch* branch = nullptr;
	void*    address = nullptr;
	void (*fcn_clear)(void* addr) = 0;
	void (*fcn_del)(void* addr) = 0;
	void clear()                    { this->fcn_clear(this->address); }
	void del()                      { this->fcn_del(this->address);   }
	template <typename T> T* get()  { return (T*)this->address;       }
	template <typename T> void set(T* ptr);
      };
      template <typename O> struct _wrapper_handler  {
	static void clear(void* ptr)  {  O* c = (O*)ptr; c->clear();  }
	static void del(void* ptr)    {  O* c = (O*)ptr; delete c;    }
      };
      typedef std::map<std::string, BranchWrapper> Collections;

      /// Reference to the parent
      Digi2ROOTWriter*         m_parent       { nullptr };
      /// Collections in the event tree
      Collections              m_collections  { };
      /// Reference to the ROOT file to open
      std::unique_ptr<TFile>   m_file         { };
      /// Reference to the event data tree
      TTree*                   m_tree         { nullptr };

      /// Property: name of the event tree
      std::string              m_section      { "EVENT" };

      /// Default basket size
      Int_t                    m_basket_size  { 32000 };
      /// Default split level
      Int_t                    m_split_level  {    99 };

    private:
      /// Helper to register single collection
      template <typename T> T* register_collection(const std::string& name, T* collection);

    public:
      /// Default constructor
      internals_t(Digi2ROOTWriter* parent);
      /// Default destructor
      ~internals_t();

      /// Open output file
      void open();
      /// Commit data to disk and close output stream
      void close();
      /// Commit data at end of filling procedure
      void commit();

      /// Create all collections according to the parent setup (locked)
      void create_collections();
      /// Clear collection content: Store is still owner!
      void clearCollections();
      /// Access named collection: throws exception ifd the collection is not present (unlocked!)
      template <typename T> BranchWrapper& get_collection(const T&);
    };

    template <typename T> void Digi2ROOTWriter::internals_t::BranchWrapper::set(T* ptr)   {
      clazz     = gROOT->GetClass(typeid(*ptr), kTRUE);
      branch    = nullptr;
      address   = ptr;
      fcn_clear = _wrapper_handler<T>::clear;
      fcn_del   = _wrapper_handler<T>::del;
    }

    /// Default constructor
    Digi2ROOTWriter::internals_t::internals_t(Digi2ROOTWriter* parent) : m_parent(parent)
    {
    }

    /// Default destructor
    Digi2ROOTWriter::internals_t::~internals_t()    {
      m_parent->info("Releasing allocated resources.");
      if ( m_file ) close();
      for( auto& coll : m_collections )   {
	coll.second.clear();
	coll.second.del();
      }
      m_collections.clear();
    }

    template <typename T> T* Digi2ROOTWriter::internals_t::register_collection(const std::string& nam, T* coll)   {
      BranchWrapper bw;
      bw.set(coll);
      m_collections.emplace(nam, bw);
      m_parent->debug("+++ created collection %s <%s>", nam.c_str(), typeName(typeid(T)).c_str());
      return coll;
    }

    /// Create all collections according to the parent setup
    void Digi2ROOTWriter::internals_t::create_collections()    {
      if ( m_collections.empty() )   {
        for( auto& cont : m_parent->m_containers )   {
          const std::string& nam = cont.first;
          const std::string& typ = cont.second;
          if ( typ == "MCParticles" )   {
            register_collection(nam, new persistent_particles_t());
	  }
          else   {
            register_collection(nam, new persistent_deposits_t());
	  }
        }
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

    /// Open output file
    void Digi2ROOTWriter::internals_t::open()   {
      if ( m_file )   {
	close();
      }
      std::string fname = m_parent->next_stream_name();
      m_file.reset(TFile::Open(fname.c_str(), "RECREATE", "DDDigi data"));
      m_tree = new TTree(m_section.c_str(), "DDDigi data", m_split_level, m_file.get());
      m_parent->info("+++ Opened ROOT output file %s", m_file->GetName());
      for( auto& coll : m_collections )    {
	auto& dsc = coll.second;
	dsc.branch = m_tree->Branch(coll.first.c_str(),
				    dsc.clazz->GetName(),
				    &dsc.address,
				    m_basket_size,
				    m_split_level);
	dsc.branch->SetAutoDelete(kFALSE);
      }
      m_parent->info("+++ Will save %ld events to %s",
		     m_parent->num_events, m_parent->m_output.c_str());
    }

    /// Commit data to disk and close output stream
    void Digi2ROOTWriter::internals_t::close()   {
      if ( m_file )    {
	TDirectory::TContext ctxt(m_file.get());
	m_parent->info("+++ Closing ROOT output file %s after %ld events and %ld bytes",
		       m_file->GetName(), m_tree->GetEntries(), m_file->GetBytesWritten());
	for( auto& coll : m_collections )   {
	  coll.second.branch->ResetAddress();
	  coll.second.branch = nullptr;
	}
	m_tree->Write();
	m_file->Close();
	m_tree = nullptr;
      }
      m_file.reset();
    }

    /// Commit data at end of filling procedure
    void Digi2ROOTWriter::internals_t::commit()   {
      if ( m_tree )   {
	m_tree->Fill();
        clearCollections();
	++m_parent->event_count;
	if ( m_parent->m_sequence_streams )  {
	  if ( 0 == (m_parent->event_count%m_parent->num_events) )  {
	    close();
	  }
	}
        return;
      }
      m_parent->except("+++ Failed to write output file. [Stream is not open]");
    }

    /// Standard constructor
    Digi2ROOTWriter::Digi2ROOTWriter(const DigiKernel& krnl, const std::string& nam)
      : DigiOutputAction(krnl, nam)
    {
      internals = std::make_shared<internals_t>(this);
      m_processor_type = "Digi2ROOTProcessor";
      declareProperty("basket_size",    internals->m_basket_size);
      declareProperty("split_level",    internals->m_split_level);
      InstanceCount::increment(this);
    }

    /// Default destructor
    Digi2ROOTWriter::~Digi2ROOTWriter()   {
      internals.reset();
      InstanceCount::decrement(this);
    }

    /// Initialization callback
    void Digi2ROOTWriter::initialize()   {
      this->DigiOutputAction::initialize();
      for ( auto& c : m_registered_processors )   {
        auto* act = dynamic_cast<Digi2ROOTProcessor*>(c.second);
        if ( act )   { // This is not nice! Need to think about something better.
          act->internals = this->internals;
	  continue;
        }
	except("Error: Invalid processor type for ROOT output: %s", c.second->c_name());
      }
      m_parallel = false;
      internals->create_collections();
    }

    /// Check for valid output stream
    bool Digi2ROOTWriter::have_output()  const  {
      return internals->m_file.get() != nullptr;
    }

    /// Open new output stream
    void Digi2ROOTWriter::open_output() const   {
      internals->open();
    }

    /// Close possible open stream
    void Digi2ROOTWriter::close_output()  const  {
      internals->close();
    }

    /// Commit event data to output stream
    void Digi2ROOTWriter::commit_output() const  {
      internals->commit();
    }

    /// Standard constructor
    Digi2ROOTProcessor::Digi2ROOTProcessor(const DigiKernel& krnl, const std::string& nam)
      : DigiContainerProcessor(krnl, nam)
    {
    }

    void Digi2ROOTProcessor::convert_particles(DigiContext&     ctxt,
					       ParticleMapping& cont)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec = coll.get<persistent_particles_t>();
      vec->clear();
      vec->reserve(cont.size());
      for( auto& p : cont )   {
	vec->emplace_back(std::make_pair(p.first, &p.second));
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_deposits(DigiContext&       ctxt,
					      DepositMapping&    cont,
					      const predicate_t& predicate)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec  = coll.get<persistent_deposits_t>();
      vec->clear();
      vec->reserve(cont.size());
      for ( auto& depo : cont )   {
	if ( predicate(depo) )   {
	  vec->emplace_back(std::make_pair(depo.first, &depo.second));
	}
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_deposits(DigiContext&       ctxt,
					      DepositVector&     cont,
					      const predicate_t& predicate)  const
    {
      auto& coll = internals->get_collection(cont);
      auto* vec  = coll.get<persistent_deposits_t>();
      vec->clear();
      vec->reserve(cont.size());
      for ( auto& depo : cont )   {
	if ( predicate(depo) )   {
	  vec->emplace_back(std::make_pair(depo.first, &depo.second));
	}
      }
      info("%s+++ %-24s added %6ld entries from mask: %04X",
           ctxt.event->id(), cont.name.c_str(), vec->size(), cont.key.mask());
    }

    void Digi2ROOTProcessor::convert_history(DigiContext&       ctxt,
					     DepositsHistory&   cont,
					     work_t&            work,
					     const predicate_t& predicate)  const
    {
      info("%s+++ %-32s Segment: %d Predicate:%s Conversion to edm4hep not implemented!",
           ctxt.event->id(), cont.name.c_str(), int(work.input.segment->id),
           typeName(typeid(predicate)).c_str());
    }

    /// Main functional callback
    void Digi2ROOTProcessor::execute(DigiContext& ctxt, work_t& work, const predicate_t& predicate)  const  {
      if ( auto* p = work.get_input<ParticleMapping>() )
        convert_particles(ctxt, *p);
      else if ( auto* m = work.get_input<DepositMapping>() )
        convert_deposits(ctxt, *m, predicate);
      else if ( auto* v = work.get_input<DepositVector>() )
        convert_deposits(ctxt, *v, predicate);
      else if ( auto* h = work.get_input<DepositsHistory>() )
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
