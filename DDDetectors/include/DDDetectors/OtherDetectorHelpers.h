//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
#ifndef Other_Helpers_hh
#define Other_Helpers_hh 1

#include "DD4hep/Printout.h"

#include <iostream>
#include <map>
#include <stdexcept>

namespace ODH {//OtherDetectorHelpers

  typedef enum { // These constants are also used in the MySQL database:
    kCenter                     = 0, // centered on the z-axis
    kUpstream                   = 1, // on the upstream branch, rotated by half the crossing angle
    kDnstream                   = 2, // on the downstream branch, rotated by half the crossing angle
    kPunchedCenter              = 3, // centered, with one or two inner holes
    kPunchedUpstream            = 4, // on the upstream branch, with two inner holes
    kPunchedDnstream            = 5, // on the downstrem branch, with two inner holes
    kUpstreamClippedFront       = 6, // upstream, with the front face parallel to the xy-plane
    kDnstreamClippedFront       = 7, // downstream, with the front face parallel to the xy-plane
    kUpstreamClippedRear        = 8, // upstream, with the rear face parallel to the xy-plane
    kDnstreamClippedRear        = 9, // downstream, with the rear face parallel to the xy-plane
    kUpstreamClippedBoth        = 10, // upstream, with both faces parallel to the xy-plane
    kDnstreamClippedBoth        = 11, // downstream, with both faces parallel to the xy-plane
    kUpstreamSlicedFront        = 12, // upstream, with the front face parallel to a tilted piece
    kDnstreamSlicedFront        = 13, // downstream, with the front face parallel to a tilted piece
    kUpstreamSlicedRear         = 14, // upstream, with the rear face parallel to a tilted piece
    kDnstreamSlicedRear         = 15, // downstream, with the rear face parallel to a tilted piece
    kUpstreamSlicedBoth         = 16, // upstream, with both faces parallel to a tilted piece
    kDnstreamSlicedBoth         = 17 // downstream, with both faces parallel to a tilted piece
  } ECrossType;



  static ECrossType getCrossType( std::string const & type) {

    std::map< std::string, ODH::ECrossType > CrossTypes;
    CrossTypes["Center"]                = ODH::kCenter               ;
    CrossTypes["Upstream"]              = ODH::kUpstream             ;
    CrossTypes["Dnstream"]              = ODH::kDnstream             ;
    CrossTypes["PunchedCenter"]         = ODH::kPunchedCenter        ;
    CrossTypes["PunchedUpstream"]       = ODH::kPunchedUpstream      ;
    CrossTypes["PunchedDnstream"]       = ODH::kPunchedDnstream      ;
    CrossTypes["UpstreamClippedFront"]  = ODH::kUpstreamClippedFront ;
    CrossTypes["DnstreamClippedFront"]  = ODH::kDnstreamClippedFront ;
    CrossTypes["UpstreamClippedRear"]   = ODH::kUpstreamClippedRear  ;
    CrossTypes["DnstreamClippedRear"]   = ODH::kDnstreamClippedRear  ;
    CrossTypes["UpstreamClippedBoth"]   = ODH::kUpstreamClippedBoth  ;
    CrossTypes["DnstreamClippedBoth"]   = ODH::kDnstreamClippedBoth  ;
    CrossTypes["UpstreamSlicedFront"]   = ODH::kUpstreamSlicedFront  ;
    CrossTypes["DnstreamSlicedFront"]   = ODH::kDnstreamSlicedFront  ;
    CrossTypes["UpstreamSlicedRear"]    = ODH::kUpstreamSlicedRear   ;
    CrossTypes["DnstreamSlicedRear"]    = ODH::kDnstreamSlicedRear   ;
    CrossTypes["UpstreamSlicedBoth"]    = ODH::kUpstreamSlicedBoth   ;
    CrossTypes["DnstreamSlicedBoth"]    = ODH::kDnstreamSlicedBoth   ;

    std::map < std::string, ODH::ECrossType>::const_iterator ct = CrossTypes.find(type);
    if ( ct == CrossTypes.end() ) {
      throw std::runtime_error("Unknown Crossing Type for this geometry");
    }
    return ct->second;
  }

  static bool checkForSensibleGeometry(double crossingAngle, ECrossType crossType) {
    if (crossingAngle == 0 && crossType != kCenter) {
      printout(dd4hep::ERROR, "Mask/Beampip", "You are trying to build a crossing geometry without a crossing angle.\n" );
      printout(dd4hep::ERROR, "Mask/Beampip", "This is probably not what you want - better check your geometry data!");
      return false; // premature exit, dd4hep will abort now
    }
    return true;
  }


  static double getCurrentAngle( double crossingAngle, ECrossType crossType ) {
    double tmpAngle;
    switch (crossType) {
    case kUpstream:
    case kPunchedUpstream:
    case kUpstreamClippedFront:
    case kUpstreamClippedRear:
    case kUpstreamClippedBoth:
    case kUpstreamSlicedFront:
    case kUpstreamSlicedRear:
    case kUpstreamSlicedBoth:
      tmpAngle = -crossingAngle; break;
    case kDnstream:
    case kPunchedDnstream:
    case kDnstreamClippedFront:
    case kDnstreamClippedRear:
    case kDnstreamClippedBoth:
    case kDnstreamSlicedFront:
    case kDnstreamSlicedRear:
    case kDnstreamSlicedBoth:
      tmpAngle = +crossingAngle; break;
    default:
      tmpAngle = 0; break;
    }

    return tmpAngle;
  }

}//namespace

#endif // Other_Helpers_hh
