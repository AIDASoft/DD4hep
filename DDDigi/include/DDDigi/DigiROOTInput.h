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

/// Forward declarations
class TBranch;

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

    public:
      /// Helper classes
      class internals_t;
      class inputsource_t;

      class work_t   {
      public:
	DataSegment& input_segment;
	Key          input_key;
	TBranch&     branch;
	TClass&      cl;
      };

    protected:
      /// Property: Name of the tree to connect to
      std::string                    m_tree_name   { };
      /// Property: Container names to be loaded
      std::vector<std::string>       m_containers  { };
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

      /// Access to tree name containing data
      const std::string& tree_name()  const   {
	return m_tree_name;
      }
      /// Access to container names containing data
      const std::vector<std::string>& container_names()  const   {
	return m_containers;
      }

      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const = 0;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIROOTINPUT_H
