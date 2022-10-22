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
#ifndef DDDIGI_DIGIATTENUATOR_H
#define DDDIGI_DIGIATTENUATOR_H

// Framework include files
#include <DDDigi/DigiContainerProcessor.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Tool to compute decay time of the signal for spillover
    /**
     *  The following signal decays are supported:
     *  -- Exponential decay depending on time offset(t0) and mean signal decay time
     *  -- more to be added
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiAttenuationTool   {
    public:
      /// Exponential decay depending on time offset(t0) and mean signal decay time
      double exponential(double t0, double decay_time)  const;
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiAttenuator : public DigiContainerProcessor   {
    protected:
      /// Signal reduction factor
      double  m_factor { 1.0 };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiAttenuator);
      /// Default destructor
      virtual ~DigiAttenuator();
      /// Attenuator callback for single container
      template <typename T> std::size_t attenuate(T& cont) const;

    public:
      /// Standard constructor
      DigiAttenuator(const DigiKernel& kernel, const std::string& nam);
      /// Main functional callback adapter
      virtual void execute(DigiContext& context, work_t& work)  const override;
    };

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiAttenuatorSequence : public  DigiContainerSequenceAction  {
    protected:
      /// Property: Data processor type
      std::string                    m_processor_type { };
      /// Property: Signal decay algorithm
      std::string                    m_signal_decay   { };
      /// Property: Container names to be loaded
      std::map<std::string, double>  m_container_attenuation  { };
      /// Property: T0 with respect to central crossing
      double                         m_t0     { 0e0 };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiAttenuatorSequence);
      /// Default destructor
      virtual ~DigiAttenuatorSequence();

    public:
      /// Standard constructor
      DigiAttenuatorSequence(const DigiKernel& kernel, const std::string& nam);
      /// Initialization callback
      virtual void initialize();
      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIATTENUATOR_H
