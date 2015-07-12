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

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDCond/ConditionsTags.h"
#include "DDCond/ConditionsHandler.h"
#include "DDCond/ConditionsTransaction.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Default constructor
ConditionsTransaction::ConditionsTransaction(LCDD& l, const XML::Handle_t& e) : lcdd(l) {
  flag = e.hasChild(_U(close_transaction));
  /// First check if a transaction is to be opened
  m_handler = lcdd.extension<Geometry::ConditionsHandler>();
  m_handler->addRef();
  if ( e.hasChild(_U(open_transaction)) )  {
    m_handler->openTransaction();
  }
}
/// Default destructor
ConditionsTransaction::~ConditionsTransaction()   {
  /// Last check if a transaction is to be closed
  if ( flag ) {
    lcdd.extension<Geometry::ConditionsHandler>()->closeTransaction();
  }
  m_handler->release();
}
