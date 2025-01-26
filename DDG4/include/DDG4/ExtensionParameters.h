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
#ifndef DDG4_EXTENSIONPARAMETERS_H
#define DDG4_EXTENSIONPARAMETERS_H

#include <map>
#include <string>
#include <vector>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Extension to pass input data to output data
    /**
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class ExtensionParameters {
    protected:
      std::map<std::string, std::vector<int>>         m_intValues {};
      std::map<std::string, std::vector<float>>       m_fltValues {};
      std::map<std::string, std::vector<std::string>> m_strValues {};
      std::map<std::string, std::vector<double>>      m_dblValues {};

    public:
      /// Get the int parameters
      auto const& intParameters() const { return m_intValues; }
      /// Get the float parameters
      auto const& fltParameters() const { return m_fltValues; }
      /// Get the string parameters
      auto const& strParameters() const { return m_strValues; }
      /// Get the double parameters
      auto const& dblParameters() const { return m_dblValues; }

    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_EXTENSIONPARAMETERS_H
