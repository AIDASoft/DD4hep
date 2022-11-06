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

/// Framework include files
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiContainerProcessor.h>

/// C/C++ include files
#include <iostream>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to move the IP of one single container
    /**
     *  Note: in place movement. Not thread safe for the containers!
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiIPMover : public DigiContainerProcessor  {
    public:

      /// Standard constructor
      DigiIPMover(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
      }

      /// Move IP location of deposits
      template <typename DEPOSITS> std::size_t
      move_deposits(const char* tag, DEPOSITS& cont, const Position& delta, const predicate_t& predicate)  const  {
	info("%s+++ %-32s [%6ld] IP: x:%7.3f y:%7.3f z:%7.3f", 
	     tag, cont.name.c_str(), cont.size(), delta.X(), delta.Y(), delta.Z());
	for( auto& dep : cont )   {
	  if( predicate(dep) )   {
	    dep.second.position += delta;
	  }
	}
	return cont.size();
      }

      /// Move IP location of MC particles
      template <typename PARTICLES> std::size_t
      move_particles(const char* tag, PARTICLES& cont, const Position& delta)  const  {
	info("%s+++ %-32s [%6ld] IP: x:%7.3f y:%7.3f z:%7.3f", 
	     tag, cont.name.c_str(), cont.size(), delta.X(), delta.Y(), delta.Z());
	for( auto& p : cont )   {
	  auto& part = p.second;
	  part.end_position   += delta;
	  part.start_position += delta;
	}
	return cont.size();
      }

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const  override  {
	const char* tag = context.event->id();
	Position  delta = property("interaction_point").value<Position>();
	if ( auto* m = work.get_input<DepositMapping>() )
	  move_deposits(tag, *m, delta, predicate);
	else if ( auto* v = work.get_input<DepositVector>() )
	  move_deposits(tag, *v, delta, predicate);
	else if ( auto* p = work.get_input<ParticleMapping>() )
	  move_particles(tag, *p, delta);
	else
	  except("%s+++ Request to handle unknown data type: %s", tag, work.input_type_name().c_str());
	return;
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiIPMover)
