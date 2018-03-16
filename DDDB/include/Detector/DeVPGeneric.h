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
#ifndef DETECTOR_DEVPGENERIC_H 
#define DETECTOR_DEVPGENERIC_H 1

// Framework include files
#include "Detector/DeVPSensor.h"
#include "Detector/DeStatic.h"
#include "Detector/DeIOV.h"

/// Gaudi namespace declaration
namespace gaudi   {

  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// VP  detector element data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVPGenericStaticObject : public DeStaticObject    {
      DE_CONDITIONS_TYPEDEFS;

    public:
      typedef  std::vector<DeVPSensorStatic>  Sensors;
      int      vp_flags = 0;
      Sensors  sensors;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPGenericStaticObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flg)  const  override;
    };
  }    // End namespace detail

  /// Handle defintiion to an instance of VP  detector element data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPGenericStatic : public dd4hep::Handle<detail::DeVPGenericStaticObject>   {
    DE_CONDITIONS_TYPEDEFS;

    enum { MAIN    = 1<<0,
           SIDE    = 1<<1,
           SUPPORT = 1<<2,
           MODULE  = 1<<3,
           LADDER  = 1<<4,
           SENSOR  = 1<<5
    };
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPGenericStatic,Base);
    /// Printout method to stdout
    void print(int indent, int flags)  const;
  };

  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// VP  detector element data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVPGenericObject : public DeIOVObject    {
      DE_CONDITIONS_TYPEDEFS;

    public:
      typedef  std::vector<DeVPSensor>  Sensors;
      int      vp_flags = 0;
      Sensors  sensors;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPGenericObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flg)  const  override;
    };
  }    // End namespace detail

  /// Handle defintiion to an instance of VP  detector element data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPGeneric : public dd4hep::Handle<detail::DeVPGenericObject>   {
    DE_CONDITIONS_TYPEDEFS;
    
    /// Forward definition of the static type for facades
    typedef detail::DeStaticObject static_t;
    
    typedef  std::vector<DeVPSensor>  Sensors;
    enum { MAIN    = 1<<0,
           SIDE    = 1<<1,
           SUPPORT = 1<<2,
           MODULE  = 1<<3,
           LADDER  = 1<<4,
           SENSOR  = 1<<5
    };
    
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPGeneric,Base);
    /// Printout method to stdout
    void print(int indent, int flags)  const;
    /// Access to the static data
    static_t& staticData()  const    { return access()->de_static;   }
  };
  
}      // End namespace gaudi
#endif // DETECTOR_DEVPGENERIC_H
