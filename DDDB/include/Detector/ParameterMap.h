//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================
#ifndef DETECTOR_PARAMETERMAP_H 
#define DETECTOR_PARAMETERMAP_H 1

// Framework include files

// C/C++ include files
#include <string>
#include <map>

/// Gaudi namespace declaration
namespace gaudi   {

  /// Generic detector element parameter map to specialize detector elements
  /**
   *  Concurrentcy notice:
   *  Except during filling, which is performed by the framework code,
   *  instances of this class are assumed to the read-only! 
   *  Thread safety hence is no issue.
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class ParameterMap  final  {
  public:
    /// Defintiion of a single parameter
    /**
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class Parameter final {
    public:
      std::string value;
      std::string type;
    public:
      Parameter() = default;
      Parameter(Parameter&& copy) = default;
      Parameter(const Parameter& copy) = default;
      Parameter(const std::string& v, const std::string& t)
        : value(v), type(t) {}
      Parameter& operator=(const Parameter& copy) = default;
      bool operator==(const Parameter& copy) const
      { return value == copy.value && type == copy.type; }
      /// Type dependent accessor to a named parameter
      template <typename T> T get()   const;
    };

    typedef std::map<std::string,Parameter> Parameters;

  protected:
    /// The parameter map
    Parameters m_params;

    /// Access single parameter
    const Parameter& _param(const std::string& nam)   const;
  public:
    /// Defautl constructor
    ParameterMap() = default;
    /// Copy constructor
    ParameterMap(const ParameterMap& copy) = default;
    /// Default destructor
    ~ParameterMap() = default;
    /// Assignment opererator
    ParameterMap& operator=(const ParameterMap& copy) = default;
    /// Check the parameter existence
    bool exists(const std::string& nam)  const
    {  return m_params.find(nam) != m_params.end();    }
    /// Add/update a parameter value
    bool set(const std::string& nam, const std::string& val, const std::string& type);
    /// Access parameters set
    const Parameters& params()  const   {  return m_params; }
    /// Access single parameter
    const Parameter& param(const std::string& nam, bool throw_if_not_present=true)   const;
    /// Type dependent accessor to a named parameter
    template <typename T> T param(const std::string& nam, bool throw_if_not_present=true)   const;
  };
}      // End namespace gaudi
#endif // DETECTOR_PARAMETERMAP_H
