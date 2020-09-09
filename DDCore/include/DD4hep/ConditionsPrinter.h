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
#ifndef DD4HEP_CONDITIONSPRINTER_H
#define DD4HEP_CONDITIONSPRINTER_H

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetElement.h"
#include "DD4hep/Conditions.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace cond {

    /// Generic Conditions data dumper.
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
    class ConditionsPrinter  {
    protected:
      /// Internal param printer class
      class ParamPrinter;
      friend class ParamPrinter;

      /// Sub-printer
      ParamPrinter*  m_print = 0;
      /// Conditionsmap to resolve things
      ConditionsMap* mapping = 0;

    protected:
      /// Printout processing and customization flag
      int            m_flag = 0;

    public:
      /** Setup parameters to configure printout */
      /// Printer name. Want to know who is printing what
      std::string    name;
      /// Printout prefix
      std::string    prefix;
      /// Printout level
      PrintLevel     printLevel = INFO;
      /// Line length
      size_t         lineLength = 80;
      /// Counter: number of parameters
      size_t         numParam = 0;
      /// Counter: number of conditions
      mutable size_t numCondition = 0;
      /// Counter: number of empty conditions
      mutable size_t numEmptyCondition = 0;
      /// Flag to print summary
      bool           summary = true;
    public:
      /// Initializing constructor
      ConditionsPrinter(ConditionsMap* m,
                        const std::string& prefix="", 
                        int flag=Condition::NO_NAME|Condition::WITH_IOV|Condition::WITH_ADDRESS);
      /// Default destructor
      virtual ~ConditionsPrinter();
      /// Set name for printouts
      void setName(const std::string& value)            {  name = value;       }
      /// Set prefix for printouts
      void setPrefix(const std::string& value)          {  prefix = value;     }
      /// Callback to output conditions information of an entire DetElement
      virtual int operator()(DetElement de, int level) const;
      /// Callback to output conditions information
      virtual int operator()(Condition condition)  const;
    };
    
  }    /* End namespace cond           */
}      /* End namespace dd4hep               */
#endif // DD4HEP_CONDITIONSPRINTER_H
