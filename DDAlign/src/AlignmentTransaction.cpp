// $Id: $
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
#include "DDAlign/AlignmentTags.h"
#include "DDAlign/AlignmentCache.h"
#include "DDAlign/AlignmentTransaction.h"

using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Default constructor
AlignmentTransaction::AlignmentTransaction(LCDD& l, const XML::Handle_t& e) : lcdd(l) {
  flag = e.hasChild(_ALU(close_transaction));
  /// First check if a transaction is to be opened
  m_cache = lcdd.extension<Alignments::AlignmentCache>();
  m_cache->addRef();
  if ( e.hasChild(_ALU(open_transaction)) )  {
    m_cache->openTransaction();
  }
}
/// Default destructor
AlignmentTransaction::~AlignmentTransaction()   {
  /// Last check if a transaction is to be closed
  if ( flag ) {
    lcdd.extension<Alignments::AlignmentCache>()->closeTransaction();
  }
  m_cache->release();
}
