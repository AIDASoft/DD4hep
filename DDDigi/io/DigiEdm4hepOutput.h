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
#ifndef DIGI_DIGIEDM4HEPOUTPUT_H
#define DIGI_DIGIEDM4HEPOUTPUT_H

// Framework include files
#include <DDDigi/DigiOutputAction.h>

/// C/C++ include files
#if __has_include("edm4hep/TrackerHitCollection.h")
#include <edm4hep/TrackerHitCollection.h>
namespace edm4hep {
  using TrackerHit3DCollection = edm4hep::TrackerHitCollection;
}
#else
#include <edm4hep/TrackerHit3DCollection.h>
#endif

/// Forward declarations from edm4hep
namespace edm4hep  {
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
     *  - edm4hep::TrackerHit3DCollection aka "TracketHits"
     *
     *  This entity actually is only the work dispatcher:
     *  It opens files and dumps data into
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepOutput : public  DigiOutputAction  {
    public:
      class internals_t;
    protected:
      /// Reference to internals
      std::shared_ptr<internals_t> internals;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiEdm4hepOutput);
      /// Default destructor
      virtual ~DigiEdm4hepOutput();

    public:
      /// Standard constructor
      DigiEdm4hepOutput(const kernel_t& kernel, const std::string& nam);
      /// Initialization callback
      virtual void initialize();
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
    /** The processor is the actual unit doing the work.
     *
     *  This is a typical worker action of the DigiEdm4hepOutput
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiEdm4hepOutputProcessor : public DigiContainerProcessor   {
      friend class DigiEdm4hepOutput;

    protected:
      /// Reference to the edm4hep engine
      std::shared_ptr<DigiEdm4hepOutput::internals_t> internals;
      /// Property: RPhi resolution
      float m_pointResoutionRPhi = 0.004;
      /// Property: Z resolution
      float m_pointResoutionZ = 0.004;
      /// Hit type for hit processor
      int   m_hit_type = 0;

    public:
      /// Standard constructor
      DigiEdm4hepOutputProcessor(const DigiKernel& krnl, const std::string& nam);

      /// Standard destructor
      virtual ~DigiEdm4hepOutputProcessor() = default;

      /// Convert tracker hits to edm4hep
      template <typename T> void
      convert_depos(const T& cont, const predicate_t& predicate, edm4hep::TrackerHit3DCollection* collection)  const;

      /// Convert calorimeter hits to edm4hep
      template <typename T> void
      convert_depos(const T& cont, const predicate_t& predicate, edm4hep::CalorimeterHitCollection* collection)  const;

      /// Dispatcher function to convert any kind of deposits
      template <typename T> void
      convert_deposits(DigiContext& context, const T& cont, const predicate_t& predicate)  const;

      /// Convert history records to edm4hep
      void convert_history(DigiContext& context, const DepositsHistory& cont, work_t& work, const predicate_t& predicate)  const;

      /// Convert particle record to edm4hep
      void convert_particles(DigiContext& context, const ParticleMapping& cont)  const;

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const override final;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DIGI_DIGIEDM4HEPOUTPUT_H
