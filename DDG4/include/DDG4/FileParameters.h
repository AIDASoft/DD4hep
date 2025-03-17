//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//==========================================================================
#ifndef DDG4_FILEPARAMETERS_H
#define DDG4_FILEPARAMETERS_H

#include <DDG4/ExtensionParameters.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Extension to pass input run data to output run data
    /**
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class FileParameters: public ExtensionParameters {
    public:
      /// Copy the parameters from source
      template <class T> void ingestParameters(T const& source);
      /// Put parameters into destination
      template <class T> void extractParameters(T& destination);
    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_FILEPARAMETERS_H
