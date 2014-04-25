// $Id: Geant4Setup.cpp 578 2013-05-17 22:33:09Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDAlign/AlignmentTags.h"
#include "DDAlign/AlignmentCache.h"
#include "DDAlign/AlignmentTransaction.h"

using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Default constructor
AlignmentTransaction::AlignmentTransaction(LCDD& l, const XML::Handle_t& e) : lcdd(l) {
  flag = e.hasChild(_U(close_transaction));
  /// First check if a transaction is to be opened
  m_cache = lcdd.extension<Geometry::AlignmentCache>();
  m_cache->addRef();
  if ( e.hasChild(_U(open_transaction)) )  {
    m_cache->openTransaction();
  }
}
/// Default destructor
AlignmentTransaction::~AlignmentTransaction()   {
  /// Last check if a transaction is to be closed
  if ( flag ) {
    lcdd.extension<Geometry::AlignmentCache>()->closeTransaction();
  }
  m_cache->release();
}
