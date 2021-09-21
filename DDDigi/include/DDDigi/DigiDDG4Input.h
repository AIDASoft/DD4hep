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
#ifndef DDDIGI_DIGIDDG4INPUT_H
#define DDDIGI_DIGIDDG4INPUT_H

/// Framework include files
#include "DDDigi/DigiInputAction.h"

// Forward declarations
class TFile;
class TTree;
class TBranch;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiDDG4Input;

    /// Base class for input actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDDG4Input : public DigiInputAction {
    protected:
      /// Reference to the current ROOT file to be read
      TFile*                   m_current;
      /// List of input sources to be worked down
      std::vector<std::string> m_todo;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiDDG4Input);

    public:
      /// Standard constructor
      DigiDDG4Input(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiDDG4Input();
      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIDDG4INPUT_H
