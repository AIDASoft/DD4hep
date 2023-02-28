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
#ifndef DD4HEP_DDDIGI_DIGIEDM4HEPINPUT_H
#define DD4HEP_DDDIGI_DIGIEDM4HEPINPUT_H

// Framework include files
#include <DDDigi/DigiInputAction.h>
#include <DDDigi/DigiData.h>

/// Forward declarations
namespace edm4hep  {
  class EventHeaderCollection;
  class MCParticleCollection;
}
namespace podio    {
  class CollectionBase;
  class Frame;
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DataSegment;
    class DataContext;

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
      class work_t;
      class source_t;
      class collection_t;
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
      /// Type of event header container
      std::string m_evtHeaderType  { };
      
    public:
      /// Initializing constructor
      DigiEdm4hepInput(const DigiKernel& krnl, const std::string& nam);

      /// Generic conversion function for hits
      template <typename HIT_TYPE, typename EDM4HEP_COLLECTION_TYPE>
      void hits_from_edm4hep(DigiContext&                   context,
			     DataSegment&                   segment,
			     Key::mask_type                 mask,
			     const std::string&             nam,
			     const EDM4HEP_COLLECTION_TYPE* collection)   const;

      /// Generic conversion function for MC particles
      void parts_from_edm4hep(DigiContext&       context,
			      DataSegment&       segment,
			      int                mask,
			      const std::string& nam,
			      const edm4hep::MCParticleCollection* collection)   const;

      /// Generic conversion function for event parameter settings
      void params_from_edm4hep(DigiContext&        context,
			       DataSegment&        segment,
			       int                 mask,
			       const std::string&  nam,
			       const podio::Frame& frame,
			       const edm4hep::EventHeaderCollection* collection)   const;

      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const;
      /// Event action callback
      virtual void execute(DigiContext& context)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIEDM4HEPINPUT_H
