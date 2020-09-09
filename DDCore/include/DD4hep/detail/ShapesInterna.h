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
#ifndef DD4HEP_DETAIL_SHAPESINTERNA_H
#define DD4HEP_DETAIL_SHAPESINTERNA_H

// Framework include files
#include "DD4hep/Shapes.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete object implementation for the Header handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class TwistedTubeObject: public TGeoTubeSeg {
  private:
    /// Inhibit move constructor
    TwistedTubeObject(TwistedTubeObject&&) = delete;
    /// Inhibit copy constructor
    TwistedTubeObject(const TwistedTubeObject&) = delete;
    /// Inhibit move assignment
    TwistedTubeObject& operator=(TwistedTubeObject&&) = delete;
    /// Inhibit copy assignment
    TwistedTubeObject& operator=(const TwistedTubeObject&) = delete;
  public:
    double fPhiTwist     {0.0};   // Twist angle from -fZHalfLength to fZHalfLength
    double fNegativeEndz {0.0};   // -ve z endplate
    double fPositiveEndz {0.0};   // +ve z endplate
    int    fNsegments    {0};     // Number of segments in totalPhi
    
  public:
    /// Standard constructor
    TwistedTubeObject() = default;
    /// Initializing constructor
    TwistedTubeObject(const char* pName,
                      double  twistedangle,  // Twisted angle
                      double  endinnerrad,   // Inner radius at endcap 
                      double  endouterrad,   // Outer radius at endcap 
                      double  negativeEndz,  // -ve z endplate
                      double  positiveEndz,  // +ve z endplate
                      int     nseg,          // Number of segments in totalPhi
                      double  totphi);       // Total angle of all segments
    /// Default destructor
    virtual ~TwistedTubeObject() = default;
    /// Access twist angle
    double GetPhiTwist    () const { return fPhiTwist;     }
    /// Access the negative z
    double GetNegativeEndZ() const { return fNegativeEndz; }
    /// Access the positive z
    double GetPositiveEndZ() const { return fPositiveEndz; }
    /// Access the number of segments
    int    GetNsegments()    const { return fNsegments;    }
    
    /// in case shape has some negative parameters, these has to be computed in order to fit the mother
    virtual TGeoShape *GetMakeRuntimeShape(TGeoShape* mother, TGeoMatrix* mat) const  override;
    /// print shape parameters
    virtual void InspectShape() const  override;

    ClassDefOverride(TwistedTubeObject,0);
  };
}      /* End namespace dd4hep           */
#endif // DD4HEP_DETAIL_SHAPESINTERNA_H
