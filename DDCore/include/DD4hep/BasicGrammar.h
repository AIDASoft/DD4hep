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
#ifndef DD4HEP_DDG4_BASICGRAMMAR_H
#define DD4HEP_DDG4_BASICGRAMMAR_H

// C/C++ include files
#include <string>
#include <typeinfo>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Base class describing string evaluation to C++ objects using boost::spirit
  /**
   *   Grammar object handle the boost::spirit conversion between strings and numeric
   *   values/objects. Numeric objects could be atomic (int, long, float, double, etc)
   *   or complex (vector<int>, vector<float>...). This way e.g. a vector<int> may
   *   be converted into a list<double>. The conversion though requires an intermediate
   *   string representation. For this reason the conversion mechanism is relatively
   *   slow and hence should not be used inside number-crunching algorithms.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    13.08.2013
   *   \ingroup DD4HEP
   */
  class BasicGrammar {
  public:
    /// Default constructor
    BasicGrammar();
    /// Default destructor
    virtual ~BasicGrammar();

    /// Instance factory
    template <typename TYPE> static const BasicGrammar& instance();
    /// Error callback on invalid conversion
    static void invalidConversion(const std::type_info& from, const std::type_info& to);
    /// Error callback on invalid conversion
    static void invalidConversion(const std::string& value, const std::type_info& to);
    /// Access to the type information
    virtual const std::type_info& type() const = 0;
    /// Access to the type information name
    virtual const std::string& type_name() const = 0;    
    /// Access the object size (sizeof operator)
    virtual size_t sizeOf() const = 0;
    /// Serialize an opaque value to a string
    virtual std::string str(const void* ptr) const = 0;
    /// Set value from serialized string. On successful data conversion TRUE is returned.
    virtual bool fromString(void* ptr, const std::string& value) const = 0;
  };
}      // End namespace dd4hep

#endif  /* DD4HEP_DDG4_BASICGRAMMAR_H */
