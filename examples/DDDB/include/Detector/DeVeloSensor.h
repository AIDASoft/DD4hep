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
#ifndef DETECTOR_DEVELOSENSOR_H 
#define DETECTOR_DEVELOSENSOR_H 1

// C/C++ include files
#include <array>

// Framework include files
#include "Detector/DetectorElement.h"
#include "Detector/DeStatic.h"
#include "Detector/DeIOV.h"
#include "DD4hep/DD4hepUnits.h"

/// Gaudi namespace declaration
namespace gaudi   {

  namespace DeVeloConst  {
  }
  namespace DeVeloFlags  {
    enum {
      // DeVelo specific
      ALL        =    2,

      // de_flags flags(static/dynamic): ALL objects
      RIGHT      =    0,
      LEFT       = 1<<0,
      MAIN       = 1<<2,
      SIDE       = 1<<3,
      SUPPORT    = 1<<4,
      MODULE     = 1<<5,
      LADDER     = 1<<6,
      SENSOR     = 1<<7,

      // de_user flags(static): DeVeloSensorStatic specific
      TOP        = 1<<8,  // 4
      DOWNSTREAM = 1<<9,
      R_TYPE     = 1<<10,
      PHI_TYPE   = 1<<11,
      PU_TYPE    = 1<<12,

