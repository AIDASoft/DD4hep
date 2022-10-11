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
#ifndef DDDIGI_DIGICONTEXT_H
#define DDDIGI_DIGICONTEXT_H

// Framework incloude files
#include <DD4hep/Primitives.h>
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiRandomGenerator.h>

/// C/C++ include files
#include <memory>
#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  // Forward declarations
  class Detector;
  class DetElement;

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiActionSequence;
    class DigiKernel;

    /// Generic context to extend user, run and event information
    /**
     *  A valid instance of the DigiContext is passed to every instance of a DigiAction at 
     *  creation time.
     *
     *  The DigiContext is the main thread specific accessor to the dd4hep, DDG4 and
     *  the user framework.
     *  - The access to the dd4hep objects is via the DigiContext::detectorDescription() call,
     *  - the access to DDG4 as a whole is supported via DigiContext::kernel() and
     *  - the access to the user gframework using a specialized implementation of:
     *  template <typename T> T& userFramework()  const;
     *
     *  A user defined implementations must be specialized somewhere in a compilation unit
     *  of the user framework, not in a header file. The framework object could host
     *  e.g. references for histogramming, logging, data access etc.
     *
     *  This way any experiment/user related data processing framework can exhibit
     *  its essential tools to DDG4 actions.
     *
     *  A possible specialized implementations would look like the following:
     *
     *  struct Gaudi  {
     *    IMessageSvc*   msg;
     *    IHistogramSvc* histos;
     *    ....
     *  };
     *
     *  template<> Gaudi& DigiContext::userFramework<Gaudi>()  const  {
     *    UserFramework& fw = m_kernel->userFramework();
     *    if ( fw.first && &typeid(T) == fw.second ) return *(T*)fw.first;
     *    throw std::runtime_error("No user specified framework context present!");
     *  }
     *
     *  To access the user framework then use the following call:
     *  Gaudi* fw = context->userFramework<Gaudi>();
     *
     *  of course after having initialized it:
     *  Gaudi * fw = ...;
     *  GaudiKernel& kernel = ...;
     *  kernel.setUserFramework(fw);
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiContext  {
      friend class DigiKernel;
    public:
      typedef std::pair<void*, const std::type_info*>   UserFramework;

    public:
      /// Reference to the thread context
      const DigiKernel&           kernel;
      /// Transient context variable - depending on the thread context: event reference
      std::unique_ptr<DigiEvent>  event  { };

    protected:
      /// Reference to the random engine for this event
      std::shared_ptr<DigiRandomGenerator> m_random;


    protected:
      /// Inhibit default constructor
      DigiContext() = delete;
      /// Inhibit move constructor
      DigiContext(DigiContext&&) = delete;
      /// Inhibit copy constructor
      DigiContext(const DigiContext&) = delete;

    public:

      /// Initializing constructor
      DigiContext(const DigiKernel& kernel, std::unique_ptr<DigiEvent>&& event);
      /// Default destructor
      virtual ~DigiContext();

      /// Have a shared initializer lock
      std::mutex& initializer_lock()   const;
      /// Have a global I/O lock (e.g. for ROOT)
      std::mutex& global_io_lock()   const;
      /// Have a global output log lock
      std::mutex& global_output_lock()   const;

      /// Access to the random engine for this event
      DigiRandomGenerator& randomGenerator()  const  { return *m_random; }
      /// Access to the user framework. Specialized function to be implemented by the client
      template <typename T> T& framework()  const;
      /// Generic framework access
      UserFramework& userFramework() const;
      /// Access to detector description
      Detector& detectorDescription() const;
      /// Access to the main input action sequence from the kernel object
      DigiActionSequence& inputAction() const;
      /// Access to the main event action sequence from the kernel object
      DigiActionSequence& eventAction() const;
      /// Access to the main output action sequence from the kernel object
      DigiActionSequence& outputAction() const;
    };

  }    // End namespace digi
}      // End namespace dd4hep

#endif // DDDIGI_DIGICONTEXT_H
