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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/detail/ShapesInterna.h"

// C/C++ include files
#include <climits>
#include <iomanip>
#include <cstdio>

using namespace std;
using namespace dd4hep;

ClassImp(dd4hep::TwistedTubeObject)

/// Initializing constructor
TwistedTubeObject::TwistedTubeObject(const char* pName,
                                     double  twistedangle,  // Twisted angle
                                     double  innerrad,      // Inner radius at endcap 
                                     double  outerrad,      // Outer radius at endcap 
                                     double  negativeEndz,  // -ve z endplate
                                     double  positiveEndz,  // +ve z endplate
                                     int     nseg,          // Number of segments in totalPhi
                                     double  totphi)        // Total angle of all segments
  : TGeoTubeSeg(pName, innerrad, outerrad, (-negativeEndz+positiveEndz)/2.0, 0, totphi),
  fPhiTwist(twistedangle), fNegativeEndz(negativeEndz), fPositiveEndz(positiveEndz), fNsegments(nseg)
{
}


/// print shape parameters
void TwistedTubeObject::InspectShape() const    {
   printf("*** Shape TwistedTubeObject %s:  ***\n", GetName());
   printf("    Rmin = %11.5f\n", GetRmin());
   printf("    Rmax = %11.5f\n", GetRmax());
   printf("    dz   = %11.5f\n", GetDz());
   printf("    phi1 = %11.5f\n", GetPhi1());
   printf("    phi2 = %11.5f\n", GetPhi2());
   printf("    negativeEndz = %11.5f\n", GetNegativeEndZ());
   printf("    positiveEndz = %11.5f\n", GetPositiveEndZ());
   printf("    Nsegemnts    = %11.d\n",  GetNsegments());
   printf(" Bounding box:\n");
   TGeoBBox::InspectShape();
}

/// in case shape has some negative parameters, these has to be computed in order to fit the mother

TGeoShape *TwistedTubeObject::GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix * /*mat*/) const
{
   if (!TestShapeBit(kGeoRunTimeShape)) return 0;
   if (!mother->TestShapeBit(kGeoTube)) {
      Error("GetMakeRuntimeShape", "Invalid mother for shape %s", GetName());
      return 0;
   }
   Double_t rmin = fRmin;
   Double_t rmax = fRmax;
   if (fRmin<0)
      rmin = ((TGeoTube*)mother)->GetRmin();
   if ((fRmax<0) || (fRmax<=fRmin))
      rmax = ((TGeoTube*)mother)->GetRmax();
   
   return (new TwistedTubeObject(GetName(), GetPhiTwist(), rmin, rmax,
                                 GetNegativeEndZ(), GetPositiveEndZ(),
                                 GetNsegments(), GetPhi2()));
}
