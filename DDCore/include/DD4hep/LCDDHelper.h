// $Id$
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

#ifndef DD4HEP_LCDDHELPER_H
#define DD4HEP_LCDDHELPER_H
#include "DD4hep/LCDD.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {


  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// LCDDHelper: class to shortcut certain questions to the DD4hep detector description interface
    /**
     *
     * Note: This class may be extended with additional useful (generic) functionality,
     *       which requires the use of the main LCDD object.
     *
     * \author  M.Frank
     * \version 1.0
     * \ingroup DD4HEP_GEOMETRY
     */
    class LCDDHelper : public Handle<LCDD>  {
    public:
      /// Default constructor
      explicit LCDDHelper() : handle_t() {}
      /// Initializing constructor from pointer
      explicit LCDDHelper(LCDD* lcdd_ptr) : handle_t(lcdd_ptr) {}
      /// Initializing constructor from pointer
      explicit LCDDHelper(LCDD& lcdd_ref) : handle_t(&lcdd_ref) {}
      /// Copy constructor
      explicit LCDDHelper(const handle_t& h) : handle_t(h) {}
      /// Default destructor
      ~LCDDHelper() {}
      /// Access the sensitive detector of a given subdetector (if the sub-detector is sensitive!)
      SensitiveDetector sensitiveDetector(const std::string& detector) const;
      /// Given a detector element, access it's sensitive detector (if the sub-detector is sensitive!)
      SensitiveDetector sensitiveDetector(DetElement detector) const;      
      /// Find a detector element by it's system ID
      DetElement detectorByID(int id)  const;
    };

  } /* End namespace Geometry  */
} /* End namespace DD4hep    */

#endif /* DD4HEP_LCDDHELPER_H */
