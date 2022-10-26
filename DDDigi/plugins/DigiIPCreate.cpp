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
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiEventAction.h>

/// C/C++ include files
#include <cmath>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiIPCreate : public DigiEventAction  {
      Position  m_offset_ip;
      Position  m_sigma_ip;
      mutable Position m_interaction_point;
    public:
      /// Standard constructor
      DigiIPCreate(const DigiKernel& krnl, const std::string& nam)
	: DigiEventAction(krnl, nam)
      {
	declareProperty("offset_ip", m_offset_ip);
	declareProperty("sigma_ip",  m_sigma_ip);
	declareProperty("interaction_point", m_interaction_point);
      }
      /// Main functional callback
      virtual void execute(DigiContext& context)   const  override final  {
	auto& rndm = context.randomGenerator();
	double theta  = rndm.uniform(0.0, M_PI);       // theta  = [0,pi]
	double phi    = rndm.uniform(0.0, 2.0*M_PI);   // phi    = [0,2*pi]	
	double radius = rndm.uniform(0.0, 1.0);        // radius = [0,1]
	radius = std::sqrt( -std::log(radius) );       // radius in a gaussian distribution

	double st = std::sin(theta);
	double xx = radius * st * std::cos(phi) * m_sigma_ip.X() + m_offset_ip.X();
	double yy = radius * st * std::sin(phi) * m_sigma_ip.Y() + m_offset_ip.Y();
	double zz = radius * std::cos(theta) * m_sigma_ip.Z() + m_offset_ip.Z();
	m_interaction_point = Position(xx, yy, zz);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiIPCreate)
