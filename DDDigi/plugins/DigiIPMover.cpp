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
#include <DDDigi/DigiContainerProcessor.h>

#include <iostream>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiIPMover : public DigiContainerProcessor  {
      DigiAction* m_ip_creator { nullptr };
      std::string m_ip_property;

    public:
      /// Standard constructor
      DigiIPMover(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("ip_property", m_ip_property);
	declareProperty("ip_creator", m_ip_creator);
      }
      /// Move IP location of deposits
      template <typename T> std::size_t move_deposits(T& cont, const Position& delta)  const  {
	info("+++ %-32s [%6ld] IP: x:%7.3f y:%7.3f z:%7.3f", 
	     cont.name.c_str(), cont.size(), delta.X(), delta.Y(), delta.Z());
	for( auto& dep : cont )
	  dep.second.position += delta;
	return cont.size();
      }
      /// Move IP location of MC particles
      template <typename T> std::size_t move_particles(T& cont, const Position& delta)  const  {
	info("+++ %-32s [%6ld] IP: x:%7.3f y:%7.3f z:%7.3f", 
	     cont.name.c_str(), cont.size(), delta.X(), delta.Y(), delta.Z());
	for( auto& p : cont )   {
	  auto& part = p.second;
	  part.end_position   += delta;
	  part.start_position += delta;
	}
	return cont.size();
      }
      /// Main functional callback
      virtual void execute(DigiContext&, work_t& work)  const  override  {
	Position delta;
	if ( m_ip_creator )   {
	  m_ip_creator->property(m_ip_property).value(delta);
	  if ( auto* m = work.get_input<DepositMapping>() )
	    move_deposits(*m, delta);
	  else if ( auto* v = work.get_input<DepositVector>() )
	    move_deposits(*v, delta);
	  else if ( auto* p = work.get_input<ParticleMapping>() )
	    move_particles(*p, delta);
	  else
	    except("Request to handle unknown data type: %s", work.input_type_name().c_str());
	  return;
	}
	except("+++ No IP creator action known. Did you set the properties: ip_creator and ip_property?");
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiIPMover)
