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
#ifndef DD4HEP_VOLUMEPROCESSOR_H
#define DD4HEP_VOLUMEPROCESSOR_H

// Framework includes
#include "DD4hep/DetElement.h"
#include "DD4hep/Volumes.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Generic PlacedVolume processor
  /**
   *   Please note that the principle of locality applies:
   *   The object is designed for stack allocation and configuration.
   *   It may NOT be shared across threads!
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/05/2017
   *   \ingroup DD4HEP_CORE
   */
  class PlacedVolumeProcessor {
  public:
    /// Initializing constructor
    PlacedVolumeProcessor() = default;
    /// R-value copy from a temporary (Since processor is reference)
    PlacedVolumeProcessor(PlacedVolumeProcessor&& copy) = default;
    /// Default copy constructor
    PlacedVolumeProcessor(const PlacedVolumeProcessor& copy) = default;
    /// Default destructor
    virtual ~PlacedVolumeProcessor();
    /// Default assignment
    PlacedVolumeProcessor& operator=(const PlacedVolumeProcessor& copy) = default;
    /// Callback to output PlacedVolume information of an single Placement
    virtual int operator()(PlacedVolume pv, int level) = 0;
    /// Callback to output PlacedVolume information of an entire Placement
    virtual int process(PlacedVolume pv, int level, bool recursive);
  };

  /// PlacedVolume scanner using a Processor object
  /**
   *   Please see the documentation of the
   *   PlacedVolumeProcessor base class for further information.
   *   The only requirement to the object is to fullfill the callback signature.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/05/2017
   *   \ingroup DD4HEP_CORE
   */
  template <typename T> class PlacementProcessor : virtual public PlacedVolumeProcessor  {
  public:
    /// Reference to execution object implementing operator()(PlacedVolume pv, int level)
    T& processor;
  public:
    /// Default constructor
    PlacementProcessor() = delete;
    /// Default constructor
    PlacementProcessor(T& p) : processor(p) {}
    /// Default move constructor is disabled
    PlacementProcessor(T&& p) = delete;
    /// R-value copy from a temporary (Since processor is reference)
    PlacementProcessor(PlacementProcessor&& copy) = default;
    /// Default copy constructor
    PlacementProcessor(const PlacementProcessor& copy) = default;
    /// Default destructor
    virtual ~PlacementProcessor() = default;
    /// Default assignment
    PlacementProcessor& operator=(const PlacementProcessor& copy) = default;
    /// Callback to output detector information of an single placement
    virtual int operator()(PlacedVolume pv, int level)  final
    {   return (processor)(pv, level);         }
  };

  /// Instantiation helper
  template <typename T> inline
  PlacementProcessor<typename std::remove_reference<T>::type> placementProcessor(T&& proc)
  { return PlacementProcessor<typename std::remove_reference<T>::type>(std::forward<T>(proc)); }

  /// Wrapper to call objects in the form of a PlacedVolume processor.
  /**
   *   \author  M.Frank
   *   \version 1.0
   *   \date    31/05/2017
   *   \ingroup DD4HEP_CORE
   */
  template <typename T> class PlacementProcessorShared : public PlacedVolumeProcessor {
  public:
    /// Reference to execution object implementing operator()(PlacedVolume pv, int level)
    std::shared_ptr<T> processor;
  public:
    /// Default constructor
    PlacementProcessorShared() = delete;
    /// Default constructor
    PlacementProcessorShared(std::shared_ptr<T>& p) : processor(p) {}
    /// Default copy constructor
    PlacementProcessorShared(const PlacementProcessorShared& copy) = default;
    /// Default destructor
    virtual ~PlacementProcessorShared() = default;
    /// Default assignment
    PlacementProcessorShared& operator=(const PlacementProcessorShared& copy) = default;
    /// Callback to output detector information of an single DetElement
    virtual int operator()(PlacedVolume pv, int level)  final
    {  return (*processor)(pv, level);                 }
  };

  /// Helper to run placement scan through volume hierarchies scans
  /**
   *   This wrapper converts any object, which has the signature
   *   int operator()(DetElement de, int level) const
   *   The object is automatically wrapped to a PlacedVolumeProcessor
   *   and the PlacedVolume tree is scanned depending on the scanning
   *   arguments.
   *  
   *   \author  M.Frank
   *   \version 1.0
   *   \date    01/04/2016
   *   \ingroup DD4HEP_CORE
   */
  class PlacedVolumeScanner  {
  public:
    /// Default constructor
    PlacedVolumeScanner() = default;
    /// Copy constructor
    PlacedVolumeScanner(const PlacedVolumeScanner& copy) = default;
    /// Assignment operator
    PlacedVolumeScanner& operator=(const PlacedVolumeScanner& copy) = default;

    /// Constructor performing the scan internally
    template <typename Q>
    PlacedVolumeScanner(Q& proc, DetElement start, int level=0, bool recursive=true)
    {  scan(proc, start.placement(), level, recursive);     }

    /// Constructor performing the scan internally
    template <typename Q>
    PlacedVolumeScanner(const Q& proc, DetElement start, int level=0, bool recursive=true)
    {  scan(proc, start.placement(), level, recursive);     }

    /// Constructor performing the scan internally
    template <typename Q>
    PlacedVolumeScanner(Q& proc, PlacedVolume start, int level=0, bool recursive=true)
    {  scan(proc, start, level, recursive);     }

    /// Constructor performing the scan internally
    template <typename Q>
    PlacedVolumeScanner(const Q& proc, PlacedVolume start, int level=0, bool recursive=true)
    {  scan(proc, start, level, recursive);     }

    /// PlacedVolume element tree scanner using wrapped PlacedVolumeProcessor objects
    int scanPlacements(PlacedVolumeProcessor& proc, PlacedVolume start, int level=0, bool recursive=true)  const {
      return proc.process(start, level, recursive);
    }
    /// PlacedVolume element tree scanner using wrapped PlacedVolumeProcessor objects
    template <typename Q>
    int scan(Q& p, PlacedVolume start, int level=0, bool recursive=true)  const {
      auto proc = placementProcessor(p);
      return proc.process(start, level, recursive);
    }
    /// PlacedVolume element tree scanner using wrapped PlacedVolumeProcessor objects
    template <typename Q>
    int scan(const Q& p, PlacedVolume start, int level=0, bool recursive=true) const {
      auto proc = placementProcessor(p);
      return proc.process(start, level, recursive);
    }
  };
}      /* End namespace dd4hep               */
#endif // DD4HEP_VOLUMEPROCESSOR_H
