// $Id$
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

#include "DD4hep/InstanceCount.h"
#include "DD4hep/Handle.inl"
#include "XML/Evaluator.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

namespace DD4hep {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(DD4hep::evaluator());
}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

short DD4hep::_toShort(const string& value) {
  string s(value);
  size_t idx = s.find("(int)");
  if (idx != string::npos)
    s.erase(idx, 5);
  while (s[0] == ' ')
    s.erase(0, 1);
  double result = eval.evaluate(s.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return (short) result;
}

int DD4hep::_toInt(const string& value) {
  string s(value);
  size_t idx = s.find("(int)");
  if (idx != string::npos)
    s.erase(idx, 5);
  while (s[0] == ' ')
    s.erase(0, 1);
  double result = eval.evaluate(s.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return (int) result;
}

long DD4hep::_toLong(const string& value) {
  string s(value);
  size_t idx = s.find("(int)");
  if (idx != string::npos)
    s.erase(idx, 5);
  while (s[0] == ' ')
    s.erase(0, 1);
  double result = eval.evaluate(s.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return (long) result;
}

bool DD4hep::_toBool(const string& value) {
  return value == "true" || value == "yes";
}

float DD4hep::_toFloat(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return (float) result;
}

double DD4hep::_toDouble(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return result;
}

template <> short DD4hep::_multiply<short>(const string& left, const string& right) {
  return (short) _toDouble(left + "*" + right);
}

template <> unsigned short DD4hep::_multiply<unsigned short>(const string& left, const string& right) {
  return (unsigned short) _toDouble(left + "*" + right);
}

template <> int DD4hep::_multiply<int>(const string& left, const string& right) {
  return (int) _toDouble(left + "*" + right);
}

template <> unsigned int DD4hep::_multiply<unsigned int>(const string& left, const string& right) {
  return (unsigned int) _toDouble(left + "*" + right);
}

template <> long DD4hep::_multiply<long>(const string& left, const string& right) {
  return (long) _toDouble(left + "*" + right);
}

template <> unsigned long DD4hep::_multiply<unsigned long>(const string& left, const string& right) {
  return (unsigned long) _toDouble(left + "*" + right);
}

template <> float DD4hep::_multiply<float>(const string& left, const string& right) {
  return _toFloat(left + "*" + right);
}

template <> double DD4hep::_multiply<double>(const string& left, const string& right) {
  return _toDouble(left + "*" + right);
}

void DD4hep::_toDictionary(const string& name, const string& value) {
  _toDictionary(name, value, "number");
}

/// Enter name value pair to the dictionary.  \ingroup DD4HEP_GEOMETRY
void DD4hep::_toDictionary(const std::string& name, const std::string& value, const std::string& typ)   {
  if ( typ == "string" )  {
    eval.setEnviron(name.c_str(),value.c_str());
    return;
  }
  else  {
    string n = name, v = value;
    size_t idx = v.find("(int)");
    if (idx != string::npos)
      v.erase(idx, 5);
    idx = v.find("(float)");
    if (idx != string::npos)
      v.erase(idx, 7);
    while (v[0] == ' ')
      v.erase(0, 1);
    double result = eval.evaluate(v.c_str());
    if (eval.status() != XmlTools::Evaluator::OK) {
      cerr << value << ": ";
      eval.print_error();
      throw runtime_error("DD4hep: Severe error during expression evaluation " + name + "=" + value);
    }
    eval.setVariable(n.c_str(), result);
  }
}

template <typename T> static inline string __to_string(T value, const char* fmt) {
  char text[128];
  ::snprintf(text, sizeof(text), fmt, value);
  return text;
}

string DD4hep::_toString(bool value) {
  return value ? "true" : "false";
}

string DD4hep::_toString(short value, const char* fmt) {
  return __to_string((int)value, fmt);
}

string DD4hep::_toString(int value, const char* fmt) {
  return __to_string(value, fmt);
}

string DD4hep::_toString(float value, const char* fmt) {
  return __to_string(value, fmt);
}

string DD4hep::_toString(double value, const char* fmt) {
  return __to_string(value, fmt);
}

string DD4hep::_ptrToString(const void* value, const char* fmt) {
  return __to_string(value, fmt);
}

namespace DD4hep {
  static long s_numVerifies = 0;

  long num_object_validations() {
    return s_numVerifies;
  }
  void increment_object_validations() {
    ++s_numVerifies;
  }
  void warning_deprecated_xml_factory(const char* name)   {
    const char* edge = "++++++++++++++++++++++++++++++++++++++++++";
    size_t len = ::strlen(name);
    cerr << edge << edge << edge << endl;
    cerr << "++  The usage of the factory: \"" << name << "\" is DEPRECATED due to naming conventions."
         << setw(53-len) << right << "++" << endl;
    cerr << "++  Please use \"DD4hep_" << name << "\" instead." << setw(93-len) << right << "++" << endl;
    cerr << edge << edge << edge << endl;
  }
}

#include "DDSegmentation/Segmentation.h"
typedef DDSegmentation::Segmentation _Segmentation;
//INSTANTIATE_UNNAMED(_Segmentation);
namespace DD4hep {
  template <> void Handle<_Segmentation>::assign(_Segmentation* s, const std::string& n, const std::string&) {
    this->m_element = s;
    s->setName(n);
  }
  template <> const char* Handle<_Segmentation>::name() const {
    return this->m_element ? this->m_element->name().c_str() : "";
  }
  template class DD4hep::Handle<_Segmentation>;
}

#include "DD4hep/LCDD.h"
#include "TMap.h"
#include "TColor.h"

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(LCDD);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TObject);
DD4HEP_INSTANTIATE_HANDLE(TNamed);

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoElement.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoElement);
DD4HEP_INSTANTIATE_HANDLE(TGeoMaterial);
DD4HEP_INSTANTIATE_HANDLE(TGeoMedium);

#include "TGeoMatrix.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoRotation);
DD4HEP_INSTANTIATE_HANDLE(TGeoTranslation);
DD4HEP_INSTANTIATE_HANDLE(TGeoIdentity);
DD4HEP_INSTANTIATE_HANDLE(TGeoCombiTrans);
DD4HEP_INSTANTIATE_HANDLE(TGeoGenTrans);

#include "TGeoNode.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoNode);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeOffset);

// Shapes (needed by "Shapes.cpp")
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"
#include "TGeoVolume.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoVolume);
DD4HEP_INSTANTIATE_HANDLE(TGeoBBox);
DD4HEP_INSTANTIATE_HANDLE(TGeoCone);
DD4HEP_INSTANTIATE_HANDLE(TGeoArb8);
DD4HEP_INSTANTIATE_HANDLE(TGeoConeSeg);
DD4HEP_INSTANTIATE_HANDLE(MyConeSeg);
DD4HEP_INSTANTIATE_HANDLE(TGeoParaboloid);
DD4HEP_INSTANTIATE_HANDLE(TGeoPcon);
DD4HEP_INSTANTIATE_HANDLE(TGeoHype);
DD4HEP_INSTANTIATE_HANDLE(TGeoPgon);
DD4HEP_INSTANTIATE_HANDLE(TGeoTube);
DD4HEP_INSTANTIATE_HANDLE(TGeoEltu);
DD4HEP_INSTANTIATE_HANDLE(TGeoTubeSeg);
DD4HEP_INSTANTIATE_HANDLE(TGeoTrap);
DD4HEP_INSTANTIATE_HANDLE(TGeoTrd1);
DD4HEP_INSTANTIATE_HANDLE(TGeoTrd2);
DD4HEP_INSTANTIATE_HANDLE(TGeoSphere);
DD4HEP_INSTANTIATE_HANDLE(TGeoTorus);
DD4HEP_INSTANTIATE_HANDLE(TGeoHalfSpace);
DD4HEP_INSTANTIATE_HANDLE(TGeoShape);
DD4HEP_INSTANTIATE_HANDLE(TGeoShapeAssembly);
DD4HEP_INSTANTIATE_HANDLE(TGeoCompositeShape);

// Volume Placements (needed by "Volumes.cpp")
#include "TGeoPhysicalNode.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoPhysicalNode);

#include "TGeoBoolNode.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoUnion);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoIntersection);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoSubtraction);

// Replicated Volumes (needed by "Volumes.cpp")
#include "TGeoPatternFinder.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternFinder);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternTrapZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphTheta);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternHoneycomb);
