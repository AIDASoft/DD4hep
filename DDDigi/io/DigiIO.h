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
#ifndef DDDIGI_DIGIIO_H
#define DDDIGI_DIGIIO_H

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/DD4hepUnits.h>
#include <DDDigi/DigiData.h>

// C/C++ include files
#include <any>
#include <array>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {
    // Forward declarations
    class Geant4Particle;
  }

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Auto cast union to read objects from ROOT files
    template <typename T> union input_data  {
      /// Void type
      const void*      m_raw;
      /// Concrete type
      std::vector<T*>* m_data;

      /// Constructor
      input_data(const void* p)   { this->m_raw = p; }
      /// Vector interface: get object
      std::vector<T*>& get()    { 
	if ( this->m_data ) return *(this->m_data);
	throw std::runtime_error("input_data: Invalid data!");
      }
      /// Vector interface: clear items
      void clear()         { if ( this->m_data ) this->m_data->clear(); }
      /// Vector interface: access array size
      std::size_t size()   { return (this->m_data) ? this->m_data->size() : 0UL; }
    };

    /// Generic I/O helper to input/output digi data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    template <typename T> struct data_io   {
      using particle_t       = typename T::particle_type;
      using trackerhit_t     = typename T::input_trackerhit_type;
      using calorimeterhit_t = typename T::input_calorimeterhit_type;
      using pwrap_t          = std::shared_ptr<particle_t>;
      using twrap_t          = std::shared_ptr<trackerhit_t>;
      using cwrap_t          = std::shared_ptr<calorimeterhit_t>;

      /// Default constructor
      data_io() = default;
      /// Default destructor
      ~data_io() = default;

      /// Check if specialization can handle data conversion
      template <typename DATA>
      static bool _can_handle(const DATA& data);

      /// Convert data of particle type
      template <typename CONT> static
      std::vector<const particle_t*> _to_vector(const CONT& cont);

      /// Pre-create container of output data
      template <typename CONT> static
      void _pre_create(CONT* coll, std::size_t n);

      template <typename FIRST, typename SECOND> static
	void _to_edm4hep(const FIRST& cont, SECOND coll);

      template <typename FIRST, typename SECOND> static
	void _to_edm4hep(const FIRST& cont, SECOND coll, int hit_type);

      template <typename FIRST, typename SECOND, typename THIRD> static
	void _to_digi(FIRST first, const SECOND& second, THIRD& third);

      template <typename FIRST, typename SECOND, typename PREDICATE> static
	void _to_digi_if(const FIRST& first, SECOND& second, const PREDICATE& pred);
    };

    /// Structure definitions for DDG4 input data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct ddg4_input  {
      typedef sim::Geant4Particle particle_type;
      struct  input_trackerhit_type {};
      struct  input_calorimeterhit_type {};
    };

  }    // End namespace digi
}      // End namespace dd4hep


#ifdef DD4HEP_USE_EDM4HEP

/// edm4hep include files
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/TrackerHitCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    edm4hep::Vector3d _toVectorD(const Position& ep);
    edm4hep::Vector3f _toVectorF(const Position& ep);

    /// Structure definitions for edm4hep input data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct edm4hep_input  {
      typedef edm4hep::MutableMCParticle particle_type;
      struct  input_trackerhit_type     {};
      struct  input_calorimeterhit_type {};
    };

    struct digi_input  {
      typedef Particle particle_type;
      struct  input_trackerhit_type     {};
      struct  input_calorimeterhit_type {};
    };

    /// Generic I/O helper to output digi data in edm4hep format
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct digi_io  {
    public:
      /// Default constructor
      digi_io() = default;
      template <typename FIRST_TYPE, typename OUTPUT_TYPE> static
	void _to_edm4hep(const FIRST_TYPE& first, OUTPUT_TYPE output);

      template <typename FIRST_TYPE, typename OUTPUT_TYPE> static
	void _to_edm4hep(const FIRST_TYPE& first, int hit_type, OUTPUT_TYPE output);

      template <typename FIRST_TYPE, typename SECOND_TYPE, typename OUTPUT_TYPE> static
	void _to_edm4hep(const FIRST_TYPE& first, const SECOND_TYPE& second, int hit_type, OUTPUT_TYPE output);
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif

#endif // DDDIGI_DIGIIO_H
