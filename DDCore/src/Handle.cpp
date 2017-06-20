//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/Printout.h"
#include "DD4hep/detail/Handle.inl"
#include "DDParsers/Evaluator.h"
#include <iostream>
#include <iomanip>
#include <climits>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

namespace dd4hep {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(dd4hep::evaluator());
}

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

short dd4hep::_toShort(const string& value) {
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
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + value);
  }
  return (short) result;
}

int dd4hep::_toInt(const string& value) {
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
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + value);
  }
  return (int) result;
}

long dd4hep::_toLong(const string& value) {
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
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + value);
  }
  return (long) result;
}

bool dd4hep::_toBool(const string& value) {
  return value == "true" || value == "yes";
}

float dd4hep::_toFloat(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + value);
  }
  return (float) result;
}

double dd4hep::_toDouble(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("dd4hep: Severe error during expression evaluation of " + value);
  }
  return result;
}

template <> char dd4hep::_multiply<char>(const string& left, const string& right) {
  double val = _toDouble(left + "*" + right);
  if ( val >= double(SCHAR_MIN) && val <= double(SCHAR_MAX) )
    return (char) (int)val;
  except("_multiply<char>",
         "Multiplication %e = %s * %s out of bounds for conversion to char.",
         val, left.c_str(), right.c_str());
  return 0;
}

template <> unsigned char dd4hep::_multiply<unsigned char>(const string& left, const string& right) {
  double val = _toDouble(left + "*" + right);
  if ( val >= 0 && val <= double(UCHAR_MAX) )
    return (unsigned char) (int)val;
  except("_multiply<char>",
         "Multiplication %e = %s * %s out of bounds for conversion to unsigned char.",
         val, left.c_str(), right.c_str());
  return 0;
}

template <> short dd4hep::_multiply<short>(const string& left, const string& right) {
  double val = _toDouble(left + "*" + right);
  if ( val >= double(SHRT_MIN) && val <= double(SHRT_MAX) )
    return (short) val;
  except("_multiply<char>",
         "Multiplication %e = %s * %s out of bounds for conversion to short.",
         val, left.c_str(), right.c_str());
  return 0;
}

template <> unsigned short dd4hep::_multiply<unsigned short>(const string& left, const string& right) {
  double val = _toDouble(left + "*" + right);
  if ( val >= 0 && val <= double(USHRT_MAX) )
    return (unsigned short)val;
  except("_multiply<char>",
         "Multiplication %e = %s * %s out of bounds for conversion to unsigned short.",
         val, left.c_str(), right.c_str());
  return 0;
}

template <> int dd4hep::_multiply<int>(const string& left, const string& right) {
  return (int) _toDouble(left + "*" + right);
}

template <> unsigned int dd4hep::_multiply<unsigned int>(const string& left, const string& right) {
  return (unsigned int) _toDouble(left + "*" + right);
}

template <> long dd4hep::_multiply<long>(const string& left, const string& right) {
  return (long) _toDouble(left + "*" + right);
}

template <> unsigned long dd4hep::_multiply<unsigned long>(const string& left, const string& right) {
  return (unsigned long) _toDouble(left + "*" + right);
}

template <> float dd4hep::_multiply<float>(const string& left, const string& right) {
  return _toFloat(left + "*" + right);
}

template <> double dd4hep::_multiply<double>(const string& left, const string& right) {
  return _toDouble(left + "*" + right);
}

void dd4hep::_toDictionary(const string& name, const string& value) {
  _toDictionary(name, value, "number");
}

/// Enter name value pair to the dictionary.  \ingroup DD4HEP_GEOMETRY
void dd4hep::_toDictionary(const std::string& name, const std::string& value, const std::string& typ)   {
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
      throw runtime_error("dd4hep: Severe error during expression evaluation " + name + "=" + value);
    }
    eval.setVariable(n.c_str(), result);
  }
}

template <typename T> static inline string __to_string(T value, const char* fmt) {
  char text[128];
  ::snprintf(text, sizeof(text), fmt, value);
  return text;
}

string dd4hep::_toString(bool value) {
  return value ? "true" : "false";
}

string dd4hep::_toString(short value, const char* fmt) {
  return __to_string((int)value, fmt);
}

string dd4hep::_toString(int value, const char* fmt) {
  return __to_string(value, fmt);
}

string dd4hep::_toString(float value, const char* fmt) {
  return __to_string(value, fmt);
}

string dd4hep::_toString(double value, const char* fmt) {
  return __to_string(value, fmt);
}

string dd4hep::_ptrToString(const void* value, const char* fmt) {
  return __to_string(value, fmt);
}

namespace dd4hep {
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
namespace dd4hep {
  template <> void Handle<_Segmentation>::assign(_Segmentation* s, const std::string& n, const std::string&) {
    this->m_element = s;
    s->setName(n);
  }
  template <> const char* Handle<_Segmentation>::name() const {
    return this->m_element ? this->m_element->name().c_str() : "";
  }
  template class dd4hep::Handle<_Segmentation>;
}

#include "DD4hep/Detector.h"
#include "TMap.h"
#include "TColor.h"

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(Detector);
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
