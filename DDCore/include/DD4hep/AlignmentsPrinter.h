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
#ifndef DD4HEP_DDCORE_ALIGNMENTSPRINTER_H
#define DD4HEP_DDCORE_ALIGNMENTSPRINTER_H

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Alignments.h"

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
    class AlignmentsPrinter /* : public Alignment::Processor , public DetElement::Processor  */ {
    public:
      /// Conditionsmap to resolve things
      ConditionsMap* mapping;
      /// Printer name. Want to know who is printing what
      std::string    name;
      /// Printout prefix
      std::string    prefix;
      /// Printout level
      PrintLevel     printLevel;
    protected:
      /// Printout processing and customization flag
      int            m_flag;

    public:
      /// No default constructor
      AlignmentsPrinter() = delete;
      /// Initializing constructor
      AlignmentsPrinter(ConditionsMap* m, const std::string& prefix="",int flags=0);
      /// Default destructor
      virtual ~AlignmentsPrinter() = default;
      /// Set name for printouts
      void setName(const std::string& value)    {  name = value;   }
      /// Set prefix for printouts
      void setPrefix(const std::string& value)  {  prefix = value; }
      /// Callback to output alignments information of an entire DetElement
      virtual int operator()(DetElement de, int level) const;
      /// Callback to output alignments information
      virtual int operator()(Alignment alignment)  const;
      /// Processing callback to print alignments of a detector element
      //virtual int processElement(DetElement de)  override;
    };

    
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
    class AlignedVolumePrinter : public AlignmentsPrinter {
#if 0
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
#endif
    public:
      /// No default constructor
      AlignedVolumePrinter() = delete;
      /// Initializing constructor
      AlignedVolumePrinter(ConditionsMap* map, const std::string& prefix="",int flags=0);
      /// Default destructor
      virtual ~AlignedVolumePrinter() = default;
      /// Set name for printouts
      //void setName(const std::string& value)    {  name = value;   }
      /// Set prefix for printouts
      //void setPrefix(const std::string& value)  {  prefix = value; }
      /// Callback to output alignments information of an entire DetElement
      virtual int operator()(DetElement de, int level) const override
      { return this->AlignmentsPrinter::operator()(de,level);        }
      /// Callback to output alignments information
      virtual int operator()(Alignment cond)  const  override;
      /// Callback to output alignments information of an entire DetElement
      //virtual int processElement(DetElement de)  override;
    };

    /// Default printout of an alignment entry
    void printAlignment(PrintLevel prt_level, const std::string& prefix, Alignment alignment);

    void printAlignment(PrintLevel lvl, const std::string& prefix,
                        const std::string& opt, DetElement de, Alignment alignment);

    /// Default printout of a detector element entry
    void printElement(PrintLevel prt_level, const std::string& prefix, DetElement element, ConditionsMap& pool);

    /// PrintElement placement with/without alignment applied
    void printElementPlacement(PrintLevel prt_level, const std::string& prefix, DetElement detector, ConditionsMap& pool);

  }    /* End namespace Alignments           */
}      /* End namespace DD4hep               */
#endif /* DD4HEP_DDCORE_ALIGNMENTSPRINTER_H  */
