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

#ifndef DD4HEP_DETECTORSELECTOR_H
#define DD4HEP_DETECTORSELECTOR_H

// Framework include files
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// View on Detector to select detectors according to certain criteria
  /** 
   * Usage:
   *
   *  Detector& lcd = ....;
   *  DetectorSelector selector(description);
   *  DetectorSelector::Result r = selector.detectors("tracker");
   *
   *  or multiple types:
   *  r = selector.detectors("tracker", "calorimeter");
   *
   *  or selections using DetType flags, e.g:
   *  select all barrel trackers but not the Vertex detector
   *  r = selector.detectors(  DetType::TRACKER | DetType::BARREL  , DetType::VERTEX ) ;
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class DetectorSelector {
  public: 
    /// Result set definition
    typedef std::vector<DetElement> Result;
#ifndef __CINT__
    /// Reference to main detector description object
    Detector& description;
#endif

  public:
#ifndef __CINT__
    /// Default constructor
    DetectorSelector(Detector& _description) : description(_description)  {}
#endif
    /// Default destructor
    ~DetectorSelector()  {}

    /// Access a set of subdetectors according to the sensitive type.
    /**
       Please note:
       - The sensitive type of a detector is set in the 'detector constructor'.
       - Not sensitive detector structures have the name 'passive'
       - Compounds (ie. nested detectors) are of type 'compound'
    */
    const Result& detectors(const std::string& type);

    /// Access a set of subdetectors according to several sensitive types.
    Result detectors(const std::string& type1,
                     const std::string& type2,
                     const std::string& type3="",
                     const std::string& type4="",
                     const std::string& type5="" );

    /** return a vector with all detectors that have all the type properties in
     *  includeFlag set but none of the properties given in excludeFlag
     */
    Result detectors(unsigned int includeFlag, 
                     unsigned int excludeFlag=0 ) const ;
  };

} /* End namespace dd4hep        */
#endif    /* DD4HEP_DETECTORSELECTOR_H      */
