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

#include <DD4hep/detail/Handle.inl>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/Printout.h>
#include <Evaluator/Evaluator.h>

/// C/C++ include files
#include <iostream>
#include <iomanip>
#include <climits>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

namespace dd4hep {
  dd4hep::tools::Evaluator& evaluator();
}

namespace {
  const dd4hep::tools::Evaluator& eval(dd4hep::evaluator());
}

using namespace dd4hep;
using namespace dd4hep::detail;

namespace   {

  /// Set true for backwards compatibility
  static bool s_allow_variable_redefine = true;

  ///
  void check_evaluation(const std::string& value, std::pair<int,double> res, std::stringstream& err)   {
    if ( res.first != tools::Evaluator::OK) {
      throw std::runtime_error("dd4hep: "+err.str()+" : value="+value+" [Evaluation error]");
    }
  }
}

namespace dd4hep  {

  /// Steer redefinition of variable re-definition during expression evaluation. returns old value
  bool set_allow_variable_redefine(bool value)    {
    bool tmp = s_allow_variable_redefine;
    s_allow_variable_redefine = value;
    return tmp;
  }

  std::pair<int, double> _toFloatingPoint(const std::string& value)   {
    std::stringstream err;
    auto result = eval.evaluate(value, err);
    check_evaluation(value, result, err);
    return result;
  }

  std::pair<int, double> _toInteger(const std::string& value)    {
    std::string s(value);
    size_t idx = s.find("(int)");
    if (idx != std::string::npos)
      s.erase(idx, 5);
    idx = s.find("(long)");
    if (idx != std::string::npos)
      s.erase(idx, 6);
    while (s[0] == ' ')
      s.erase(0, 1);
    return _toFloatingPoint(s);
  }

  short _toShort(const std::string& value) {
    return (short) _toInteger(value).second;
  }

  unsigned short _toUShort(const std::string& value) {
    return (unsigned short) _toInteger(value).second;
  }

  int _toInt(const std::string& value) {
    return (int) _toInteger(value).second;
  }

  unsigned int _toUInt(const std::string& value) {
    return (unsigned int) _toInteger(value).second;
  }

  long _toLong(const std::string& value) {
    return (long) _toInteger(value).second;
  }

  unsigned long _toULong(const std::string& value) {
    return (unsigned long) _toInteger(value).second;
  }

  bool _toBool(const std::string& value) {
    return value == "true" || value == "yes" || value == "True";
  }

  /// String conversions: string to float value
  float _toFloat(const std::string& value) {
    return (float) _toFloatingPoint(value).second;
  }

  /// String conversions: string to double value
  double _toDouble(const std::string& value) {
    return _toFloatingPoint(value).second;
  }

  /// Generic type conversion from string to primitive value  \ingroup DD4HEP_CORE
  template <typename T> T _toType(const std::string& value)    {
    notImplemented("Value "+value+" cannot be converted to type "+typeName(typeid(T)));
    return T();
  }

  /// Generic type conversion from string to primitive value
  template <> bool _toType<bool>(const std::string& value)  {
    return _toBool(value);
  }

