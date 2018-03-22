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

// Framework include files
#include "Detector/DeVeloSensor.h"
#include "DD4hep/ConditionsData.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

namespace gaudi  {
  const DeVeloSensorElement::itemkey_type DeVeloSensorElement::key_info    = dd4hep::ConditionKey::itemCode("StripInfo");
  const DeVeloSensorElement::itemkey_type DeVeloSensorElement::key_noise   = dd4hep::ConditionKey::itemCode("StripNoise");
  const DeVeloSensorElement::itemkey_type DeVeloSensorElement::key_readout = dd4hep::ConditionKey::itemCode("StripReadout");
}

/// Printout method to stdout
void gaudi::detail::DeVeloSensorStaticObject::print(int indent, int flg)  const   {
  this->DeStaticObject::print(indent, flg);
  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVeloSensor", "%s+ >> %s:%d [%s] Class:%d Sens:%d Side:%c %s R:%s Phi:%s Veto:%s Thick:%g Rmin:%g Rmax:%g",
             DE::indent(indent).c_str(), 
             moduleName.c_str(), moduleId, fullType.c_str(), clsID, sensorNumber,
             (de_user&DeVeloFlags::LEFT) == 0 ? 'R' : 'L',
             (de_user&DeVeloFlags::DOWNSTREAM) == 0 ? "UpStrm" : "DnStrm",
             yes_no((de_user&DeVeloFlags::R_TYPE) != 0),
             yes_no((de_user&DeVeloFlags::PHI_TYPE) != 0),
             yes_no((de_user&DeVeloFlags::PU_TYPE) != 0),
             siliconThickness, innerRadius, outerRadius);
  }
}

/// Initialization of sub-classes
void gaudi::detail::DeVeloSensorStaticObject::initialize()   {
  de_user         |= DeVeloFlags::READOUT;
  moduleName       = param<std::string>("Module");
	typeName         = param<std::string>("Type");
  sensorNumber     = param<int>("SensorNumber");
  siliconThickness = param<double>("SiThick");
  innerRadius      = param<double>("InnerRadius");
  outerRadius      = param<double>("OuterRadius");

  if ( typeName.find("R") == 0 ) de_user |= DeVeloFlags::R_TYPE;
  if ( typeName.find("Phi") == 0 ) de_user |= DeVeloFlags::PHI_TYPE;
  if ( typeName.find("Veto") == 0 ) de_user |= DeVeloFlags::PU_TYPE;
  if ( typeName == "R" || typeName == "Phi" || typeName == "Veto" ) {
    if ( 0 != param<int>("Left") ) de_user |= DeVeloFlags::LEFT;
    if ( 0 != param<int>("Downstream") ) de_user |= DeVeloFlags::DOWNSTREAM;
    fullType = typeName
      + ( (de_user&DeVeloFlags::DOWNSTREAM) ? "DnStrm" : "UpStrm" )
      + ( (de_user&DeVeloFlags::LEFT) ? "Left" : "Right");
  } else {
    if ( (typeName == "PhiDL" || typeName == "PhiUL" ||
          typeName == "RLeft" || typeName == "VetoL") ) de_user |= DeVeloFlags::LEFT;
    if (typeName == "PhiDL" || typeName == "PhiDR") de_user |= DeVeloFlags::DOWNSTREAM;
    fullType = typeName;
  }
  // test new parameters ....
  if ( de_user&DeVeloFlags::R_TYPE ) {
    (0 != param<int>("DownstreamR")) ? de_user|=DeVeloFlags::DOWNSTREAM : de_user &= ~DeVeloFlags::DOWNSTREAM;
  } else if ( de_user&DeVeloFlags::PHI_TYPE )   {
    (0 != param<int>("DownstreamPhi")) ? de_user|=DeVeloFlags::DOWNSTREAM : de_user &= ~DeVeloFlags::DOWNSTREAM;
  }
}

