//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDCORE_DETECTORPROCESSOR_H
#define DD4HEP_DDCORE_DETECTORPROCESSOR_H

// Framework includes
#include "DD4hep/Detector.h"

// C/C++ include files
#include <vector>
#include <memory>
#include <algorithm>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace Geometry  {

    /// Generic Detector processor
    /**
     *   Please note that the principle of locality applies:
     *   The object is designed for stack allocation and configuration.
     *   It may NOT be shared across threads!
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_GEOMETRY
     */
    class DetectorProcessor {
    public:
      /// Self type definition
      typedef DetectorProcessor  self_type;
    public:
      struct Internal {
        virtual ~Internal() {}
      };
      /// Initializing constructor
      DetectorProcessor() = default;
      /// Default destructor
      virtual ~DetectorProcessor() = default;
      /// Callback to output detector information of an single DetElement
      virtual int operator()(DetElement de, int level)  const = 0;
      /// Callback to output detector information of an entire DetElement
      virtual int process(DetElement de, int level, bool recursive)  const;
      /// Pointer conversion to underlying object
      virtual Internal* internal()  const  {  return 0; }
    };

    /// Detector scanner using a Processor object
    /**
     *   Please see the documentation of the
     *   DetectorProcessor base class for further information.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class DetElementProcessor : virtual public DetectorProcessor  {
    public:
      /// Reference to execution object implementing operator()(DetElement de, int level)
      T& processor;
    public:
      /// Default constructor
      DetElementProcessor() = delete;
      /// Default constructor
      DetElementProcessor(T& p) : processor(p) {}
      /// Default move constructor is disabled
      DetElementProcessor(T&& p) = delete;
      /// R-value copy from a temporary (Since processor is reference)
      DetElementProcessor(DetElementProcessor&& copy) = default;
      /// Default copy constructor
      DetElementProcessor(const DetElementProcessor& copy) = default;
      /// Default destructor
      virtual ~DetElementProcessor() = default;
      /// Default assignment
      DetElementProcessor& operator=(const DetElementProcessor& copy) = default;
      /// Callback to output detector information of an single DetElement
      virtual int operator()(DetElement de, int level)  const final
      {   return (processor)(de, level);         }
      /// Pointer conversion to underlying object
      virtual Internal* internal()  const  final
      {  return (Internal*)&processor;           }
    };

    /// Instantiation helper
    template <typename T> inline
    DetElementProcessor<typename std::remove_reference<T>::type> detectorProcessor(T&& proc)
    { return DetElementProcessor<typename std::remove_reference<T>::type>(std::forward<T>(proc)); }

    /// Wrapper to call objects in the form of a detector element processor.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     */
    template <typename T> class DetectorProcessorShared : public DetectorProcessor {
      /// Reference to execution object implementing operator()(DetElement de, int level)
      std::shared_ptr<T> processor;
    public:
      /// Default constructor
      DetectorProcessorShared() = delete;
      /// Default constructor
      DetectorProcessorShared(std::shared_ptr<T>& p) : processor(p) {}
      /// Default copy constructor
      DetectorProcessorShared(const DetectorProcessorShared& copy) = default;
      /// Default destructor
      virtual ~DetectorProcessorShared() = default;
      /// Default assignment
      DetectorProcessorShared& operator=(const DetectorProcessorShared& copy) = default;
      /// Callback to output detector information of an single DetElement
      virtual int operator()(DetElement de, int level)  const final
      {  return (*processor)(de, level);                 }
      /// Pointer conversion to underlying object
      virtual Internal* internal()  const  final
      {  return (Internal*)processor.get();              }
    };

    /// Helper to run DetElement scans
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(DetElement de, int level) const
     *  The object is automatically wrapped to a DetectorProcessor
     *  and the detector tree is scanned depending on the scanning
     *  arguments.
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    class DetectorScanner  {
    public:
      /// Default constructor
      DetectorScanner() = default;
      /// Copy constructor
      DetectorScanner(const DetectorScanner& copy) = default;
      /// Assignment operator
      DetectorScanner& operator=(const DetectorScanner& copy) = default;

      /// Constructor performing the scan internally
      template <typename Q>
      DetectorScanner(Q& proc, DetElement start, int level=0, bool recursive=true)
      {  scan(proc, start, level, recursive);     }

      /// Constructor performing the scan internally
      template <typename Q>
      DetectorScanner(const Q& proc, DetElement start, int level=0, bool recursive=true)
      {  scan(proc, start, level, recursive);     }

      /// Detector element tree scanner using wrapped DetectorProcessor objects
      template <typename Q>
      int scan(Q& p, DetElement start, int level=0, bool recursive=true)  const {
        auto proc = detectorProcessor(p);
        return proc.process(start, level, recursive);
      }

      /// Detector element tree scanner using wrapped DetectorProcessor objects
      template <typename Q>
      int scan(const Q& p, DetElement start, int level=0, bool recursive=true) const {
        auto proc = detectorProcessor(p);
        return proc.process(start, level, recursive);
      }
    };

  }    /* End namespace Geometry             */
}      /* End namespace DD4hep               */
#endif /* DD4HEP_DDCORE_DETECTORPROCESSOR_H  */
