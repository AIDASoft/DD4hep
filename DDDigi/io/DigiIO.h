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
    public:
      /// Default constructor
      data_io() = default;
      /// Default destructor
      ~data_io() = default;

      template <typename FIRST, typename SECOND> static
      void _to_edm4hep(const FIRST& first, SECOND second);

      template <typename FIRST, typename SECOND> static
      void _to_edm4hep(const FIRST& first, SECOND& second, int hit_type);

      template <typename FIRST, typename SECOND> static
	void _to_edm4hep(const FIRST& cont, SECOND coll, int hit_type);

      template <typename FIRST, typename SECOND, typename THIRD> static
      void _to_edm4hep(const FIRST& first, const SECOND& second, THIRD& third, int hit_type);

      template <typename FIRST, typename SECOND, typename THIRD> static
      void _to_digi(FIRST first, const SECOND& second, THIRD& third);

      template <typename FIRST, typename SECOND, typename PREDICATE> static
      void _to_digi_if(const FIRST& first, SECOND& second, const PREDICATE& pred);
    };

    /// Structure definitions for template specializations
    struct ddg4_input;
    struct digi_input;
    struct edm4hep_input;
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIIO_H
