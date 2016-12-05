//==========================================================================
//  AIDA Detector description implementation for LCD
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
#ifndef DD4HEP_DDCORE_ALIGNEDVOLUMEPRINTER_H
#define DD4HEP_DDCORE_ALIGNEDVOLUMEPRINTER_H

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/AlignmentsProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace Alignments {

    /// Generic Alignments data dumper.
    /**
     *   Please note that the principle of locality applies:
     *   The object is designed for stack allocation and configuration.
     *   It may NOT be shared across threads!
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class AlignedVolumePrinter : public AlignmentsProcessor  {
    public:
      /// Printer name. Want to know who is printing what
      std::string   name;
      /// Printout prefix
      std::string   prefix;
      /// Printout level
      PrintLevel    printLevel;
    protected:
      /// Printout processing and customization flag
      int           m_flag;

    public:
      /// Initializing constructor
      AlignedVolumePrinter(const std::string& prefix="",int flags=0);
      /// Initializing constructor
      AlignedVolumePrinter(UserPool* pool, const std::string& prefix="",int flags=0);
      /// Default destructor
      virtual ~AlignedVolumePrinter() = default;
      /// Set name for printouts
      void setName(const std::string& value)    {  name = value;   }
      /// Set prefix for printouts
      void setPrefix(const std::string& value)  {  prefix = value; }
      /// Callback to output alignments information
      virtual int operator()(Alignment cond);
      /// Container callback for object processing
      virtual int operator()(Container container);
      /// Callback to output alignments information of an entire DetElement
      virtual int processElement(DetElement de);
    };
  }    /* End namespace Alignments              */
}      /* End namespace DD4hep                  */
#endif /* DD4HEP_DDCORE_ALIGNEDVOLUMEPRINTER_H  */
