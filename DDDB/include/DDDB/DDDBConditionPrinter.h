// $Id$
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
#include "DD4hep/ConditionsData.h"

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
    class ConditionPrinter : public Conditions::Condition::Processor {
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
	/// Printout prefix
	std::string& m_prefix;
      public:

	/// Initializing constructor
        ParamPrinter(std::string& prefix);
	/// Default destructor
	virtual ~ParamPrinter()  {}

	/// Set prefix for prinouts
	void setPrefix(const std::string& value)  {  m_prefix = value; }
	/// Access prefix value
	const std::string& prefix() const         {   return m_prefix; }
	/// Callback to output conditions information
	virtual void operator()(const Conditions::AbstractMap::Params::value_type& obj)  const;
      };

    protected:
      std::string m_prefix;
      ParamPrinter* m_print;
      int    m_flag;

    public:
      typedef Conditions::Condition Cond;

      /// Initializing constructor
      ConditionPrinter(const std::string& prefix="", 
		       int flag=Cond::NO_NAME|Cond::WITH_IOV|Cond::WITH_ADDRESS,
		       ParamPrinter* prt=0);
      /// Set prefix for prinouts
      void setPrefix(const std::string& value)  {  m_prefix = value; }
      /// Callback to output conditions information
      virtual int operator()(Cond cond);
    };

  } /* End namespace DDDB    */
} /* End namespace DD4hep    */

#endif /* DD4HEP_DDDB_DDDBCONDITIONPRINTER_H  */
