//==========================================================================
//  ALEPH software suite
//--------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
//
//  For the licensing terms see OnlineSys/LICENSE.
//
//--------------------------------------------------------------------------
//
//  Author     : Markus Frank
//==========================================================================
#ifndef  DDCORE_XML_DETECTORSTRUCTURE_H
#define  DDCORE_XML_DETECTORSTRUCTURE_H

/// DD4hep include files
#include <DD4hep/Printout.h>
#include <DD4hep/DetFactoryHelper.h>

/// dd4hep namespace declaration
namespace dd4hep {

  /// Helper class to define subdetectors depending on volume names/placement pathes
  class DetectorStructure  {
  private:
    Detector&         description;
    SensitiveDetector sensitive {        };
    std::string       name      {        };
    PrintLevel        print     { ALWAYS };
    int               id        {     -1 };
    xml_det_t         xd;
    
  public:
    /// Assign volume ID to placement
    void _set_volid( PlacedVolume pv, const std::string& volid, int value );
    /// Set properties for all volumes
    void _set_attr(Volume volume, xml_det_t e)  const;
    
  public:
    /// Default constructor
    DetectorStructure( Detector& dsc, xml_h e, SensitiveDetector sd );
    /// Construct subdetector from Geant3 geometry
    DetElement construct();
    /// Add sub-branch of DetElements to subdetector
    void add_children(DetElement parent, xml_det_t e);
  };
}
#endif // DDCORE_XML_DETECTORSTRUCTURE_H
