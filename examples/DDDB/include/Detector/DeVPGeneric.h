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
      /// Unique classID
      enum { classID = 2 };

    public:
      typedef  std::vector<DeVPSensorStatic>  Sensors;
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
  class DeVPGenericStaticElement
    : public dd4hep::Handle<detail::DeVPGenericStaticObject>
  {
    DE_CONDITIONS_TYPEDEFS;
    typedef Object           static_t;
    typedef Object::Sensors  Sensors;
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPGenericStaticElement,Base);
    /// Export access to the sensors from the detector element
    Object::Sensors& sensors()  const   {   return access()->sensors;    }
  };
  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorStaticElement<DeVPGenericStaticElement>  DeVPGenericStatic;

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
  class DeVPGenericElement : public dd4hep::Handle<detail::DeVPGenericObject>   {
    DE_CONDITIONS_TYPEDEFS;
    /// These two are needed by the DetectorElement<TYPE> to properly forward requests.
    typedef Object           iov_t;
    typedef Object::static_t static_t;
    typedef Object::Sensors  Sensors;
    
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPGenericElement,Base);
    /// Access to the static data
    static_t& staticData()  const    { return access()->de_static;   }
    /// Export access to the sensors from the detector element
    Object::Sensors& sensors()  const   {   return access()->sensors;   }
  };
  
  /// For the fully enabled object, we have to combine it with the generic stuff
  typedef  DetectorElement<DeVPGenericElement>  DeVPGeneric;

}      // End namespace gaudi
#endif // DETECTOR_DEVPGENERIC_H
