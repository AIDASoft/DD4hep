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
//
// NOTE:
//
// This is an internal include file. It should only be included to 
// instantiate code. Otherwise the Alignments include file should be
// sufficient for all practical purposes.
//
//==========================================================================
#ifndef DD4HEP_ALIGNMENTS_ALIGNMENTSINTERNA_H
#define DD4HEP_ALIGNMENTS_ALIGNMENTSINTERNA_H

// Framework include files
#include "DD4hep/IOV.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignments part of the AIDA detector description toolkit
  namespace Alignments   {

    // Forward declarations
    class AlignmentsPool;
    class Alignment;

    /// The data class behind a alignments container handle.
    /**
     *  See AlignmentsInterna.cpp for the implementation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class AlignmentsLoader  {
    protected:
      /// Forward defintion of the key type
      typedef Alignment::key_type              key_type;
      /// Forward definition of the iov type
      typedef Alignment::iov_type              iov_type;
      /// Protected destructor
      virtual ~AlignmentsLoader();
    public:
      /// Addreference count. Use object
      virtual void addRef() = 0;
      /// Release object. The client may not use any reference any further.
      virtual void release() = 0;
      /// Access the alignments loading mechanism
      virtual Alignment get(key_type key, const iov_type& iov) = 0;
      /// Access the alignments loading mechanism. Only alignments in the user pool will be accessed.
      virtual Alignment get(key_type key, const UserPool& pool) = 0;
    };

    /// Alignments internal namespace declaration
    /** Internally defined datastructures are not presented to the
     *  user directly, but are used by dedicated views.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    namespace Interna {

      // Forward declarations
      class AlignmentContainer;
      class AlignmentConditionObject;
      
      /// The data class behind a alignments handle.
      /**
       *  See AlignmentsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_ALIGNMENTS
       */
      class AlignmentConditionObject
        : public Conditions::Interna::ConditionObject, public AlignmentData
      {
      public:
        // Make the conditions object type local!
        typedef Conditions::Interna::ConditionObject ConditionObject;
        /// Standard constructor
        AlignmentConditionObject(const std::string& nam,const std::string& tit="");
        /// Standard Destructor
        virtual ~AlignmentConditionObject();
        /// Clear data
        void clear();
      };

      /// The data class behind a alignments container handle.
      /**
       *  See AlignmentsInterna.cpp for the implementation.
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_ALIGNMENTS
       */
      class AlignmentContainer : public NamedObject {
      public:
        /// Forward defintion of the key type
        typedef Alignment::key_type              key_type;
        /// Forward definition of the iov type
        typedef Alignment::iov_type              iov_type;
        /// Forward definition of the mapping type
        typedef std::pair<key_type, std::string> key_value;
        /// Definition of the keys
        typedef std::map<key_type, key_value>    Keys;

      public:
        /// Standard constructor
        AlignmentContainer(Geometry::DetElementObject* parent);
        /// Default destructor
        virtual ~AlignmentContainer();

#ifdef __CINT__
        Handle<NamedObject> detector;
#else
        /// The hosting detector element
        DetElement detector;	

        /// Access to alignment objects by key and IOV. 
        Alignment get(const std::string& alignment_key, const iov_type& iov);

        /// Access to alignment objects directly by their hash key. 
        Alignment get(key_type hash_key, const iov_type& iov);

        /// Access to alignment objects. Only alignments in the pool are accessed.
        Alignment get(const std::string& alignment_key, const UserPool& iov);

        /// Access to alignment objects. Only alignments in the pool are accessed.
        Alignment get(key_type alignment_key, const UserPool& iov);
#endif
      public:
        /// Known keys of alignments in this container
        Keys       keys;
        /// Add a new key to the alignments access map
        void addKey(const std::string& key_value);
        /// Add a new key to the alignments access map: Allow for alias if key_val != data_val
        void addKey(const std::string& key_value, const std::string& data_value);
      };

    } /* End namespace Interna    */

  } /* End namespace Alignments             */
} /* End namespace DD4hep                   */

#endif    /* DD4HEP_ALIGNMENTS_ALIGNMENTSINTERNA_H    */
