// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/InstanceCount.h"
#include "DD4hep/Handle.h"
#include "XML/Evaluator.h"
#include <iostream>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

class TObject;

namespace DD4hep {
  XmlTools::Evaluator& evaluator();
}

namespace {
  XmlTools::Evaluator& eval(DD4hep::evaluator());
}

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

int DD4hep::Geometry::_toInt(const string& value) {
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

long DD4hep::Geometry::_toLong(const string& value) {
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

bool DD4hep::Geometry::_toBool(const string& value) {
  return value == "true";
}

float DD4hep::Geometry::_toFloat(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return (float) result;
}

double DD4hep::Geometry::_toDouble(const string& value) {
  double result = eval.evaluate(value.c_str());
  if (eval.status() != XmlTools::Evaluator::OK) {
    cerr << value << ": ";
    eval.print_error();
    throw runtime_error("DD4hep: Severe error during expression evaluation of " + value);
  }
  return result;
}

template <> int DD4hep::Geometry::_multiply<int>(const string& left, const string& right) {
  return (int) _toDouble(left + "*" + right);
}

template <> long DD4hep::Geometry::_multiply<long>(const string& left, const string& right) {
  return (long) _toDouble(left + "*" + right);
}

template <> float DD4hep::Geometry::_multiply<float>(const string& left, const string& right) {
  return _toFloat(left + "*" + right);
}

template <> double DD4hep::Geometry::_multiply<double>(const string& left, const string& right) {
  return _toDouble(left + "*" + right);
}

void DD4hep::Geometry::_toDictionary(const string& name, const string& value) {
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

string DD4hep::Geometry::_toString(bool value) {
  char text[32];
  ::snprintf(text, sizeof(text), "%s", value ? "true" : "false");
  return text;
}

string DD4hep::Geometry::_toString(int value) {
  char text[32];
  ::snprintf(text, sizeof(text), "%d", value);
  return text;
}

string DD4hep::Geometry::_toString(float value) {
  char text[32];
  ::snprintf(text, sizeof(text), "%f", value);
  return text;
}

string DD4hep::Geometry::_toString(double value) {
  char text[32];
  ::snprintf(text, sizeof(text), "%f", value);
  return text;
}
namespace DD4hep {
  namespace Geometry {
    static long s_numVerifies = 0;

    long num_object_validations() {
      return s_numVerifies;
    }
    void increment_object_validations() {
      ++s_numVerifies;
    }

    template <typename T> void Handle<T>::bad_assignment(const type_info& from, const type_info& to) {
      string msg = "Wrong assingment from ";
      msg += from.name();
      msg += " to ";
      msg += to.name();
      msg += " not possible!!";
      throw runtime_error(msg);
    }
    template <typename T> void Handle<T>::assign(T* n, const string& nam, const string& tit) {
      this->m_element = n;
      if (!nam.empty())
        n->SetName(nam.c_str());
      if (!tit.empty())
        n->SetTitle(tit.c_str());
    }

    template <typename T> const char* Handle<T>::name() const {
      return this->m_element ? this->m_element->GetName() : "";
    }

    template <> const char* Handle<TObject>::name() const {
      return "";
    }

    template <> void Handle<TObject>::bad_assignment(const type_info& from, const type_info& to) {
      string msg = "Wrong assingment from ";
      msg += from.name();
      msg += " to ";
      msg += to.name();
      msg += " not possible!!";
      throw runtime_error(msg);
    }
  }
}

static const std::string __typeinfoName(const std::type_info& tinfo) {
  const char* class_name = tinfo.name();
  std::string result;
#ifdef WIN32
  size_t off = 0;
  if ( ::strncmp(class_name, "class ", 6) == 0 ) {
    // The returned name is prefixed with "class "
    off = 6;
  }
  if ( ::strncmp(class_name, "struct ", 7) == 0 ) {
    // The returned name is prefixed with "struct "
    off = 7;
  }
  if ( off != std::string::npos ) {
    std::string tmp = class_name + off;
    size_t loc = 0;
    while( (loc = tmp.find("class ")) != std::string::npos ) {
      tmp.erase(loc, 6);
    }
    loc = 0;
    while( (loc = tmp.find("struct ")) != std::string::npos ) {
      tmp.erase(loc, 7);
    }
    result = tmp;
  }
  else {
    result = class_name;
  }
  // Change any " *" to "*"
  while ( (off=result.find(" *")) != std::string::npos ) {
    result.replace(off, 2, "*");
  }
  // Change any " &" to "&"
  while ( (off=result.find(" &")) != std::string::npos ) {
    result.replace(off, 2, "&");
  }
#elif defined(sun)
  result = class_name;
#elif !defined(__ICC)
  if (::strlen(class_name) == 1) {
    // See http://www.realitydiluted.com/mirrors/reality.sgi.com/dehnert_engr/cxx/abi.pdf
    // for details
    switch (class_name[0]) {
    case 'v':
      result = "void";
      break;
    case 'w':
      result = "wchar_t";
      break;
    case 'b':
      result = "bool";
      break;
    case 'c':
      result = "char";
      break;
    case 'h':
      result = "unsigned char";
      break;
    case 's':
      result = "short";
      break;
    case 't':
      result = "unsigned short";
      break;
    case 'i':
      result = "int";
      break;
    case 'j':
      result = "unsigned int";
      break;
    case 'l':
      result = "long";
      break;
    case 'm':
      result = "unsigned long";
      break;
    case 'x':
      result = "long long";
      break;
    case 'y':
      result = "unsigned long long";
      break;
    case 'n':
      result = "__int128";
      break;
    case 'o':
      result = "unsigned __int128";
      break;
    case 'f':
      result = "float";
      break;
    case 'd':
      result = "double";
      break;
    case 'e':
      result = "long double";
      break;
    case 'g':
      result = "__float128";
      break;
    case 'z':
      result = "ellipsis";
      break;
    }
  }
  else {
    char buff[16 * 1024];
    size_t len = sizeof(buff);
    int status = 0;
    result = __cxxabiv1::__cxa_demangle(class_name, buff, &len, &status);
  }
#else
  result = class_name;
  throw std::runtime_error("CXXABI is missing for ICC!");
#endif
  return result;
}

string DD4hep::typeName(const type_info& typ) {
  return __typeinfoName(typ);
}

#include "DDSegmentation/Segmentation.h"
typedef DDSegmentation::Segmentation _Segmentation;
//INSTANTIATE_UNNAMED(_Segmentation);
namespace DD4hep {
  namespace Geometry {
    template <> void Handle<_Segmentation>::assign(_Segmentation* s, const std::string& n, const std::string& t) {
      this->m_element = s;
      s->setName(n);
    }
    template <> const char* Handle<_Segmentation>::name() const {
      return this->m_element ? this->m_element->name().c_str() : "";
    }
    template struct DD4hep::Geometry::Handle<_Segmentation>;
  }
}

#include "DD4hep/LCDD.h"
#include "TMap.h"
#include "TColor.h"

#define INSTANTIATE(X)   template struct DD4hep::Geometry::Handle<X>
#define INSTANTIATE_UNNAMED(X)   namespace DD4hep { namespace Geometry {                         \
  template <> void Handle<X>::assign(X* n, const string&, const string&) { this->m_element = n;} \
  template <> const char* Handle<X>::name() const { return ""; }	                         \
}}									                         \
template struct DD4hep::Geometry::Handle<X>

INSTANTIATE(TNamed);

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoElement.h"
INSTANTIATE(TGeoElement);
INSTANTIATE(TGeoMaterial);
INSTANTIATE(TGeoMedium);

#include "TGeoMatrix.h"
INSTANTIATE(TGeoMatrix);
INSTANTIATE(TGeoRotation);
INSTANTIATE(TGeoTranslation);
INSTANTIATE(TGeoIdentity);
INSTANTIATE(TGeoCombiTrans);
INSTANTIATE(TGeoGenTrans);

#include "TGeoNode.h"
INSTANTIATE(TGeoNode);
INSTANTIATE(TGeoNodeMatrix);
INSTANTIATE(TGeoNodeOffset);

// Shapes (needed by "Shapes.cpp")
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"
#include "TGeoVolume.h"
#include "TGeoCompositeShape.h"
INSTANTIATE(TGeoVolume);
INSTANTIATE(TGeoBBox);
INSTANTIATE(TGeoCone);
INSTANTIATE(TGeoConeSeg);
INSTANTIATE(MyConeSeg);
INSTANTIATE(TGeoParaboloid);
INSTANTIATE(TGeoPcon);
INSTANTIATE(TGeoPgon);
INSTANTIATE(TGeoTube);
INSTANTIATE(TGeoTubeSeg);
INSTANTIATE(TGeoTrap);
INSTANTIATE(TGeoTrd2);
INSTANTIATE(TGeoSphere);
INSTANTIATE(TGeoTorus);
INSTANTIATE(TGeoShape);
INSTANTIATE(TGeoCompositeShape);

// Volume Placements (needed by "Volumes.cpp")
#include "TGeoPhysicalNode.h"
INSTANTIATE_UNNAMED(TGeoPhysicalNode);

// Replicated Volumes (needed by "Volumes.cpp")
#include "TGeoPatternFinder.h"
INSTANTIATE_UNNAMED(TGeoPatternFinder);
INSTANTIATE_UNNAMED(TGeoPatternX);
INSTANTIATE_UNNAMED(TGeoPatternY);
INSTANTIATE_UNNAMED(TGeoPatternZ);
INSTANTIATE_UNNAMED(TGeoPatternParaX);
INSTANTIATE_UNNAMED(TGeoPatternParaY);
INSTANTIATE_UNNAMED(TGeoPatternParaZ);
INSTANTIATE_UNNAMED(TGeoPatternTrapZ);
INSTANTIATE_UNNAMED(TGeoPatternCylR);
INSTANTIATE_UNNAMED(TGeoPatternCylPhi);
INSTANTIATE_UNNAMED(TGeoPatternSphR);
INSTANTIATE_UNNAMED(TGeoPatternSphTheta);
INSTANTIATE_UNNAMED(TGeoPatternSphPhi);
INSTANTIATE_UNNAMED(TGeoPatternHoneycomb);
