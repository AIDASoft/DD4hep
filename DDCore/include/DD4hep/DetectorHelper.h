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

#ifndef DD4HEP_DETECTORHELPER_H
#define DD4HEP_DETECTORHELPER_H

#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// DetectorHelper: class to shortcut certain questions to the dd4hep detector description interface
  /**
   *
   * Note: This class may be extended with additional useful (generic) functionality,
   *       which requires the use of the main Detector object.
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_CORE
   */
  class DetectorHelper : public Handle<Detector>  {
  public:
    /// Default constructor
    DetectorHelper() = default;
    /// Copy constructor
    DetectorHelper(const DetectorHelper& h) = default;
    /// Initializing constructor from pointer
    DetectorHelper(Detector* description_ptr) : Handle<Detector>(description_ptr) {}
    /// Initializing constructor from pointer
    DetectorHelper(Detector& description_ref) : Handle<Detector>(&description_ref) {}
    /// Copy constructor
    DetectorHelper(const Handle<Detector>& h) : Handle<Detector>(h) {}
    /// Default destructor
    ~DetectorHelper() = default;
    /// Assignment operator
    DetectorHelper& operator=(const DetectorHelper& c) = default;
    /// Access the sensitive detector of a given subdetector (if the sub-detector is sensitive!)
    SensitiveDetector sensitiveDetector(const std::string& detector) const;
    /// Given a detector element, access it's sensitive detector (if the sub-detector is sensitive!)
    SensitiveDetector sensitiveDetector(DetElement detector) const;      
    /// Find a detector element by it's system ID
    DetElement detectorByID(int id)  const;
    /// Access an element from the element table by name
    Atom element(const std::string& name)  const;
    /// Access a material from the material table by name
    Material material(const std::string& name)  const;
  };

}       /* End namespace dd4hep               */
#endif // DD4HEP_DETECTORHELPER_H
