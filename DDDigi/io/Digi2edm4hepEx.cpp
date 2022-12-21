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
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiContainerProcessor.h>

// edm4hep include files
#include <podio/EventStore.h>
#include <podio/ROOTWriter.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/CaloHitContributionCollection.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to select energy deposits according to the supplied segmentation
    /** Actor to select energy deposits according to the supplied segmentation
     *
     *  The selected deposits are placed in the output container
     *  supplied by the arguments.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class Digi2edm4hep : public DigiContainerProcessor   {

      std::unordered_map<std::string, podio::CollectionBase*> m_collections;
    public:
      /// Standard constructor
      Digi2edm4hep(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	//declareProperty("mean",  m_mean);
	//declareProperty("sigma", m_sigma);
      }

      /// Standard destructor
      virtual ~Digi2edm4hep() = default;

      template <typename T> void
      convert_deposits(const char* tag, const T& cont, work_t& work, const predicate_t& predicate)  const  {
	Key key(cont.name, work.environ.output.mask);
	DepositMapping m(cont.name, work.environ.output.mask, cont.data_type);
	std::size_t start = m.size();
	for( const auto& dep : cont )   {
	  if ( predicate(dep) )    {
	    m.data.emplace(dep.first, EnergyDeposit());
	  }
	}
	std::size_t end   = m.size();
	work.environ.output.data.put(m.key, std::move(m));
	info("%s+++ %-32s added %6ld entries (now: %6ld) from mask: %04X to mask: %04X",
	     tag, cont.name.c_str(), end-start, end, cont.key.mask(), m.key.mask());
      }
      void convert_history(const char* tag, const DepositsHistory& cont, work_t& work, const predicate_t& predicate)  const  {
      }
      void convert_particles(const char* tag, const ParticleMapping& cont, work_t& work, const predicate_t& predicate)  const  {
	//auto mcpc = static_cast<edm4hep::MCParticleCollection*>(m_collections["MCParticles"]);
	
      }
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final  {
	if ( const auto* m = work.get_input<DepositMapping>() )
	  convert_deposits(context.event->id(), *m, work, predicate);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  convert_deposits(context.event->id(), *v, work, predicate);
	else if ( const auto* h = work.get_input<DepositsHistory>() )
	  convert_history(context.event->id(), *h, work, predicate);
	else if ( const auto* p = work.get_input<ParticleMapping>() )
	  convert_particles(context.event->id(), *p, work, predicate);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

/// Factory instantiation:
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,Digi2edm4hep)
