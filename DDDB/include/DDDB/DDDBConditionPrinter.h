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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBCONDITIONPRINTER_H
#define DD4HEP_DDDB_DDDBCONDITIONPRINTER_H

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/ConditionsPrinter.h"

using namespace std;
using namespace DD4hep;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace DDDB {

    /// DDDB Conditions data dumper.
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class ConditionPrinter : public Conditions::ConditionsPrinter {
    public:

      /// DDDB Conditions data dumper helper to output parameter maps.
      /**
       *   \author  M.Frank
       *   \version 1.0
       *   \date    31/03/2016
       *   \ingroup DD4HEP_DDDB
       */
      class ParamPrinter {
        
      protected:
        /// Parent object
        ConditionPrinter* m_parent = 0;
      public:
        /// Copy constructor
        ParamPrinter(const ParamPrinter& copy) = default;
        /// Initializing constructor
        ParamPrinter(ConditionPrinter* p);
        /// Default destructor
        virtual ~ParamPrinter() = default;
        /// Assignment operator
        ParamPrinter& operator=(const ParamPrinter& copy) = default;
        /// Callback to output conditions information
        virtual void operator()(const Conditions::AbstractMap::Params::value_type& obj)  const;
      };

    protected:
      friend class ParamPrinter;
      
      /// Sub-printer
      ParamPrinter* m_print = 0;
      /// Printout prefix
      std::string   m_prefix;
      /// Flags to steer output processing
      int           m_flag = 0;
      /// Printout level
      PrintLevel    m_printLevel = INFO;

      /// Counter: number of parameters
      size_t        m_numParam = 0;
      /// Counter: number of conditions
      size_t        m_numCondition = 0;
      /// Counter: number of empty conditions
      size_t        m_numEmptyCondition = 0;
      
    public:
      typedef Conditions::Condition Condition;
      typedef Conditions::Container Container;
      /// No default constructor
      ConditionPrinter() = delete;
      /// No copy constructor
      ConditionPrinter(const ConditionPrinter& copy) = delete;
      /// Initializing constructor
      ConditionPrinter(const std::string& prefix, 
                       int flag=Condition::NO_NAME|Condition::WITH_IOV|Condition::WITH_ADDRESS,
                       ParamPrinter* prt=0);
      /// Default destructor
      virtual ~ConditionPrinter();
      
      /// Set printout level for prinouts
      void setPrintLevel(PrintLevel lvl);
      /// Access the prefix value
      const std::string& prefix() const         { return m_prefix;   }
      /// Set prefix for prinouts
      void setPrefix(const std::string& value)  {  m_prefix = value; }
      /// Callback to output conditions information
      virtual int operator()(Condition condition);
    };

  } /* End namespace DDDB    */
} /* End namespace DD4hep    */

#endif /* DD4HEP_DDDB_DDDBCONDITIONPRINTER_H  */
