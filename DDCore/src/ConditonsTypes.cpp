// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/objects/ConditionsInterna.h"

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

// C/C++ include files
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

DD4HEP_DEFINE_CONDITIONS_U_CONT(char)
DD4HEP_DEFINE_CONDITIONS_U_CONT(short)
  DD4HEP_DEFINE_CONDITIONS_U_CONT(int)
  DD4HEP_DEFINE_CONDITIONS_U_CONT(long)
  DD4HEP_DEFINE_CONDITIONS_U_CONT(long long)

  DD4HEP_DEFINE_CONDITIONS_CONT(bool)
  DD4HEP_DEFINE_CONDITIONS_CONT(float)
  DD4HEP_DEFINE_CONDITIONS_CONT(double)

  DD4HEP_DEFINE_CONDITIONS_TYPE(std::string)
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::vector<std::string>)
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::list<std::string>)
  DD4HEP_DEFINE_CONDITIONS_TYPE(std::set<std::string>)

// ROOT::Math Object instances
  typedef std::map<std::string, int> map_string_int;
DD4HEP_DEFINE_CONDITIONS_TYPE(map_string_int)
DD4HEP_DEFINE_CONDITIONS_TYPE(ROOT::Math::XYZPoint)
  DD4HEP_DEFINE_CONDITIONS_TYPE(ROOT::Math::XYZVector)
  DD4HEP_DEFINE_CONDITIONS_TYPE(ROOT::Math::PxPyPzEVector)
