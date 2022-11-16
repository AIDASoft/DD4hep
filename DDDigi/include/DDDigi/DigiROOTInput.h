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
#ifndef DDDIGI_DIGIROOTINPUT_H
#define DDDIGI_DIGIROOTINPUT_H

/// Framework include files
#include <DDDigi/DigiInputAction.h>

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiROOTInput;

    /// Base class for input actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiROOTInput : public DigiInputAction {

    protected:
      /// Helper classes
      class internals_t;
      /// Property: Name of the tree to connect to
      std::string                    m_tree_name   { };
      /// Property: Container names to be loaded
      std::vector<std::string>       m_containers  { };
      /// Property: Segment name to place input data default: inputs
      std::string                    m_location    { };

      /// Current input id
      mutable int                    m_curr_input  { 0 };
      /// Connection parameters to the "current" input source
      mutable std::unique_ptr<internals_t> imp;

      /// Open new input file
      void open_new_file()  const;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiROOTInput);

    public:
      /// Standard constructor
      DigiROOTInput(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiROOTInput();
      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIROOTINPUT_H
