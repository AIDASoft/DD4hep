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
#ifndef DD4HEP_CONDITIONS_CONDITIONSDATA_H
#define DD4HEP_CONDITIONS_CONDITIONSDATA_H

// Framework include files
#include "DD4hep/Objects.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <vector>
#include <stdexcept>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    /// Client data addition
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    struct ClientData {
      virtual ~ClientData();
      virtual void release() = 0;
    };

    /// Conditions data block. Internally maps other objects to abstract data blocks
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class AbstractMap   {
    private:
    public:
      enum {
        REGULAR = 0,
        ALIGNMENT = 6
      };
      typedef std::map<std::string, OpaqueDataBlock> Params;
      ClientData*   clientData;
      Params        params;
      int           classID;
      /// Default constructor
      AbstractMap();
      /// Copy constructor
      AbstractMap(const AbstractMap& c);
      /// Default destructor
      virtual ~AbstractMap();
      /// Assignment operator
      AbstractMap& operator=(const AbstractMap& c);
      /// Simplify access to client data
      template <typename T> T* option()  const   {
        return static_cast<T*>(clientData);
      }
      /// Simplify access to first item of the parameter list (const access)
      const Params::value_type& firstParam()  const   {
        Params::const_iterator i=std::begin(params);
        if ( i != std::end(params) ) return (*i);
        throw std::runtime_error("AbstractMap: Failed to access non-existing first parameter");
      }
      /// Simplify access to first item of the parameter list
      Params::value_type& firstParam()   {
        Params::iterator i=std::begin(params);
        if ( i != std::end(params) ) return (*i);
        throw std::runtime_error("AbstractMap: Failed to access non-existing first parameter");
      }
      /// Simplify access to first item of the parameter list (const access)
      template <typename T> const T& first()  const   {
        Params::const_iterator i=std::begin(params);
        if ( i != std::end(params) ) return (*i).second.get<T>();
        throw std::runtime_error("AbstractMap: Failed to access non-existing first item");
      }
      /// Simplify access to first item of the parameter list
      template <typename T> T& first()   {
        Params::iterator i=std::begin(params);
        if ( i != std::end(params) ) return (*i).second.get<T>();
        throw std::runtime_error("AbstractMap: Failed to access non-existing first item");
      }
      /// Simplify access to mapped item of the parameter list (const access)
      template <typename T> const T& operator[](const std::string& item)  const   {
        Params::const_iterator i=params.find(item);
        if ( i != std::end(params) ) return (*i).second.get<T>();
        throw std::runtime_error("AbstractMap: Failed to access non-existing item:"+item);
      }
      /// Simplify access to mapped item of the parameter list
      template <typename T> T& operator[](const std::string& item)   {
        Params::iterator i=params.find(item);
        if ( i != std::end(params) ) return (*i).second.get<T>();
        throw std::runtime_error("AbstractMap: Failed to access non-existing item:"+item);
      }
    };

  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_CONDITIONS_CONDITIONSDATA_H    */
