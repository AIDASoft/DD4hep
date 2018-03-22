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
#ifndef DETECTOR_DEVELO_H 
#define DETECTOR_DEVELO_H 1

// Framework include files
#include "Detector/DeVeloSensor.h"
#include "Detector/DeVeloGeneric.h"

#define DE_VELO_TYPEDEFS(TYP) public:                               \
  DE_CONDITIONS_TYPEDEFS;                                           \
  typedef std::vector<DeVeloGeneric##TYP>           Sides;          \
  typedef std::vector<DeVeloGeneric##TYP>           Modules;        \
  typedef std::vector<DeVeloGeneric##TYP>           Supports;       \
  typedef std::vector<DeVeloSensor##TYP>            Sensors;        \
  typedef std::map<unsigned int, DeVeloSensor##TYP> SensorByTell1;  \
  typedef std::map<unsigned int, unsigned int>      Tell1BySensorID

/// Gaudi namespace declaration
namespace gaudi   {

  /// Gaudi::detail namespace declaration
  namespace detail   {
    
    /// Velo  detector element data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVeloStaticObject : public DeStaticObject  {
      DE_VELO_TYPEDEFS(Static);
      
    public:
      enum { NHalfs = 2 };
      enum { classID = 8100 };
      Sides            sides;
      Modules          modules;
      Supports         supports;
      Sensors          zOrdered;
      Sensors          sensors[3];
      Sensors          rSensors[2];
      Sensors          phiSensors[2];
      Sensors          rphiSensors[2];
      Sensors          puSensors[2];
      SensorByTell1    sensorByTell1;
      Tell1BySensorID  tell1BySensorID;
      dd4hep::Position halfBoxOffsets[NHalfs];
      double           sensitiveVolumeCut = 0e0;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloStaticObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const override;
    };
  }    // End namespace detail


  /// Handle defintiion to an instance of Velo  detector element data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloStaticElement : public dd4hep::Handle<detail::DeVeloStaticObject>   {
    DE_VELO_TYPEDEFS(Static);
    typedef Object static_t;
    enum { NHalfs = 2 };

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloStaticElement,Base);
  };
  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorStaticElement<DeVeloStaticElement>  DeVeloStatic;

  /// Gaudi::detail namespace declaration
  namespace detail   {
  
    /// Velo  detector element data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVeloObject : public DeIOVObject  {
      DE_VELO_TYPEDEFS();
      typedef DeVeloStatic::Object static_t;

    public:
      DeVeloStatic     vp_static;
      Sides            sides;
      Modules          modules;
      Supports         supports;
      Sensors          zOrdered;
      Sensors          sensors[3];
      Sensors          rSensors[2];
      Sensors          phiSensors[2];
      Sensors          rphiSensors[2];
      Sensors          puSensors[2];

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloObject);
      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      void print(int indent, int flags)  const  override;
    };
  }    // End namespace detail

  /// Handle defintiion to an instance of Velo  detector element data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloElement : public dd4hep::Handle<detail::DeVeloObject>   {
    DE_VELO_TYPEDEFS();
    typedef Object::static_t static_t;
    typedef Object           iov_t;
    enum { NHalfs = 2 };

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloElement,Base);
    /// Access to the static data. If this handle is valid, the static handle must be valid too!
    static_t& staticData()  const    { return *(access()->vp_static.ptr());   }
  };

  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorElement<DeVeloElement>  DeVelo;

}      // End namespace gaudi
#undef DE_VELO_TYPEDEFS
#endif // DETECTOR_DEVELO_H
