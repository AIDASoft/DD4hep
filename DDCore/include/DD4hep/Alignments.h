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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTS_H
#define DD4HEP_ALIGMENTS_ALIGNMENTS_H

// Framework include files
#include "DD4hep/Volumes.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  class IOV;

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    using Geometry::LCDD;
    using Geometry::DetElement;
    using Geometry::PlacedVolume;

    /// Alignments internal namespace
    namespace Interna  {
      /// Forward declarations
      class AlignmentContainer;
      class AlignmentObject;
    }
    class AlignmentsManagerObject;
    class AlignmentsLoader;
    class AlignmentData;
    class Alignment;
    class UserPool;
    class Delta;

    /// Main handle class to hold a TGeo alignment object of type TGeoPhysicalNode
    /**
     *  See the ROOT documentation about the TGeoPhysicalNode for further details:
     *  @see http://root.cern.ch/root/html/TGeoPhysicalNode.html
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_GEOMETRY
     *  \ingroup DD4HEP_ALIGN
     */
    class Alignment : public Handle<AlignmentData>   {
    public:
      /// Forward definition of the base data object containing alignment data
      typedef AlignmentData             Object;
      /// Forward definition of the geometry placement
      typedef Geometry::PlacedVolume    PlacedVolume;
      /// Forward definition of the nodelist leading to the world
      typedef std::vector<PlacedVolume> NodeList;
      /// Forward definition of the alignment delta data
      typedef Alignments::Delta         Delta;
      /// Forward definition of the key type
      typedef unsigned int              key_type;
      /// Forward definition of the iov type
      typedef IOV                       iov_type;

    public:
      /// Default constructor
      Alignment();
      /// Default constructor
      Alignment(Object* p);
      /// Copy constructor
      Alignment(const Alignment& c);
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Alignment(const Handle<Q>& e) : Handle<Object>(e) {}
      /// Object constructor for pure alignment objects
      Alignment(const std::string& name);

      /// Hash code generation from input string
      static key_type hashCode(const char* value);
      /// Hash code generation from input string
      static key_type hashCode(const std::string& value);

      /// Assignment operator
      Alignment& operator=(const Alignment& c);
      /// Data accessor for the use of decorators
      AlignmentData& alignmentData()              {   return (*access()); }
      /// Data accessor for the use of decorators
      const AlignmentData& alignmentData() const  {   return (*access()); }
    };

    /// Default constructor
    inline Alignment::Alignment(Object* p) : Handle<Object>(p)  {}

    /// Copy constructor
    inline Alignment::Alignment(const Alignment& c) : Handle<Object>(c.ptr()) {}

    /// Assignment operator
    inline Alignment& Alignment::operator=(const Alignment& c)  {
      if ( &c != this )  {
        m_element = c.ptr();
      }
      return *this;
    }

    /// Hash code generation from input string
    inline Alignment::key_type Alignment::hashCode(const char* value)
    {   return hash32(value);    }

    /// Hash code generation from input string
    inline Alignment::key_type Alignment::hashCode(const std::string& value)
    {   return hash32(value);    }

    /// Container class for alignment handles aggregated by a detector element
    /**
     *  Note: The alignments container is owner by the detector element
     *        On deletion the detector element will destroy the container
     *        and all associated entries.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class Container : public Handle<Interna::AlignmentContainer> {
    public:
      /// Standard object type
      typedef Interna::AlignmentContainer Object;
      /// Forward definition of the key type
      typedef Alignment::key_type key_type;
      /// Forward definition of the iov type
      typedef Alignment::iov_type iov_type;

    public:
      /// Default constructor
      Container();

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Container& c) : Handle<Object>(c) {}

      /// Constructor to be used when reading the already parsed object
      template <typename Q> Container(const Handle<Q>& e) : Handle<Object>(e) {}

      /// Access the number of conditons keys available for this detector element
      size_t numKeys() const;

      /// Access to alignment objects by key and IOV. 
      Alignment get(const std::string& alignment_key, const iov_type& iov);

      /// Access to alignment objects directly by their hash key. 
      Alignment get(key_type alignment_key, const iov_type& iov);

      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(const std::string& alignment_key, const UserPool& iov);

      /// Access to alignment objects. Only alignments in the pool are accessed.
      Alignment get(key_type alignment_key, const UserPool& iov);
    };

    /// Default constructor
    inline Container::Container() : Handle<Object>() {
    }

  } /* End namespace Aligments               */
} /* End namespace DD4hep                    */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTS_H   */
