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
#ifndef DD4HEP_DDG4_EVENTPARAMETERS_H
#define DD4HEP_DDG4_EVENTPARAMETERS_H

#include <map>
#include <string>
#include <vector>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Event extension to pass input event data to output event
    /**
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class EventParameters  {
    protected:
      int                                             m_runNumber = -1;
      int                                             m_eventNumber = -1;
      std::map<std::string, std::vector<int>>         m_intValues {};
      std::map<std::string, std::vector<float>>       m_fltValues {};
      std::map<std::string, std::vector<std::string>> m_strValues {};

    public:
      /// Initializing constructor
      EventParameters() = default;
      /// Default destructor
      ~EventParameters() = default;

      /// Set the event parameters
      void setRunNumber(int runNumber);
      void setEventNumber(int eventNumber);
      /// Get the run number
      int runNumber() const { return m_runNumber; }
      /// Get the event number
      int eventNumber() const { return m_eventNumber; }

      /// Copy the parameters from source
      template <class T> void ingestParameters(T const& source);
      /// Put parameters into destination
      template <class T> void extractParameters(T& destination);

      /// Get the int event parameters
      auto const& intParameters() const { return m_intValues; }
      /// Get the float event parameters
      auto const& fltParameters() const { return m_fltValues; }
      /// Get the string event parameters
      auto const& strParameters() const { return m_strValues; }

    };

  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif  /* DD4HEP_DDG4_EVENTPARAMETERS_H  */
