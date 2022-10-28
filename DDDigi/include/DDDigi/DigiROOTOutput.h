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
#ifndef DDDIGI_DIGIROOTOUTPUT_H
#define DDDIGI_DIGIROOTOUTPUT_H

/// Framework include files
#include <DDDigi/DigiOutputAction.h>

/// C/C++ include files
#include <memory>

/// Forward declarations
class TFile;
class TTree;
class TBranch;


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiROOTOutput;

    /// Base class for output actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiROOTOutput : public DigiOutputAction {

    protected:
      /// Helper classes
      class internals_t;
      /// Property: Name of the tree to connect to
      std::string m_tree_name       { };
      /// Property: Segment name to place output data default: outputs
      std::string m_location        { };
      /// Property: name of the event tree
      std::string m_section         { };
      /// Property: File size limit
      std::size_t m_max_file_size   { std::numeric_limits<std::size_t>::max() };
      /// Property: vector with disabled collections
      std::vector<std::string> m_disabledCollections { };
      /// Property: vector with disabled collections
      bool  m_disableParticles      { false };

      /// Connection parameters to the "current" output source
      std::unique_ptr<internals_t> imp     { };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiROOTOutput);

    public:
      /// Standard constructor
      DigiROOTOutput(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiROOTOutput();
      /// Callback to read event output
      virtual void execute(DigiContext& context)  const override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIROOTOUTPUT_H
