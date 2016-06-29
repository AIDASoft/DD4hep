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
#ifndef DD4HEP_DDDB_DDDBCONDITIONDATA_H
#define DD4HEP_DDDB_DDDBCONDITIONDATA_H

// Framework includes
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    // Forward declarations
    class Document;

    /// CondDB reader context to support intervals of validity
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class DDDBConditionData   {
    public:
      typedef Conditions::BlockData Data;
      typedef std::map<std::string, Data> Params;
      Document*     document;
      Params        params;
      int           classID;
      /// Default constructor
      DDDBConditionData();
      /// Default destructor
      virtual ~DDDBConditionData();
    };
  }    /* End namespace DDDB              */
}      /* End namespace DD4hep            */
#endif /* DD4HEP_DDDB_CONDITIONDATA_H     */
