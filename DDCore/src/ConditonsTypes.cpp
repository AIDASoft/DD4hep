// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Primitives.h"
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
#include <deque>

DD4HEP_DEFINE_CONDITIONS_U_CONT(char)
DD4HEP_DEFINE_CONDITIONS_U_CONT(short)
DD4HEP_DEFINE_CONDITIONS_U_CONT(int)
DD4HEP_DEFINE_CONDITIONS_U_CONT(long)
DD4HEP_DEFINE_CONDITIONS_U_CONT(long long)

DD4HEP_DEFINE_CONDITIONS_CONT(bool)
DD4HEP_DEFINE_CONDITIONS_CONT(float)
DD4HEP_DEFINE_CONDITIONS_CONT(double)
DD4HEP_DEFINE_CONDITIONS_CONT(std::string)

// ROOT::Math Object instances
DD4HEP_DEFINE_CONDITIONS_CONT(ROOT::Math::XYZPoint)
DD4HEP_DEFINE_CONDITIONS_CONT(ROOT::Math::XYZVector)
DD4HEP_DEFINE_CONDITIONS_CONT(ROOT::Math::PxPyPzEVector)
