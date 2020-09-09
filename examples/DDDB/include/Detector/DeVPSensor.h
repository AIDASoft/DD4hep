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
#ifndef DETECTOR_DEVPSENSOR_H 
#define DETECTOR_DEVPSENSOR_H 1

// C/C++ include files
#include <array>

// Framework include files
#include "Kernel/VPConstants.h"
#include "Detector/DetectorElement.h"
#include "Detector/DeStatic.h"
#include "Detector/DeIOV.h"

/// Gaudi namespace declaration
namespace gaudi   {


  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// Generic VP static detector element 
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVPSensorStaticObject : public detail::DeStaticObject  {
    public:
      enum { classID = 1008205 };
      /// Cache of local x-cooordinates
      std::array<double,VP::NSensorColumns> local_x;
      /// Cache of x-pitch
      std::array<double,VP::NSensorColumns> x_pitch;

      /// Dimensions of the sensor active area
      double sizeX              = 0e0;
      double sizeY              = 0e0;
      double thickness          = 0e0;
      /// Length of chip active area
      double chipSize           = 0e0;
      /// Distance between two chips
      double interChipDist      = 0e0;
      /// Cell size of pixels
      double pixelSize          = 0e0;
      /// Cell size in column direction of elongated pixels
      double interChipPixelSize = 0e0;
      /// Global Z position
      double zpos               = 0e0;

      /// Number of chips per ladder
      unsigned int nChips       = 0;
      /// Number of columns and rows
      unsigned int nCols        = 0;
      unsigned int nRows        = 0;
      /// Sensor ans module number
      unsigned int sensorNumber = 0;
      unsigned int module       = 0;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPSensorStaticObject);

      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const  override;
    };
  }    // End namespace detail

  /// Handle defintiion to an instance of VP static detector element data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPSensorStaticElement
    : public dd4hep::Handle<detail::DeVPSensorStaticObject>
  {
    DE_CONDITIONS_TYPEDEFS;
    /// This is needed by the DetectorElement<TYPE> to properly forward requests.
    typedef Object static_t;
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPSensorStaticElement,Base);
  };
  
  /// For the full sensor object, we have to combine it with the geometry stuff:
  typedef  DetectorStaticElement<DeVPSensorStaticElement>  DeVPSensorStatic;

  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// Generic VP iov dependent detector element 
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVPSensorObject : public DeIOVObject  {
      DE_CONDITIONS_TYPEDEFS;
      typedef DeVPSensorStatic::Object static_t;

      /// The static part of the detector element
      DeVPSensorStatic sensor_static;
      
      /** For ref only: values are taken from the RUN-II conditions information  */
      /// Reference to time-dependent sensor information
      Condition        info;
      /// Reference to time-dependent noise
      Condition        noise;
      /// Reference to time-dependent readout configuration parameters
      Condition        readout;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVPSensorObject);

      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const  override;

    };
  }    // End namespace detail

  
  /// Handle defintiion to an instance of VP IOV dependent data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPSensorElement : public dd4hep::Handle<detail::DeVPSensorObject>   {
    DE_CONDITIONS_TYPEDEFS;
    typedef Object::static_t static_t;
    typedef Object           iov_t;

  public:
    /** Define conditions access keys for optimization     */
    static const itemkey_type key_info;
    static const itemkey_type key_noise;
    static const itemkey_type key_readout;
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVPSensorElement,Base);
    /// Access to the static data. Does this need to be optionized???
    static_t& staticData()  const  {  return access()->sensor_static;            }
    //bool isLeft()   const          {  return (ptr()->de_user & VP::RIGHT) == 0;  }
    //bool isRight()   const         {  return (ptr()->de_user & VP::RIGHT) != 0;  }

  };

  /// For the full sensor object, we have to combine it with the geometry stuff:
  typedef  DetectorElement<DeVPSensorElement>  DeVPSensor;

}      // End namespace gaudi
#endif // DETECTOR_DEVPSENSOR_H
