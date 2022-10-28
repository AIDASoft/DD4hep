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
#ifndef DDDIGI_DIGIKERNEL_H
#define DDDIGI_DIGIKERNEL_H

// Framework include files
#include <DD4hep/Callback.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiParallelWorker.h>

// C/C++ include files
#include <mutex>
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiAction;
    class DigiActionSequence;
    
    /// Class, which allows all DigiAction derivatives to access the DDG4 kernel structures.
    /**
     *  To implement access to a user specified framework please see class DigiContext.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiKernel  {
    public:
      typedef std::map<std::string,int>                 ClientOutputLevels;
      typedef std::pair<void*, const std::type_info*>   UserFramework;

    private:
      class Internals;
      class Processor;
      template <typename ACTION, typename ARGUMENT> class Wrapper;

      /// Internal only data structures;
      Internals*            internals   { nullptr };
      
    protected:
      /// Detector description object
      Detector*             m_detDesc         { nullptr };
      /// Reference to the user framework
      mutable UserFramework m_userFramework;
      
      /// Execute one single event
      virtual void executeEvent(std::unique_ptr<DigiContext>&& context);
      /// Notify kernel that the execution of one single event finished
      void notify(std::unique_ptr<DigiContext>&& context);
      /// Notify kernel that the execution of one single event finished
      void notify(std::unique_ptr<DigiContext>&&, const std::exception& e);
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiKernel);

    public:
      /// Standard constructor for the master instance
      DigiKernel(Detector& description);

    public:
      /// Default destructor
      virtual ~DigiKernel();

#ifndef __CINT__
      /// Instance accessor
      static DigiKernel& instance(Detector& description);
#endif
      /// Access to detector description
      Detector& detectorDescription() const     {        return *m_detDesc;        }
      /// Generic framework access
      UserFramework& userFramework() const      {        return m_userFramework;   }
      /// Set the framework context to the kernel object
      template <typename T> void setUserFramework(T* object)   {
        m_userFramework = UserFramework(object,&typeid(T));
      }

      /// Have a shared initializer lock
      std::mutex& initializer_lock()  const;

      /// Have a global I/O lock (e.g. for ROOT)
      std::mutex& global_io_lock()   const;

      /// Have a global output log lock
      std::mutex& global_output_lock()   const;
      
      /** Property access                            */
      /// Access to the properties of the object
      PropertyManager& properties();
      /// Print the property values
      void printProperties() const;
      /// Declare property
      template <typename T> DigiKernel& declareProperty(const std::string& nam, T& val);
      /// Declare property
      template <typename T> DigiKernel& declareProperty(const char* nam, T& val);
      /// Check property for existence
      bool hasProperty(const std::string& name) const;
      /// Access single property
      Property& property(const std::string& name);

      /** Output level settings                       */
      /// Access the output level
      PrintLevel outputLevel() const;
      /// Set the global output level of the kernel object; returns previous value
      PrintLevel setOutputLevel(PrintLevel new_level);
      /// Fill cache with the global output level of a named object. Must be set before instantiation
      void setOutputLevel(const std::string object, PrintLevel new_level);
      /// Retrieve the global output level of a named object.
      PrintLevel getOutputLevel(const std::string object) const;

      /// Access current number of events still to process
      std::size_t events_todo()  const;
      /// Access current number of events already processed
      std::size_t events_done()  const;
      /// Access current number of events processing (events in flight)
      std::size_t events_processing()  const;

      /// Register configure callback. Signature:   (function)()
      void register_configure(const Callback& callback)   const;
      /// Register initialize callback. Signature:  (function)()
      void register_initialize(const Callback& callback)   const;
      /// Register terminate callback. Signature:   (function)()
      void register_terminate(const Callback& callback)   const;
      /// Register start event callback. Signature: (function)(DigiContext*)
      void register_start_event(const Callback& callback)   const;
      /// Register end event callback. Signature:   (function)(DigiContext*)
      void register_end_event(const Callback& callback)   const;

      /// Construct detector geometry using description plugin
      virtual void loadGeometry(const std::string& compact_file);
      /// Load XML file 
      virtual void loadXML(const char* fname);

      /// Configure the digitization: call all registered configurators
      virtual int configure();
      /// Initialize the digitization: call all registered initializers
      virtual int initialize();
      /// Run the digitization sequence over the requested number of events
      virtual int run();
      /// Terminate the digitization: call all registered terminators and release the allocated resources
      virtual int terminate();

      /// Access to the main input action sequence from the kernel object
      DigiActionSequence& inputAction() const;
      /// Access to the main event action sequence from the kernel object
      DigiActionSequence& eventAction() const;
      /// Access to the main output action sequence from the kernel object
      DigiActionSequence& outputAction() const;

      /// Submit a bunch of actions to be executed in parallel
      virtual void submit (DigiContext& context, ParallelCall*const algorithms[], std::size_t count, void* data, bool parallel=true)  const;

      /// Submit a bunch of actions to be executed in parallel
      virtual void submit (DigiContext& context, const std::vector<ParallelCall*>& algorithms, void* data, bool parallel=true)  const;

      /// If running multithreaded: wait until the thread-group finished execution
      virtual void wait(DigiContext& context)   const;

    };

    /// Declare property
    template <typename T> inline DigiKernel& DigiKernel::declareProperty(const std::string& nam, T& val) {
      properties().add(nam, val);
      return *this;
    }

    /// Declare property
    template <typename T> inline DigiKernel& DigiKernel::declareProperty(const char* nam, T& val) {
      properties().add(nam, val);
      return *this;
    }
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIKERNEL_H
