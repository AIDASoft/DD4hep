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

/// C/C++ include files
#include <memory>

/// Forward declarations
class TBranch;
class TClass;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiROOTInput;

    /// Base class for input actions to the digitization using ROOT
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
      class container_t  {
      public:
	Key          key;
	TBranch&     branch;
	TClass&      clazz;
      container_t(Key k, TBranch& b, TClass& c) : key(k), branch(b), clazz(c) {}
      };
      class work_t   {
      public:
	DataSegment& segment;
	container_t& container;
      };


    protected:
      /// Connection parameters to the "current" input source
      mutable std::unique_ptr<internals_t> imp;

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
      /// Callback to handle single branch
      virtual void operator()(DigiContext& context, work_t& work)  const = 0;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIROOTINPUT_H