/// Printout method to stdout
void gaudi::detail::DeVeloSensorObject::print(int indent, int flg)  const   {
  DeIOVObject::print(indent, flg);
  printout(INFO,"DeIOVVeloSensor", "%s+ Z:%g Info(%s): %s [%ld entries] Noise(%s): %s [%ld entries] "
           "Readout(%s): %s [READOUT:%s Tell1-no-Sensor:%s]",
           DE::indent(indent).c_str(),
           z,

           info.isValid() ? info.name() : "--",
           yes_no(info.isValid()),
           stripInfos.size(),

           noise.isValid() ? noise.name() : "--",
           yes_no(noise.isValid()),
           stripNoise.size(),

           readout.isValid() ? readout.name() : "--",
           yes_no(readout.isValid()),
           yes_no((de_user&DeVeloFlags::READOUT)!=0),
           yes_no((de_user&DeVeloFlags::T1NOSENS)!=0));
}

/// Initialization of sub-classes
void gaudi::detail::DeVeloSensorObject::initialize()   {
  DeIOVObject::initialize();

  sensor_static = de_static;
  // We require a valid alignment object for sensors!
  checkAlignment();
  z = DetectorElement<DeVeloSensor>(this).toGlobal(XYZPoint(0,0,0)).z();
  for(const auto& c : conditions)   {
    if ( c.first == DeVeloSensor::key_info ) info = c.second;
    else if ( c.first == DeVeloSensor::key_noise ) noise = c.second;
    else if ( c.first == DeVeloSensor::key_readout ) readout = c.second;
  }

  // Check here if values must be valid ?
  if ( !info.isValid() )  {
    except("DeIOVVeloSensor", "Invalid IOV dependent sensor info!");
  }
  else   {
    const dd4hep::cond::AbstractMap& m = info.get<dd4hep::cond::AbstractMap>();
    stripInfos = m.get<std::vector<int> >("StripInfo");
#if 0
    if(m_verbose){
      msg() << MSG::VERBOSE << "Sensor " << m_sensorNumber << " has stripInfoConditions "<< endmsg;
      for( int row = 0 ; row < 64 ; ++row ){ //table of 64 rows * 32 columns of statuses
        msg() << MSG::VERBOSE << format("%4i-%4i",(32*row),(32*row+31));
        for( int col = 0 ; col < 32 ; ++col ){
          msg() << MSG::VERBOSE << format("%2i ",m_stripInfos[32*row+col].asInt());
        }
        msg() << MSG::VERBOSE << endmsg;
      }
    }
#endif
    if (stripInfos.size() != DeVeloSensorStaticObject::numberOfStrips) {
      except("DeIOVVeloSensor","Strip info condition size does not match number of strips!");
    }
  }
  if ( !noise.isValid() )  {
    except("DeIOVVeloSensor", "Invalid IOV dependent sensor noise!");
  }
  else   {
    const dd4hep::cond::AbstractMap& m = noise.get<dd4hep::cond::AbstractMap>();
    stripNoise = m.get<std::vector<double> >("StripNoise");
    if ( stripNoise.size() != DeVeloSensorStaticObject::numberOfStrips ) {
      except("IOVDeVeloSensor","Strip noise condition size does not match number of strips!");
    }
  }
  if ( !readout.isValid() )  {
    // except(DeIOVVeloSensor", "Invalid IOV dependent sensor readout!");
  }
  else   {
    const dd4hep::cond::AbstractMap& m = noise.get<dd4hep::cond::AbstractMap>();
    (0 != m.get<int>("ReadoutFlag"))        ? de_user |= DeVeloFlags::READOUT  : de_user &= ~DeVeloFlags::READOUT;
    (0 != m.get<int>("Tell1WithoutSensor")) ? de_user |= DeVeloFlags::T1NOSENS : de_user &= ~DeVeloFlags::T1NOSENS;
  }
}
