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
#ifndef DETECTOR_DEVP_H 
#define DETECTOR_DEVP_H 1

// Framework include files
#include "Detector/DeVPSensor.h"
#include "Detector/DeVPGeneric.h"

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
    class DeVPStaticObject : public DeVPGenericStaticObject  {
      DE_CONDITIONS_TYPEDEFS;
      typedef  std::vector<DeVPGenericStatic> Sides;
      typedef  std::vector<DeVPGenericStatic> ModuleSupports;
      typedef  std::vector<DeVPGenericStatic> Modules;
      typedef  std::vector<DeVPGenericStatic> Ladders;

    public:
      Sides          sides;
      ModuleSupports supports;
      Modules        modules;
      Ladders        ladders;
      
      double         sensitiveVolumeCut = 0e0;
    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPStaticObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const override;
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
  class DeVPStatic : public dd4hep::Handle<detail::DeVPStaticObject>   {
    DE_CONDITIONS_TYPEDEFS;

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPStatic,Base);
    /// Printout method to stdout
    void print(int indent, int flags)  const;
    /// Return the number of sensors.
    unsigned int numberSensors() const
    { return ptr()->sensors.size(); }
    /// Return vector of sensors.
    const std::vector<DeVPSensorStatic>& sensors() const
    { return ptr()->sensors;        }
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
    class DeVPObject : public DeVPGenericObject  {
      DE_CONDITIONS_TYPEDEFS;
      typedef std::vector<DeVPGeneric> Sides;
      typedef std::vector<DeVPGeneric> ModuleSupports;
      typedef std::vector<DeVPGeneric> Modules;
      typedef std::vector<DeVPGeneric> Ladders;

    public:
      DeVPStatic     vp_static;
      Sides          sides;
      ModuleSupports supports;
      Modules        modules;
      Ladders        ladders;
      
    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      void print(int indent, int flags)  const  override;
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
  class DeVP : public dd4hep::Handle<detail::DeVPObject>   {
    DE_CONDITIONS_TYPEDEFS;
    typedef detail::DeVPStaticObject static_t;

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVP,Base);
    /// Printout method to stdout
    void print(int indent, int flags)  const;
    /// Access to the static data
    static_t& staticData()  const
    { return access()->vp_static;   }
    /// Return the number of sensors.
    unsigned int numberSensors() const
    { return ptr()->sensors.size(); }
    /// Return vector of sensors.
    const std::vector<DeVPSensor>& sensors() const
    { return ptr()->sensors;        }
  };
}      // End namespace gaudi
#endif // DETECTOR_DEVP_H
