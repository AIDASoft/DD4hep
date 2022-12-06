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
#include <DDDigi/DigiEventAction.h>

/// C/C++ include files
#include <mutex>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Forward declarations
    class DigiEvent;
    class DataSegment;

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiStoreDump : public DigiEventAction   {
    protected:
      /// Property: Flag to check history record
      bool m_dump_history     { false };
      /// Property: Data segments to be dumped
      std::vector<std::string>        m_segments   { };
      std::vector<std::string>        m_containers { };
      std::vector<int>                m_masks      { };
      std::vector<Key::itemkey_type>  m_container_items { };

      using records_t = std::vector<std::string>;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiStoreDump);

      /// Default destructor
      virtual ~DigiStoreDump();

      template <typename T> std::string data_header(Key key, const std::string& tag, const T& data)  const;
      template <typename T> records_t dump_history(context_t& context, Key key, const T& container)  const;
      template <typename T> records_t dump_history(context_t& context, Key key, const T& item, std::size_t seq_no)  const;
      template <typename T> records_t dump_deposit_history(context_t& context, Key container_key, const T& container)  const;

      records_t
	dump_particle_history(context_t& context, Key container_key, const ParticleMapping& container)  const;

      /// Dump hit container
      void dump_headers(const std::string& tag, const DigiEvent& event, const DataSegment& segment)  const;

      /// Dump hit container
      void dump_history(context_t& context, const std::string& tag, const DigiEvent& event, const DataSegment& cont)  const;

      /// Initialize the action
      virtual void initialize();
      
    public:
      /// Standard constructor
      DigiStoreDump(const DigiKernel& kernel, const std::string& nam);
      /// Main functional callback
      virtual void execute(context_t& context)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGISTOREDUMP_H
