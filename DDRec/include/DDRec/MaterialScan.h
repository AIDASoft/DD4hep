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
#ifndef DD4HEP_DDREC_MATERIALSCAN_H
#define DD4HEP_DDREC_MATERIALSCAN_H

// Framework include files
#include "DDRec/MaterialManager.h"

#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Forward declarations
  class Detector;

  /// Namespace for the reconstruction part of the AIDA detector description toolkit
  namespace rec {

    /// Class to perform material scans along a straight line
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_REC
     */
    class MaterialScan  {
    private:
      
      /// Reference to detector setup
      Detector& m_detector;
      /// Material manager
      std::unique_ptr<MaterialManager> m_materialMgr;  //!

      /// Default constructor
      MaterialScan();
 
    public:

      /// Standard constructor for the master instance
      MaterialScan(Detector& description);

      /// Default destructor
      virtual ~MaterialScan();

      /// Set a specific detector volume to limit the scan
      void setDetector(DetElement detector);

      /// Scan along a line and store the matrials internally
      const MaterialVec& scan(double x0, double y0, double z0, double x1, double y1, double z1, double epsilon=1e-4)  const;

      /// Scan along a line and print the materials traversed
      void print(const Vector3D& start, const Vector3D& end, double epsilon=1e-4)  const;

      /// Scan along a line and print the materials traversed
      void print(double x0, double y0, double z0,
                 double x1, double y1, double z1,
                 double epsilon=1e-4)  const;
    };
  }    // End namespace rec
}      // End namespace dd4hep
#endif // DD4HEP_DDREC_MATERIALSCAN_H