      // de_user flags(dynamic): DeVeloSensorIOV specific
      READOUT    = 1<<7,
      T1NOSENS   = 1<<8
    };
  }
    
  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// Generic VP static detector element 
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVeloSensorStaticObject : public detail::DeStaticObject  {
    public:
      /// Unique classID
      enum { classID = 1008101  };

      static const unsigned int 	minRoutingLine = 1;
      static const unsigned int 	maxRoutingLine = 2048;
      static const unsigned int 	numberOfStrips = 2048;

      std::string 	moduleName;
      std::string 	typeName;
      std::string 	fullType;
      int 	        moduleId;//<Liverpool database module id
      unsigned int 	sensorNumber;
      double       	siliconThickness;
      double 	      innerRadius;
      double 	      outerRadius;
      
      /// Un-inited - still
      unsigned int 	numberOfZones;
      std::map< unsigned int, unsigned int > 	mapRoutingLineToStrip;
      std::map< unsigned int, unsigned int > 	mapStripToRoutingLine;
      std::vector< std::pair< XYZPoint, XYZPoint > > 	stripLimits;

    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloSensorStaticObject);

      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const  override;
    };
  }    // End namespace detail

  /// Handle definition to an instance of VP static detector element data
  /**
   *  This object defines the behaviour of the objects's data.
   *  We implement here only the behaviour of the object specific
   *  stuff. The geometry interactions are then combined with this
   *  implementation and the specialized detector element
   *  DetectorElementStatic<TYPE> to the real data accessor.
   *  The DetectorElementStatic<TYPE> by non-virtual inheritance
   *  automatically exposes the specific stuff here.
   *
   *  See the corresponding typedef below.
   *
   *  Note: in this class the is no big deal of specialization!
   *        this for the time being is only for illustration about the mechanism.
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloSensorStaticElement : public dd4hep::Handle<detail::DeVeloSensorStaticObject>   {
    DE_CONDITIONS_TYPEDEFS;
    /// This is needed by the DetectorElement<TYPE> to properly forward requests.
    typedef Object static_t;
  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloSensorStaticElement,Base);
  };

  /// For the full sensor object, we have to combine it with the geometry stuff:
  typedef  DetectorStaticElement<DeVeloSensorStaticElement>  DeVeloSensorStatic;
  
  
  /// Gaudi::detail namespace declaration
  namespace detail   {

    /// Generic VP iov dependent detector element 
    /**
     *
     *  \author  Markus Frank
     *  \date    2018-03-08
     *  \version  1.0
     */
    class DeVeloSensorObject : public DeIOVObject  {
      DE_CONDITIONS_TYPEDEFS;
      typedef DeVeloSensorStatic::Object static_t;
      
      /// The static part of the detector element. Cached for optimizations to avoid casts
      DeVeloSensorStatic sensor_static;
      
      /** For ref only: values are taken from the RUN-II conditions information  */
      /// Reference to time-dependent sensor information
      Condition        info;
      /// Reference to time-dependent noise
      Condition        noise;
      /// Reference to time-dependent readout configuration parameters
      Condition        readout;

      DeIOV            halfBoxGeom;      
      Condition 	     stripNoiseCondition;
      Condition 	     stripInfoCondition;
      Condition 	     readoutCondition;
      
      std::vector< double > stripNoise;
      std::vector< int >   	stripInfos;

      double 	      z = 0.0;
      
    public:
      /// Standard constructors and assignment
      DE_CTORS_DEFAULT(DeVeloSensorObject);

      /// Initialization of sub-classes
      virtual void initialize()  override;
      /// Printout method to stdout
      virtual void print(int indent, int flags)  const  override;
    };
  }    // End namespace detail

  
  /// Handle defintiion to an instance of VP IOV dependent data
  /**
   *  This object defines the behaviour of the objects's data.
   *  We implement here only the behaviour of the object specific
   *  stuff. The geometry interactions are then combined with this
   *  implementation and the specialized detector element
   *  DetectorElement<TYPE> to the real data accessor.
   *  The DetectorElement<TYPE> by non-virtual inheritance
   *  automatically exposes the specific stuff here.
   *
   *  See the corresponding typedef below.
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloSensorElement : public dd4hep::Handle<detail::DeVeloSensorObject>  {    
    DE_CONDITIONS_TYPEDEFS;
    /// These two are needed by the DetectorElement<TYPE> to properly forward requests.
    typedef Object::static_t static_t;
    typedef Object           iov_t;

  public:
    /** Define conditions access keys for optimization     */
    static const itemkey_type key_info;
    static const itemkey_type key_noise;
    static const itemkey_type key_readout;

    /** @class DeVeloSensor::StripInfo DeVeloSensor.h VeloDet/DeVeloSensor.h
     *
     *  Encodes strip information.
     *  From construction information states are :
     *  OK, Low gain, Noisy, Dead, Open, Pinhole and Short
     *
     *  @author Kurt Rinnert
     *  @date   2006-01-18
     */
    class StripInfo
    {
    public:
      StripInfo() : m_info(0) { ; }
      StripInfo(int i) : m_info(i) { ; }

    private:
      enum {
        LOWGAIN=  2,
        NOISY= 1,
        STRIPOK = 0,
        DEAD=    -1,
        OPEN=    -2,
        PINHOLE= -3,
        SHORT=   -4
      };

    public:
      /// No problems with this strip
      bool stripIsOK() const { return m_info==STRIPOK; }
      /// Lower gain strip
      bool stripIsLowGain() const { return m_info==LOWGAIN; }
      /// higher noise strip
      bool stripIsNoisy() const { return m_info==NOISY; }
      /// very low gain strip
      bool stripIsDead() const { return m_info==DEAD; }
      /// Did not bond strip to beetle
      bool stripIsOpen() const { return m_info==OPEN; }
      /// pinhole in sensor
      bool stripIsPinhole() const { return m_info==PINHOLE; }
      /// Strip shorted to another (may not be next neighbour)
      bool stripIsShort() const { return m_info==SHORT; }
      /// StripInfo as integer
      int asInt() const{ return m_info; }

    private:
      int m_info;
    };

    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloSensorElement,Base);
    /// Access to the static data. Does this need to be optionized???
    static_t& staticData()  const
    {  return access()->sensor_static;                              }
    /// Access the time dependent data block. Used internally - may be specialized for optimization.
    const iov_t& iovData()  const
    {  return *access();                                            }

    /// Return station number, station contains 2 modules (right and left) 
    unsigned int station () const
    {  return (staticData().sensorNumber&0x3E) >> 1;                }
    ///	Returns the sensor number.
    unsigned int 	sensorNumber () const
    {  return staticData().sensorNumber;                            }
    /// Returns the hardware module number. 
    unsigned int 	moduleId () const
    {  return staticData().moduleId;                                }
    ///	Return the number of strips.
    unsigned int numberOfStrips () const
    {  return staticData().numberOfStrips;                          }
    /// Access the strip info object
    StripInfo stripInfo(size_t which)  const
    {  return iovData().stripInfos[which];                          }
    ///	The number of zones in the detector.
    unsigned int 	numberOfZones () const
    {  return staticData().numberOfZones;                           }
    /// Return the z position of the sensor in the global frame. 
    double z () const
    {  return iovData().z;                                          }
    /// The minimum radius for the sensitive area of the sensor. 
    double 	innerRadius () const
    {  return staticData().innerRadius;                             }
    ///	The maximum radius for the sensitive area of the sensor. 
    double 	outerRadius () const
    {  return staticData().outerRadius;                             }
    /// The thickness of the sensor in mm. 
    double 	siliconThickness () const
    {  return staticData().siliconThickness;                        }
    /// Return true for X<0 side of the detector (-ve x is Right) 
    bool   	isRight () const
    {  return 0 == (staticData().de_user&DeVeloFlags::LEFT);        }
    /// Return true for X>0 side of the detector (+ve x is Left) 
    bool 	  isLeft () const
    {  return 0 != (staticData().de_user&DeVeloFlags::LEFT);        }
    /// Returns true if sensor is downstream. 
    bool 	  isDownstream () const
    {  return 0 != (staticData().de_user&DeVeloFlags::DOWNSTREAM);  }
    /// Returns true if pile up Sensor. 
    bool 	  isPileUp () const
    {  return 0 != (staticData().de_user&DeVeloFlags::PU_TYPE);     }
    /// Returns true if R Sensor. 
    bool   	isR () const
    {  return 0 != (staticData().de_user&DeVeloFlags::R_TYPE);      }
    /// Returns true if Phi Sensor. 
    bool 	  isPhi () const
    {  return 0 != (staticData().de_user&DeVeloFlags::PHI_TYPE);    }
    /// Returns true if sensor is top cabled. 
    bool 	  isTop () const
    {  return 0 != (staticData().de_user&DeVeloFlags::TOP);         }
    /// Returns true if sensor is bottom cabled. 
    bool 	  isBottom () const
    {  return 0 == (staticData().de_user&DeVeloFlags::TOP);         }
    /** Check whether this sensor is read out at all (cached condition).
     *  This information is based on CondDB, i.e. it can change
     *  with time.
     */
    bool isReadOut() const
    { return 0 != (staticData().de_user&DeVeloFlags::READOUT);      }
    /** Check whether this sensor exists or is TELL1 w/o sensor (cached condition).
     *  This information is based on CondDB, i.e. it can change
     *  with time.
     */
    bool tell1WithoutSensor() const
    { return 0 != (staticData().de_user&DeVeloFlags::T1NOSENS);     }


    unsigned int globalZoneOfStrip(const unsigned int strip) const {
      int flg = staticData().de_user;
      return (flg&DeVeloFlags::PHI_TYPE)
        ? static_cast<unsigned int>(strip>682)
        : ((flg&DeVeloFlags::DOWNSTREAM) ? 3-strip/512 : strip/512);
    }
    /** The Noise of a strip (cached condition).
     *  This information is based on CondDB, i.e. it can change
     *  with time.
     */
    double stripNoise(unsigned int strip) const { return iovData().stripNoise[strip];            }

    /** Get info for this strip (cached condition).
     *  This information is based on CondDB, i.e. it can change
     *  with time.
     *  @see StripInfo
     */
    StripInfo stripInfo(unsigned int strip) const { return iovData().stripInfos[strip];          }

    /// Convert position inside Velo half Box (one per side) into local position
    /// Local from is +ve x and Upstream
    XYZPoint veloHalfBoxToLocal(const XYZPoint& boxPos) const  {
      XYZPoint globalPos = iovData().halfBoxGeom.toGlobal(boxPos);
      return DeIOV(ptr()).toLocal(globalPos);
    }
    /// Convert position inside Velo half Box (one per side) to global
    XYZPoint veloHalfBoxToGlobal(const XYZPoint& boxPos) const {
      return iovData().halfBoxGeom.toGlobal(boxPos);
    }
    /// Convert global position to inside Velo half Box (one per side)
    XYZPoint globalToVeloHalfBox(const XYZPoint& globalPos) const {
      return iovData().halfBoxGeom.toLocal(globalPos);
    }

    /// Returns a pair of points which define the begin and end points of a strip in the local frame
    std::pair<XYZPoint,XYZPoint> localStripLimits(unsigned int strip) const {
      return staticData().stripLimits[strip];
    }

    /// Returns a pair of points which define the begin and end points of a strip in the global frame
    std::pair<XYZPoint,XYZPoint> globalStripLimits(unsigned int strip) const {
      const DeIOV iov(ptr());
      const static_t& s = staticData();
      XYZPoint begin = iov.toGlobal(s.stripLimits[strip].first);
      XYZPoint end   = iov.toGlobal(s.stripLimits[strip].second);
      return std::pair<XYZPoint,XYZPoint>(begin,end);
    }
    /// Convert local phi to ideal global phi
    double localPhiToGlobal(double phiLocal) const {
      int flg = staticData().de_user;
      if(  flg&DeVeloFlags::DOWNSTREAM ) phiLocal = -phiLocal;
      if( 0 == (flg&DeVeloFlags::LEFT) ) phiLocal += dd4hep::pi;  // if ( isRight() ) ....
      return phiLocal;
    }
    /// Convert routing line to chip channel (1234 -> 0213)
    unsigned int RoutingLineToChipChannel(unsigned int routLine) const {
      routLine = detail::DeVeloSensorStaticObject::maxRoutingLine-routLine;
      if(1 == routLine%4){
        routLine++;
      }else if(2 == routLine%4){
        routLine--;
      }
      return routLine;
    }
    /// Convert chip channel to routing line (0213 -> 1234)
    unsigned int ChipChannelToRoutingLine(unsigned int chipChan) const {
      if(1 == chipChan%4){
        chipChan++;
      } else if(2 == chipChan%4) {
        chipChan--;
      }
      return (detail::DeVeloSensorStaticObject::numberOfStrips-chipChan);
    }
    /// Convert chip channel to strip number
    unsigned int ChipChannelToStrip(unsigned int chipChan) const {
      return RoutingLineToStrip(ChipChannelToRoutingLine(chipChan));
    }
    /// Convert strip number to chip channel
    unsigned int StripToChipChannel(unsigned int strip) const {
      return RoutingLineToChipChannel(StripToRoutingLine(strip));
    }
    /// Convert routing line to strip number
    unsigned int RoutingLineToStrip(unsigned int routLine) const {
      return staticData().mapRoutingLineToStrip.at(routLine);
    }
    /// Convert strip number to routing line
    unsigned int StripToRoutingLine(unsigned int strip) const {
      return staticData().mapStripToRoutingLine.at(strip);
    }
    /// Get the chip number from the routing line
    unsigned int ChipFromRoutingLine(unsigned int routLine) const {
      return ChipFromChipChannel(RoutingLineToChipChannel(routLine));
    }
    /// Get the chip number from the chip channel
    unsigned int ChipFromChipChannel(unsigned int chipChan) const {
      return static_cast<int>(chipChan/128);
    }
    /// Get the Chip from the strip
    unsigned int ChipFromStrip(unsigned int strip) const {
      return ChipFromChipChannel(StripToChipChannel(strip));
    }
    /**  Return the validity of a strip
     *   Since this method uses the condition cache, the result
     *   depends on CondDB.
     */
    bool OKStrip(unsigned int strip) const {
      return (strip<staticData().numberOfStrips && stripInfo(strip).stripIsOK());
    }
    /// Returns the validity of a given channel
    bool OKChipChannel(unsigned int chipChan) const {
      return (chipChan<staticData().numberOfStrips && OKStrip(ChipChannelToStrip(chipChan)));
    } 
  };
  
  /// For the full sensor object, we have to combine it with the geometry stuff:
  typedef  DetectorElement<DeVeloSensorElement>  DeVeloSensor;

  
  /// Handle defintiion to an instance of VP IOV dependent data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloRSensor : public DeVeloSensorElement   {
    enum { classID = 1008101  };

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloRSensor,DeVeloSensorElement);
  };

  
  /// Handle defintiion to an instance of VP IOV dependent data
  /**
   *  This object defines the behaviour of the objects's data
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloPhiSensor : public DeVeloSensorElement   {
    enum { classID = 1008101  };

  public:
    /// Standard constructors and assignment
    DE_CTORS_HANDLE(DeVeloPhiSensor,DeVeloSensorElement);
  };
  
}      // End namespace gaudi
#endif // DETECTOR_DEVELOSENSORIOV_H
