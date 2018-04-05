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
#ifndef DETECTOR_DEVELOGENERIC_H 
#define DETECTOR_DEVELOGENERIC_H 1

// Framework include files
#include "Detector/DeVeloSensor.h"
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
    class DeVeloGenericStaticObject : public DeStaticObject    {
      DE_CONDITIONS_TYPEDEFS;
      /// Unique classID
      enum { classID = 2 };

    public:
      typedef  std::vector<DeVeloGenericStaticObject*> Children;
      typedef  std::vector<DeVeloSensorStatic>         Sensors;
      Children children;  // Note: Sensors are no children. They go extra
      Sensors  sensors;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloGenericStaticObject);
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
  class DeVeloGenericStaticElement : public dd4hep::Handle<detail::DeVeloGenericStaticObject>   {
    DE_CONDITIONS_TYPEDEFS;
    typedef Object static_t;
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloGenericStaticElement,Base);
    /// Export access to the sensors from the detector element
    Object::Sensors& sensors()  const   {   return access()->sensors;    }
    /// Export access to the children from the detector element
    Object::Children& children()  const {   return access()->children;   }
  };
  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorStaticElement<DeVeloGenericStaticElement>  DeVeloGenericStatic;

  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// VP  detector element data
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVeloGenericObject : public DeIOVObject    {
      DE_CONDITIONS_TYPEDEFS;

    public:
      typedef  std::vector<DeVeloGenericObject*> Children;
      typedef  std::vector<DeVeloSensor>         Sensors;
      Children children;  // Note: Sensors are no children. They go extra
      Sensors  sensors;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloGenericObject);
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
  class DeVeloGenericElement : public dd4hep::Handle<detail::DeVeloGenericObject>   {
    DE_CONDITIONS_TYPEDEFS;
    /// These two are needed by the DetectorElement<TYPE> to properly forward requests.
    typedef Object::static_t static_t;
    typedef Object           iov_t;
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloGenericElement,Base);
    /// Access to the static data. Must define here, not in DetectorElement to preserve types.
    static_t& staticData()  const       {  return access()->de_static;  }
    /// Export access to the sensors from the detector element
    Object::Sensors& sensors()  const   {   return access()->sensors;   }
    /// Export access to the children from the detector element
    Object::Children& children()  const {   return access()->children;  }
  };

  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorElement<DeVeloGenericElement>  DeVeloGeneric;
  
}      // End namespace gaudi
#endif // DETECTOR_DEVELOGENERIC_H
