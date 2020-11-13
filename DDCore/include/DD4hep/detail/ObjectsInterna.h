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
#ifndef DD4HEP_DETAIL_OBJECTSINTERNA_H
#define DD4HEP_DETAIL_OBJECTSINTERNA_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/NamedObject.h"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/Segmentations.h"
#include "DD4hep/BitFieldCoder.h"

// C/C++ include files
#include <set>

// Forward declarations
class TColor;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Concrete object implementation for the Header handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class HeaderObject: public NamedObject {
  public:
    std::string url;
    std::string author;
    std::string status;
    std::string version;
    std::string comment;
    /// Standard constructor
    HeaderObject();
    /// Default destructor
    virtual ~HeaderObject();
  private:
    /// Private copy constructor
    HeaderObject(const HeaderObject&) : NamedObject() {}
    /// Private assignment operator
    HeaderObject& operator=(const HeaderObject&) { return *this; }
  };

  /// Concrete object implementation for the Constant handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class ConstantObject: public NamedObject {
  public:
    /// Constant type
    std::string dataType;
    /// Default constructor for ROOT persistency
    ConstantObject();
    /// Standard constructor
    ConstantObject(const std::string& nam, const std::string& val, const std::string& typ);
    /// Default destructor
    virtual ~ConstantObject();
  private:
    /// Private copy constructor
    ConstantObject(const ConstantObject&) : NamedObject() {}
    /// Private assignment operator
    ConstantObject& operator=(const ConstantObject&) { return *this; }
  };

  /// Concrete object implementation of the VisAttr Handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class VisAttrObject: public NamedObject {
  public:
    unsigned long magic;
    TColor*       color   = nullptr;
    TColor*       colortr = nullptr;
    float         alpha   = 0;
    unsigned char drawingStyle  = VisAttr::SOLID;
    unsigned char lineStyle     = VisAttr::SOLID;
    unsigned char showDaughters = true;
    unsigned char visible       = true;
    /// Standard constructor
    VisAttrObject();
    /// Default destructor
    virtual ~VisAttrObject();
  };

  /// Concrete object implementation of the Region Handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class RegionObject: public NamedObject {
  public:
    unsigned long magic;
    double        threshold         = 10.0;
    double        cut               = 10.0;
    bool          store_secondaries = false;
    bool          use_default_cut   = true;
    bool          was_threshold_set = false;
    /// References to user limits
    std::vector<std::string> user_limits;
    /// Standard constructor
    RegionObject();
    /// Default destructor
    virtual ~RegionObject();
  };

  /// Concrete object implementation of the LimitSet Handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CORE
   */
  class LimitSetObject: public NamedObject  {
  public:
    /// Iterator definitions
    typedef std::set<Limit>::iterator       iterator;
    typedef std::set<Limit>::const_iterator const_iterator;
  public:
    /// Particle specific limits
    std::set<Limit> limits;
    /// Particle specific production cuts
    std::set<Limit> cuts;
  public:
    /// Standard constructor
    LimitSetObject();
    /// Default destructor
    virtual ~LimitSetObject();
  };

  /// Definition of the HitCollection parameters used by the Readout
  /**
   *   \author  M.Frank
   *   \version 1.0
   *   \ingroup DD4HEP_CORE
   */
  class HitCollection   {
  public:
    /// Hit collection name
    std::string name;
    /// Discriminator key name from the <id/> string
    std::string key;
    /// Range values of the key is not empty.
    long key_min, key_max;
    /// Default constructor
    HitCollection() : key_min(~0x0), key_max(~0x0) {}
    /// Copy constructor
    HitCollection(const HitCollection& c);
    /// Initializing constructor
    HitCollection(const std::string& name, const std::string& key="",long k_min=~0x0, long kmax=~0x0);
    /// Assignment operator
    HitCollection& operator=(const HitCollection& c);
  };

  /// Concrete object implementation of the Readout Handle
  /**
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_CORE
   */
  class ReadoutObject: public NamedObject {
  public:
    /// Handle to the readout segmentation
    Segmentation  segmentation;  //! not ROOT-persistent
    /// Handle to the volume
    Volume        readoutWorld;
    /// Handle to the field descriptor
    IDDescriptor  id;
    /// Hit collection container (if defined)
    std::vector<HitCollection> hits;
    /// Standard constructor
    ReadoutObject();
    /// Default destructor
    virtual ~ReadoutObject();
  };

  /// Concrete object implementation of the IDDescriptorObject Handle
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \date    2012/07/31
   *  \ingroup DD4HEP_CORE
   */
  class IDDescriptorObject: public NamedObject {
  public:
    typedef std::vector<std::pair<std::string, const BitFieldElement*> > FieldMap;
    typedef std::vector<std::pair<size_t, std::string> >         FieldIDs;
    /// Map of id-fields in the descriptor
    FieldMap      fieldMap;
    /// String map of id descriptors
    FieldIDs      fieldIDs;
    /// Decoder object
    BitFieldCoder decoder;
    
    /// The description string to build the bit-field descriptors.
    std::string description;
    /// Default constructor
    IDDescriptorObject();
    /// Standard constructor
    IDDescriptorObject(const std::string& initString);
    /// Default destructor
    virtual ~IDDescriptorObject();
#if 0
#ifndef __CINT__
    /// Access to the field container of the BitFieldCoder
    const std::vector<BitFieldElement*>& fields() const {
      return decoder.fields();
    }
#endif
#endif
  };
}      /* End namespace dd4hep            */
#endif // DD4HEP_DETAIL_OBJECTSINTERNA_H
