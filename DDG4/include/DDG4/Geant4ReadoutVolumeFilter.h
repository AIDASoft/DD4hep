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
#ifndef DD4HEP_DDG4_GEANT4READOUTVOLUMEFILTER_H
#define DD4HEP_DDG4_GEANT4READOUTVOLUMEFILTER_H

// Framework include files
#include "DD4hep/Readout.h"
#include "DD4hep/IDDescriptor.h"
#include "DDG4/Geant4SensDetAction.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    /// Default base class for all geant 4 tracking actions.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ReadoutVolumeFilter: public Geant4Filter {
      typedef Geometry::IDDescriptor::Field Field;
      typedef Geometry::Readout             Readout;
      typedef Readout::Collection           Collection;

    protected:
      /// Reference to readout descriptor
      Readout             m_readout;
      /// Collection index
      const Collection*   m_collection;
      /// Bit field value from ID descriptor
      Field               m_key;

    public:
      /// Standard constructor
      Geant4ReadoutVolumeFilter(Geant4Context* context, 
                                const std::string& name, 
                                Readout ro, 
                                const std::string& coll);
      /// Default destructor
      virtual ~Geant4ReadoutVolumeFilter();
      /// Filter action. Return true if hits should be processed
      virtual bool operator()(const G4Step* step) const;
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4READOUTVOLUMEFILTER_H
