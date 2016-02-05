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
#include "DD4hep/DetectorSelector.h"
#include "DD4hep/LCDD.h"

using namespace std;
using namespace DD4hep::Geometry;

/// Access a set of subdetectors according to the sensitive type.
const DetectorSelector::Result& 
DetectorSelector::detectors(const string& type)
{
  return lcdd.detectors(type);
}

/// Access a set of subdetectors according to several sensitive types.
DetectorSelector::Result
DetectorSelector::detectors(const string& type1,
                            const string& type2,
                            const string& type3,
                            const string& type4,
                            const string& type5)  {
  const string* types[] = { &type1, &type2, &type3, &type4, &type5 };
  Result result;
  for(size_t i=0; i<sizeof(types)/sizeof(types[0]); ++i)  {
    try  {
      if ( !types[i]->empty() )  {
        const vector<DetElement>& v = lcdd.detectors(*(types[i]));
        result.insert(result.end(),v.begin(),v.end());
      }
    }
    catch(...)   {}
  }
  return result;
}

/** return a vector with all detectors that have all the type properties in
 *  includeFlag set but none of the properties given in excludeFlag
 */
DetectorSelector::Result
DetectorSelector::detectors(unsigned int includeFlag, unsigned int excludeFlag ) const  {
  Result result;
  const LCDD::HandleMap& entries = lcdd.detectors();
  result.reserve( entries.size() ) ;
  lcdd.detectors(""); // Just to ensure the geometry is closed....
  for(LCDD::HandleMap::const_iterator i=entries.begin(); i!=entries.end(); ++i)   {
    DetElement det((*i).second);
    if ( det.parent().isValid() )  { // Exclude 'world'
      //fixme: what to do with compounds - add their daughters  ?
      // ...
      if( ( det.typeFlag() &  includeFlag ) == includeFlag &&
          ( det.typeFlag() &  excludeFlag ) ==  0 )
        result.push_back( det ) ;
    }
  }
  return result;
}
