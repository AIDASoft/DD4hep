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

// Framework include files
#include <DD4hep/DetectorSelector.h>
#include <DD4hep/Detector.h>

using namespace dd4hep;

/// Access a set of subdetectors according to the sensitive type.
const DetectorSelector::Result& DetectorSelector::detectors(const std::string& type)
{
  return description.detectors(type);
}

/// Access a set of subdetectors according to several sensitive types.
DetectorSelector::Result
DetectorSelector::detectors(const std::string& type1,
                            const std::string& type2,
                            const std::string& type3,
                            const std::string& type4,
                            const std::string& type5)  {
  const std::string* types[] = { &type1, &type2, &type3, &type4, &type5 };
  Result result;
  for( std::size_t i=0; i<sizeof(types)/sizeof(types[0]); ++i )  {
    try  {
      if ( !types[i]->empty() )  {
        const std::vector<DetElement>& v = description.detectors(*(types[i]));
        result.insert(std::end(result), std::begin(v), std::end(v));
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
  const Detector::HandleMap& entries = description.detectors();
  result.reserve( entries.size() ) ;
  description.detectors(""); // Just to ensure the geometry is closed....
  for( const auto& i : entries )  {
    DetElement det(i.second);
    if ( det.parent().isValid() )  { // Exclude 'world'
      //fixme: what to do with compounds - add their daughters  ?
      // ...
      if( ( det.typeFlag() &  includeFlag ) == includeFlag &&
          ( det.typeFlag() &  excludeFlag ) ==  0 )
        result.emplace_back( det ) ;
    }
  }
  return result;
}
