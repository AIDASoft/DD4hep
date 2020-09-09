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
#ifndef DDREC_MATERIALSCAN_H
#define DDREC_MATERIALSCAN_H

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
     *  Examples: from DDDetectors/compact/SiD.xml
     *  $> materialScan file:checkout/DDDetectors/compact/SiD.xml -interactive
     *
     *  1) Simple scan:
     *     root [0] gMaterialScan->print(5,5,0,5,5,400)
     *  2) Scan a given subdetector:
     *     root [0] de=gDD4hepUI->instance()->detector("LumiCal");
     *     root [1] gMaterialScan->setDetector(de);
     *     root [2] gMaterialScan->print(5,5,0,5,5,400)
     *  3) Scan by material:
     *     root [0] gMaterialScan->setMaterial("Silicon");
     *     root [1] gMaterialScan->print(5,5,0,5,5,400)
     *  4) Scan by region:
     *     root [0] gMaterialScan->setRegion("SiTrackerBarrelRegion");
     *     root [1] gMaterialScan->print(0,0,0,100,100,0)
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_REC
     */
    class MaterialScan  {
    private:
      
      /// Reference to detector setup
      Detector&                        m_detector;
      /// Material manager
      std::unique_ptr<MaterialManager> m_materialMgr;  //!
      /// Local cache: subdetector placements
      std::set<const TGeoNode*>        m_placements;
      /// Default constructor
      MaterialScan();
 
    public:

      /// Standard constructor for the master instance
      MaterialScan(Detector& description);

      /// Default destructor
      virtual ~MaterialScan();

      /// Set a specific detector volume to limit the scan (resets other selection criteria)
      void setDetector(DetElement detector);
      /// Set a specific detector volume to limit the scan (resets other selection criteria)
      void setDetector(const char* detector);

      /// Set a specific volume material to limit the scan (resets other selection criteria)
      void setMaterial(const char* material);
      /// Set a specific volume material to limit the scan (resets other selection criteria)
      void setMaterial(Material material);

      /// Set a specific region to limit the scan (resets other selection criteria)
      void setRegion(const char* region);
      /// Set a specific region to limit the scan (resets other selection criteria)
      void setRegion(Region region);

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
#endif // DDREC_MATERIALSCAN_H
