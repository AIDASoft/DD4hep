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
#ifndef DD4HEP_OPAQUEDATABINDER_H
#define DD4HEP_OPAQUEDATABINDER_H

// Framework include files
#include "DD4hep/Printout.h"

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Helper class to bind string values to C++ data objects (primitive or complex)
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct ValueBinder   {
    template <typename T, typename Q> bool bind(T& object, const std::string& val, const Q*) const;
  };

  /// Helper class to bind string values to a STL vector of data objects (primitive or complex)
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct VectorBinder  {
    template <typename T, typename Q> bool bind(T& object, const std::string& val, const Q*) const;
  };

  /// Helper class to bind string values to a STL list of data objects (primitive or complex)
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct ListBinder   {
    template <typename T, typename Q> bool bind(T& object, const std::string& val, const Q*) const;
  };

  /// Helper class to bind string values to a STL set of data objects (primitive or complex)
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct SetBinder   {
    template <typename T, typename Q> bool bind(T& object, const std::string& val, const Q*) const;
  };

  /// Helper class to bind STL map objects
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  struct MapBinder   {
    template <typename T, typename Q> bool bind(T& object, const Q*) const;
  };

  /// Helper class to bind string values to C++ data items
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class OpaqueDataBinder  {
  protected:
    static const char*           _char()    { return 0; }
    static const unsigned char*  _uchar()   { return 0; }
    static const short*          _short()   { return 0; }
    static const unsigned short* _ushort()  { return 0; }
    static const int*            _int()     { return 0; }
    static const unsigned int*   _uint()    { return 0; }
    static const long*           _long()    { return 0; }
    static const unsigned long*  _ulong()   { return 0; }
    static const float*          _float()   { return 0; }
    static const double*         _double()  { return 0; }
    static const std::string*    _string()  { return 0; }

  public:
    /// Default constructor
    OpaqueDataBinder() = default;
    /// Default destructor
    ~OpaqueDataBinder() = default;

    /// Binding function for scalar items. See the implementation function for the concrete instantiations
    template <typename BINDER, typename OBJECT> static
    bool bind(const BINDER& b, OBJECT& object, const std::string& typ, const std::string& val);

    /// Binding function for sequences (unmapped STL containers)
    template <typename OBJECT> static
    bool bind_sequence(OBJECT& object, const std::string& typ, const std::string& val);

    /// Binding function for STL maps. Does not fill data!
    template <typename BINDER, typename OBJECT> static
    bool bind_map(const BINDER& b, OBJECT& o, const std::string& key_type, const std::string& val_type);

    /// Filling function for STL maps.
    template <typename BINDER, typename OBJECT> static
    bool insert_map(const BINDER& b, OBJECT& o,
                    const std::string& key_type, const std::string& key,
                    const std::string& val_type, const std::string& val);
    /// Filling function for STL maps.
    template <typename BINDER, typename OBJECT> static
    bool insert_map(const BINDER& b, OBJECT& o,
                    const std::string& key_type, const std::string& val_type,
                    const std::string& pair_data);
  };
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_OPAQUEDATABINDER_H */
