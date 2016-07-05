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

#ifndef DD4hep_GEOMETRY_READOUT_H
#define DD4hep_GEOMETRY_READOUT_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/Segmentations.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    // Forward declarations
    class DetElement;
    class ReadoutObject;
    class HitCollection;

    /// Handle to the implementation of the readout structure of a subdetector
    /**
     *   If there is no explicit hit collection defined, by default one single
     *   hit collection is defined by the name of the readout itself.
     *   If hit collections are defined, ALL must be defined.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \ingroup DD4HEP_GEOMETRY
     */
    class Readout: public Handle<ReadoutObject> {
    public:
      /// Implementation type
      typedef ReadoutObject Object;
      typedef HitCollection Collection;
    public:
      /// Default constructor
      Readout()
        : Handle<Object>() {
      }
      /// Copy Constructor from object
      Readout(const Readout& e)
        : Handle<Object>(e) {
      }
#ifndef __CINT__
      /// Copy Constructor from handle
      Readout(const Handle<ReadoutObject>& e)
        : Handle<Object>(e) {
      }
#endif
      /// Constructor to be used when reading the already parsed object
      template <typename Q> Readout(const Handle<Q>& e)
        : Handle<Object>(e) {
      }
      /// Initializing constructor
      Readout(const std::string& name);
      /// Assignment operator
      Readout& operator=(const Readout& ro)  {
        if ( &ro == this ) return *this;
        m_element = ro.m_element;
        return *this;
      }
      /// Access explicit names of hit collections if present
      std::vector<std::string> collectionNames()  const;
#ifndef __CINT__
      /// Access hit collections if present
      std::vector<const HitCollection*> collections()  const;
#endif
      /// Access number of hit collections
      size_t numCollections() const;
      /// Assign IDDescription to readout structure
      void setIDDescriptor(const Ref_t& spec) const;
      /// Access IDDescription structure
      IDDescriptor idSpec() const;
      /// Assign segmentation structure to readout
      void setSegmentation(const Segmentation& segment) const;
      /// Access segmentation structure
      Segmentation segmentation() const;
    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4hep_GEOMETRY_READOUT_H      */
