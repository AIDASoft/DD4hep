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
#ifndef DDDIGI_DIGISTOREDUMP_H
#define DDDIGI_DIGISTOREDUMP_H

/// Framework include files
#include "DDDigi/DigiEventAction.h"

/// C/C++ include files
#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiStoreDump : public DigiEventAction   {
    protected:
      /// Property: Flag to check history record
      bool m_dump_history     { false };
      /// Property: Data segments to be dumped
      std::vector<std::string> m_data_segments { };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiStoreDump);

      /// Default destructor
      virtual ~DigiStoreDump();

      /// Dump hit container
      template <typename CONTAINER> void dump(const std::string& tag,
					      const DigiEvent& event,
					      const CONTAINER& cont)  const;

    public:
      /// Standard constructor
      DigiStoreDump(const DigiKernel& kernel, const std::string& nam);
      /// Main functional callback
      virtual void execute(DigiContext& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISTOREDUMP_H
