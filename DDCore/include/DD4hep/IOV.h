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
#ifndef DD4HEP_CONDITIONS_IOV_H
#define DD4HEP_CONDITIONS_IOV_H

// C/C++ include files
#include <string>
#include <algorithm>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class IOVType;
  class IOV;

  /// Class describing the interval of validty type
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class IOVType   {
  public:
    enum { UNKNOWN_IOV = ~0x0 } _IOVTypes;
    /// integer identifier ised internally
    unsigned int type = UNKNOWN_IOV;
    /// String name
    std::string  name;
    /// Standard Constructor
    IOVType() = default;
    /// Standard Destructor
    ~IOVType() = default;
    /// Copy constructor
    IOVType(const IOVType& copy) : type(copy.type), name(copy.name) {}
    /// Assignment operator
    IOVType& operator=(const IOVType& copy);
    /// Conversion to string
    std::string str() const;
  };

  /// Class describing the interval of validty
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_CONDITIONS
   */
  class IOV   {
  private:
    /// Initializing constructor: Does not set reference to IOVType !
    explicit IOV() = delete;
  public:
    /// Key definition
    typedef long Key_first_type;
    typedef long Key_second_type;
    typedef std::pair<Key_first_type,Key_second_type> Key;

    const IOVType* iovType;
    Key            keyData;
    int            optData;
    /// IOV buffer type: Must be a bitmap!
    unsigned int   type;

    /// Initializing constructor
    explicit IOV(const IOVType* typ);
    /// Specialized copy constructor for range IOVs
    explicit IOV(const IOVType* typ, const Key& key);
    /// Specialized copy constructor for discrete IOVs
    explicit IOV(const IOVType* typ, Key_first_type iov_value);
    /// Copy constructor
    IOV(const IOV& copy) = default;
    /// Standard Destructor
    ~IOV() = default;
    /// Assignment operator
    IOV& operator=(const IOV& c) = default;
    /// Move the data content: 'from' will be reset to NULL
    void move(IOV& from);
    /// Create string representation of the IOV
    std::string str() const;
    /// Check if the IOV corresponds to a range
    bool has_range() const             {  return keyData.first != keyData.second;  }
    /// Check if the IOV corresponds to a range
    bool is_discrete() const           {  return keyData.first == keyData.second;  }
    /// Get the local key of the IOV
    Key  key() const                   {  return keyData;                          }

    /// Set discrete IOV value
    void set(const Key& value);
    /// Set discrete IOV value
    void set(Key_first_type value);
    /// Set range IOV value
    void set(Key_first_type val_1, Key_second_type val_2);
    /// Set keys to unphysical values (LONG_MAX, LONG_MIN)
    IOV& reset();
    /// Invert the key values (first=second and second=first)
    IOV& invert();
    /// Set the intersection of this IOV with the argument IOV
    void iov_intersection(const IOV& comparator);
    /// Set the intersection of this IOV with the argument IOV
    void iov_intersection(const IOV::Key& comparator);
    /// Set the union of this IOV with the argument IOV
    void iov_union(const IOV& comparator);
    /// Set the union of this IOV with the argument IOV
    void iov_union(const IOV::Key& comparator);

    /// Check for validity containment
    /** Check if the caller 'iov' is of the same type and the range 
     *  is fully conained by the caller.
     */
    bool contains(const IOV& iov)  const;
    /// Check if 2 IOV objects are of the same type
    static bool same_type(const IOV& iov, const IOV& test)           {
      unsigned int typ1 = iov.iovType ? iov.iovType->type : iov.type;
      unsigned int typ2 = test.iovType ? test.iovType->type : test.type;
      return typ1 == typ2;
    }
    /// Check if IOV 'test' is fully contained in IOV 'key'
    static bool key_is_contained(const Key& key, const Key& test)
    {   return key.first >= test.first && key.second <= test.second;      }
    /// Same as above, but reverse logic. Gives sometimes more understandable logic.
    static bool key_contains_range(const Key& key, const Key& test)
    {   return key.first <= test.first && key.second >= test.second;      }
    /// Check if IOV 'test' has an overlap on the lower interval edge with IOV 'key'
    static bool key_overlaps_lower_end(const Key& key, const Key& test)         
    {   return key.first <= test.second && key.first >= test.first;       }
    /// Check if IOV 'test' has an overlap on the upper interval edge with IOV 'key'
    static bool key_overlaps_higher_end(const Key& key, const Key& test)         
    {   return key.second >= test.first && key.second <= test.second;     }
    /// Check if IOV 'test' has an overlap on the upper interval edge with IOV 'key'
    static bool key_partially_contained(const Key& key, const Key& test)         
    {   
      return 
        (test.first <= key.first  && key.second   >= test.second) || // test fully contained in key
        (test.first <= key.first  && key.first    <= test.second) || // test overlaps left edge of key
        (test.first <= key.second && key.second   <= test.second);   // test overlaps right edge of key
    }
    /// Check if IOV 'test' is of same type and is fully contained in iov
    static bool full_match(const IOV& iov, const IOV& test)
    {   return same_type(iov,test) && key_is_contained(iov.keyData,test.keyData);      }
    /// Check if IOV 'test' is of same type and is at least partially contained in iov
    static bool partial_match(const IOV& iov, const IOV& test)
    {   return same_type(iov,test) && key_partially_contained(iov.keyData,test.keyData);      }
  };
    
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_CONDITIONS_IOV_H        */
