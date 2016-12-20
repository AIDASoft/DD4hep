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
      /// Initializing constructor
      DetectorProcessor() = default;
      /// Default destructor
      virtual ~DetectorProcessor() = default;
      /// Callback to output detector information of an single DetElement
      virtual int operator()(DetElement de, int level);
      /// Callback to output detector information of an entire DetElement
      virtual int process(DetElement de, int level, bool recursive);
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
      /// Collection container
      T* processor;
    public:
      /// Default constructor
      DetElementProcessor() = delete;
      /// Default constructor
      DetElementProcessor(T* p) : processor(p) {}
      /// Default destructor
      virtual ~DetElementProcessor() = default;
    };

    /// Instantiation helper
    template <typename T> DetElementProcessor<T> detectorProcessor(T* proc)
    { return DetElementProcessor<T>(proc); }

    /// Generic Condition object collector
    /**
     *   Please see the documentation of the
     *   DetectorProcessor base class for further information.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_GEOMETRY
     */
    class DetectorCollector : virtual public DetectorProcessor  {
    public:
      /// Collection container
      std::vector<std::pair<int,DetElement> > detectors;
    public:
      /// Default constructor
      DetectorCollector() = default;
      /// Default destructor
      virtual ~DetectorCollector() = default;
      /// Callback to output detector information of an entire DetElement
      virtual int operator()(DetElement de, int level)  final;
    };
  }    /* End namespace Geometry             */
}      /* End namespace DD4hep               */
#endif /* DD4HEP_DDCORE_DETECTORPROCESSOR_H  */