  /// Generic type conversion from string to primitive value
  template <> short _toType<short>(const std::string& value)  {
    return _toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned short _toType<unsigned short>(const std::string& value)  {
    return (unsigned short)_toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> int _toType<int>(const std::string& value)  {
    return _toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned int _toType<unsigned int>(const std::string& value)  {
    return (unsigned int)_toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> long _toType<long>(const std::string& value)  {
    return _toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned long _toType<unsigned long>(const std::string& value)  {
    return (unsigned long)_toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> float _toType<float>(const std::string& value)  {
    return _toFloat(value);
  }

  /// Generic type conversion from string to primitive value
  template <> double _toType<double>(const std::string& value)  {
    return _toDouble(value);
  }

  /// Generic type conversion from string to primitive value
  template <> std::string _toType<std::string>(const std::string& value)  {
    return value;
  }

  template <> char _multiply<char>(const std::string& left, const std::string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SCHAR_MIN) && val <= double(SCHAR_MAX) )
      return (char) (int)val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> unsigned char _multiply<unsigned char>(const std::string& left, const std::string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(UCHAR_MAX) )
      return (unsigned char) (int)val;
    except("_multiply<unsigned char>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> short _multiply<short>(const std::string& left, const std::string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SHRT_MIN) && val <= double(SHRT_MAX) )
      return (short) val;
    except("_multiply<short>",
           "Multiplication %e = %s * %s out of bounds for conversion to short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> unsigned short _multiply<unsigned short>(const std::string& left, const std::string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(USHRT_MAX) )
      return (unsigned short)val;
    except("_multiply<unsigned short>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> int _multiply<int>(const std::string& left, const std::string& right) {
    return (int) _toDouble(left + "*" + right);
  }

  template <> unsigned int _multiply<unsigned int>(const std::string& left, const std::string& right) {
    return (unsigned int) _toDouble(left + "*" + right);
  }

  template <> long _multiply<long>(const std::string& left, const std::string& right) {
    return (long) _toDouble(left + "*" + right);
  }

  template <> unsigned long _multiply<unsigned long>(const std::string& left, const std::string& right) {
    return (unsigned long) _toDouble(left + "*" + right);
  }

  template <> float _multiply<float>(const std::string& left, const std::string& right) {
    return _toFloat(left + "*" + right);
  }

  template <> double _multiply<double>(const std::string& left, const std::string& right) {
    return _toDouble(left + "*" + right);
  }

  void _toDictionary(const std::string& name, const std::string& value) {
    _toDictionary(name, value, "number");
  }

  /// Enter name value pair to the dictionary.  \ingroup DD4HEP_CORE
  void _toDictionary(const std::string& name, const std::string& value, const std::string& typ)   {
    if ( typ == "string" )  {
      eval.setEnviron(name.c_str(),value.c_str());
      return;
    }
    else  {
      int status;
      std::stringstream err;
      std::string n = name, v = value;
      size_t idx = v.find("(int)");
      if (idx != std::string::npos)
        v.erase(idx, 5);
      idx = v.find("(float)");
      if (idx != std::string::npos)
        v.erase(idx, 7);
      while (v[0] == ' ')
        v.erase(0, 1);
      auto result = eval.evaluate(v, err);
      check_evaluation(v, result, err);
      err.str("");
      status = eval.setVariable(n, result.second, err);
      if ( status != tools::Evaluator::OK )   {
        std::stringstream err_msg;
        err_msg << "name=" << name << " value=" << value
                << "  " << err.str() << " [setVariable error]";
        if ( status == tools::Evaluator::WARNING_EXISTING_VARIABLE )   {
          if ( s_allow_variable_redefine )
            printout(WARNING,"Evaluator","+++ Overwriting variable: "+err_msg.str());
          else
            except("Evaluator","+++ Overwriting variable: "+err_msg.str());
        }
      }
    }
  }

  /// Evaluate string constant using environment stored in the evaluator
  std::string _getEnviron(const std::string& env)   {    
    // We are trying to deal with the case when several variables are being replaced in the string.   
    size_t current_index = 0;
    std::stringstream processed_variable;
    while (true) {
      // Looking for the start of a variable use, with the syntax
      // "path1/path2/${VAR1}/path3/${VAR2}"
      size_t id1 = env.find("${", current_index);
      // variable start found, do a greedy search for the variable end 
      if (id1 == std::string::npos) {
        // In this case we did not find the ${ to indicate a start of variable,
        // we just copy the rest of the variable to the stringstream and exit
        processed_variable << env.substr(current_index);
        break;
      }
      size_t id2 = env.find("}", id1);
      if (id2 == std::string::npos) {
        std::runtime_error("dd4hep: Syntax error, bad variable syntax: " + env); 
      }
      processed_variable << env.substr(current_index, id1 -current_index );
      std::string v   = env.substr(id1, id2-id1+1);
      std::stringstream err;
      auto   ret = eval.getEnviron(v, err);
      // Checking that the variable lookup worked
      if ( ret.first != tools::Evaluator::OK) {
        std::cerr << v << ": " << err.str() << std::endl;
        throw std::runtime_error("dd4hep: Severe error during environment lookup of " + v + " " + err.str());
      }
      // Now adding the variable
      processed_variable << ret.second;
      current_index = id2 + 1;       
    }
    return processed_variable.str();
  }

  /// String manipulations: Remove unconditionally all white spaces
  std::string remove_whitespace(const std::string& v)    {
    std::string value;
    value.reserve(v.length()+1);
    for(const char* p = v.c_str(); *p; ++p)   {
      if ( !::isspace(*p) ) value += *p;
    }
    return value;
  }

  template <typename T> static inline std::string __to_string(T value, const char* fmt) {
    char text[128];
    ::snprintf(text, sizeof(text), fmt, value);
    return text;
  }

  std::string _toString(bool value) {
    return value ? "true" : "false";
  }

  std::string _toString(short value, const char* fmt) {
    return __to_string((int)value, fmt);
  }

  std::string _toString(int value, const char* fmt) {
    return __to_string(value, fmt);
  }

  std::string _toString(unsigned long value, const char* fmt) {
    return __to_string(value, fmt);
  }

  std::string _toString(float value, const char* fmt) {
    return __to_string(value, fmt);
  }

  std::string _toString(double value, const char* fmt) {
    return __to_string(value, fmt);
  }

  std::string _ptrToString(const void* value, const char* fmt) {
    return __to_string(value, fmt);
  }

  // ==================================================================================
  static long s_numVerifies = 0;

  long num_object_validations() {
    return s_numVerifies;
  }
  void increment_object_validations() {
    ++s_numVerifies;
  }
  void warning_deprecated_xml_factory(const char* name)   {
    const char* edge = "++++++++++++++++++++++++++++++++++++++++++";
    size_t len = std::strlen(name);
    std::cerr << edge << edge << edge << std::endl;
    std::cerr << "++  The usage of the factory: \"" << name << "\" is DEPRECATED due to naming conventions."
              << std::setw(53-len) << std::right << "++" << std::endl;
    std::cerr << "++  Please use \"DD4hep_" << name << "\" instead." << std::setw(93-len) << std::right << "++" << std::endl;
    std::cerr << edge << edge << edge << std::endl;
  }
}

#include "DDSegmentation/Segmentation.h"
typedef DDSegmentation::Segmentation _Segmentation;
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

#include <DD4hep/Detector.h>
#include <TMap.h>
#include <TColor.h>

template class dd4hep::Handle<NamedObject>;
DD4HEP_SAFE_CAST_IMPLEMENTATION(NamedObject,NamedObject)

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(Detector);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TObject,NamedObject);
DD4HEP_INSTANTIATE_HANDLE_CODE(NONE,TNamed,TObject,NamedObject);

#include <TGeoMedium.h>
#include <TGeoMaterial.h>
#include <TGeoElement.h>
DD4HEP_INSTANTIATE_HANDLE(TGeoElement);
DD4HEP_INSTANTIATE_HANDLE(TGeoMaterial);
DD4HEP_INSTANTIATE_HANDLE(TGeoMedium);

#include <TGeoMatrix.h>
DD4HEP_INSTANTIATE_HANDLE(TGeoMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoRotation);
DD4HEP_INSTANTIATE_HANDLE(TGeoTranslation);
DD4HEP_INSTANTIATE_HANDLE(TGeoIdentity);
DD4HEP_INSTANTIATE_HANDLE(TGeoCombiTrans);
DD4HEP_INSTANTIATE_HANDLE(TGeoGenTrans);

#include <TGeoNode.h>
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAttLine);
DD4HEP_INSTANTIATE_HANDLE(TGeoNode,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeOffset);

// Shapes (needed by "Shapes.cpp")
#include <TGeoBBox.h>
#include <TGeoPcon.h>
#include <TGeoPgon.h>
#include <TGeoTube.h>
#include <TGeoCone.h>
#include <TGeoArb8.h>
#include <TGeoTrd1.h>
#include <TGeoTrd2.h>
#include <TGeoParaboloid.h>
#include <TGeoSphere.h>
#include <TGeoTorus.h>
#include <TGeoTessellated.h>
#include <TGeoBoolNode.h>
#include <TGeoVolume.h>
#include <TGeoScaledShape.h>
#include <TGeoCompositeShape.h>
#include <TGeoShapeAssembly.h>
#include <DD4hep/detail/ShapesInterna.h>
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeAssembly,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeMulti,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolume,TGeoAtt,TAttLine,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE(TGeoShape);
DD4HEP_INSTANTIATE_HANDLE(TGeoBBox,TGeoShape);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCone);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoConeSeg);
//DD4HEP_INSTANTIATE_SHAPE_HANDLE(MyConeSeg);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoParaboloid);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPgon,TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoXtru);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoScaledShape);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHype,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoEltu,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTubeSeg,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCtub,TGeoTubeSeg,TGeoTube);
using dd4hep::TwistedTubeObject;
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TwistedTubeObject,TGeoTubeSeg);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrap,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoGtra,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd1);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd2);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoSphere);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTorus);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTessellated);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHalfSpace);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoShapeAssembly);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCompositeShape);

// Volume Placements (needed by "Volumes.cpp")
#include <TGeoPhysicalNode.h>
DD4HEP_INSTANTIATE_HANDLE(TGeoPhysicalNode);

#include <TGeoBoolNode.h>
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoUnion);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoIntersection);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoSubtraction);

// Replicated Volumes (needed by "Volumes.cpp")
#include <TGeoPatternFinder.h>
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
