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
#include "Detector/DeVPSensor.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Printout.h"

namespace gaudi  {
  const DeVPSensor::itemkey_type DeVPSensor::key_info    = dd4hep::ConditionKey::itemCode("StripInfo");
  const DeVPSensor::itemkey_type DeVPSensor::key_noise   = dd4hep::ConditionKey::itemCode("StripNoise");
  const DeVPSensor::itemkey_type DeVPSensor::key_readout = dd4hep::ConditionKey::itemCode("StripReadout");
}

/// Printout method to stdout
void gaudi::detail::DeVPSensorStaticObject::print(int indent, int flg)  const   {

  if ( flg & DePrint::SPECIFIC )  {
    printout(INFO,"DeVPSensorStatic", "%s*==========%s",
             DE::indent(indent).c_str(), detector.path().c_str());
    printout(INFO,"DeVPSensorStatic",
             "%s%s >> Module:%d Sensor:%d %s %d Chips Rows:%d Cols:%d",
             DE::indent(indent).c_str(),"+ ",
             module, sensorNumber, isLeft ? "Left" : "Right", nChips, nCols, nRows);
  }
  if ( flg & DePrint::DETAIL )  {
    printout(INFO,"DeVPSensorStatic",
             "%s%s >> Thickness:%g ChipSize:%g Dist:%g Pix-Size:%g Dist:%g",
             DE::indent(indent).c_str(),"+ ",
             thickness, chipSize, interChipDist, pixelSize, interChipPixelSize);
    printout(INFO,"DeVPSensorStatic",
             "%s%s >> SizeX: %g SizeY: %g local:%ld pitch:%ld",
             DE::indent(indent).c_str(), "+ ", sizeX, sizeY, local_x.size(), x_pitch.size());
  }
  this->DeStaticObject::print(indent, flg);
}

/// Initialization of sub-classes
void gaudi::detail::DeVPSensorStaticObject::initialize()   {
  std::string side   = param<std::string>("Side");

  sensorNumber       = param<int>("SensorNumber");
  module             = param<int>("Module");
  isLeft             = side.find("Left") == 0; 

  thickness          = param<double>("Thickness");
  nChips             = param<int>("NChips");
  chipSize           = param<double>("ChipSize");
  interChipDist      = param<double>("InterChipDist");
  nCols              = param<int>("NColumns");
  nRows              = param<int>("NRows");
  pixelSize          = param<double>("PixelSize");
  interChipPixelSize = param<double>("InterChipPixelSize");
    
  sizeX              = nChips * chipSize + (nChips - 1) * interChipDist;
  sizeY              = chipSize;
  for (unsigned int col = 0; col < VP::NSensorColumns; ++col) {
    // Calculate the x-coordinate of the pixel centre and the pitch.
    const double x0 =
      (col / VP::NColumns) * (chipSize + interChipDist);
    double x = x0 + (col % VP::NColumns + 0.5) * pixelSize;
    double pitch = pixelSize;
    switch (col) {
    case 256:
    case 512:
      // right of chip border
      x -= 0.5 * (interChipPixelSize - pixelSize);
      pitch =
        0.5 * (interChipPixelSize + pixelSize);
      break;
    case 255:
    case 511:
      // left of chip border
      x += 0.5 * (interChipPixelSize - pixelSize);
      pitch = interChipPixelSize;
      break;
    case 254:
    case 510:
      // two left of chip border
      pitch =
        0.5 * (interChipPixelSize + pixelSize);
      break;
    }
    local_x[col] = x;
    x_pitch[col] = pitch;
  }
}

/// Printout method to stdout
void gaudi::DeVPSensorStatic::print(int indent, int flg)  const    {
  access()->print(indent,flg);
}


/// Printout method to stdout
void gaudi::detail::DeVPSensorObject::print(int indent, int flg)  const   {
  printout(INFO,"DeVPSensor", "%s+ Info: %s Noise:%s Readout:%s",
           DE::indent(indent).c_str(),
           yes_no(info.isValid()),
           yes_no(noise.isValid()),
           yes_no(readout.isValid()));
  DeIOVObject::print(indent, flg);
}

/// Initialization of sub-classes
void gaudi::detail::DeVPSensorObject::initialize()   {
  DeIOVObject::initialize();

  sensor_static = de_static;
  // We require a valid alignment object for sensors!
  checkAlignment();
  for(const auto& c : conditions)   {
    if ( c.first == DeVPSensor::key_info ) info = c.second;
    else if ( c.first == DeVPSensor::key_noise ) noise = c.second;
    else if ( c.first == DeVPSensor::key_readout ) readout = c.second;
  }
  // Check here if values must be valid ?
  if ( !info.isValid() )  {
    // except(DeVPSensor", "Invalid IOV dependent sensor info!");
  }
  if ( !noise.isValid() )  {
    // except(DeVPSensor", "Invalid IOV dependent sensor noise!");
  }
  if ( !readout.isValid() )  {
    // except(DeVPSensor", "Invalid IOV dependent sensor readout!");
  }
}

/// Printout method to stdout
void gaudi::DeVPSensor::print(int indent, int flg)  const    {
  access()->print(indent, flg);
}
