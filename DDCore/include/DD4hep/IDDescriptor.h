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
#ifndef DD4HEP_IDDESCRIPTOR_H
#define DD4HEP_IDDESCRIPTOR_H

// Framework include files
#include "DD4hep/Handle.h"
#include "DD4hep/BitFieldCoder.h"

// C++ include files
#include <string>
#include <vector>
#include <map>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class IDDescriptorObject;

  /// Class implementing the ID encoding of the detector response
  /** @class IDDescriptor IDDescriptor.h DDCore/IDDescriptor.h
   *
   *  @author  M.Frank
   *  @version 1.0
   *  @date    2012/07/31
   */
  class IDDescriptor: public Handle<IDDescriptorObject> {
  public:
    typedef BitFieldElement                                    Field;
    typedef std::vector<std::pair<std::string, const Field*> > FieldMap;
    typedef std::vector<std::pair<size_t, std::string> >       FieldIDs;

  public:
    /// Default constructor
    IDDescriptor() = default;
    /// Constructor to be used when reading the already parsed object
    template <typename Q> IDDescriptor(const Handle<Q>& e) : Handle<Object>(e) { }
    /// Initializing constructor
    IDDescriptor(const std::string& name, const std::string& description);
    /// The string description of all fields from the BitField
    std::string fieldDescription() const;
    /// The total number of encoding bits for this descriptor
    unsigned maxBit() const;
    /// Access the field-id container
    const FieldIDs& ids() const;
    /// Access the fieldmap container
    const FieldMap& fields() const;
    /// Get the field descriptor of one field by name
    const BitFieldElement* field(const std::string& field_name) const;
    /// Get the field identifier of one field by name
    size_t fieldID(const std::string& field_name) const;
    /// Get the field descriptor of one field by its identifier
    const BitFieldElement* field(size_t identifier) const;
#ifndef __MAKECINT__
    /// Encode partial volume identifiers to a volumeID.
    static VolumeID encode(const Field* fld, VolumeID value);
    /// Encode a set of volume identifiers (corresponding to this description of course!) to a volumeID.
    VolumeID encode(const std::vector<std::pair<std::string, int> >& ids) const;
    /// Encode a set of volume identifiers to a volumeID with the system ID on the top bits
    VolumeID encode_reverse(const std::vector<std::pair<std::string, int> >& id_vector) const;
    /// Compute the submask for a given set of volume IDs
    VolumeID get_mask(const std::vector<std::pair<std::string, int> >& id_vector) const;
#endif
    /// Decode volume IDs and return filled descriptor with all fields
    void decodeFields(VolumeID vid, std::vector<std::pair<const BitFieldElement*, VolumeID> >& fields)  const;
    /// Decode volume IDs and return string reprensentation for debugging purposes
    std::string str(VolumeID vid)  const;
    /// Decode volume IDs and return string reprensentation for debugging purposes
    std::string str(VolumeID vid, VolumeID mask)  const;
    /// Access string representation
    std::string toString() const;
    /// Access the BitFieldCoder object
    BitFieldCoder* decoder()  const;
    /// Re-build object in place
    void rebuild(const std::string& description);
  };
}         /* End namespace dd4hep          */
#endif // DD4HEP_IDDESCRIPTOR_H
