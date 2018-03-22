#ifndef KERNEL_VPCONSTANTS_H
#define KERNEL_VPCONSTANTS_H 1
namespace VP {

  static const unsigned int NModules = 52;
  static const unsigned int NSensorsPerModule = 4;
  static const unsigned int NSensors = NModules * NSensorsPerModule;
  static const unsigned int NChipsPerSensor = 3;
  static const unsigned int NRows = 256;
  static const unsigned int NColumns = 256;
  static const unsigned int NSensorColumns = NColumns * NChipsPerSensor; 
  static const unsigned int NPixelsPerSensor = NSensorColumns * NRows;

  static const double Pitch = 0.055;

  enum UserFlags {
    LEFT    =    0,
    RIGHT   = 1<<0,
    MAIN    = 1<<1,
    SIDE    = 1<<2,
    SUPPORT = 1<<3,
    MODULE  = 1<<4,
    LADDER  = 1<<5,
    SENSOR  = 1<<6
  };
  
}

#endif
