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
//
// DDCMS is a detector description convention developed by the CMS experiment.
//
//==========================================================================

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/BasicGrammar.h"
#include "DDCMS/DDCMS.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cms;

Rotation3D dd4hep::cms::make_rotation3D(double thetaX, double phiX,
                                          double thetaY, double phiY,
                                          double thetaZ, double phiZ)   {
  Position  posX(sin(thetaX) * cos(phiX), sin(thetaX) * sin(phiX), cos(thetaX));
  Position  posY(sin(thetaY) * cos(phiY), sin(thetaY) * sin(phiY), cos(thetaY));
  Position  posZ(sin(thetaZ) * cos(phiZ), sin(thetaZ) * sin(phiZ), cos(thetaZ));
  Rotation3D rot(posX,posY,posZ);
  return rot;
}

Volume ParsingContext::volume(const std::string& name)  const   {
  auto i = volumes.find(name);
  if ( i != volumes.end() ) return (*i).second;
  throw runtime_error("Unknown volume identifier:"+name);
}

const Rotation3D& ParsingContext::rotation(const std::string& name)  const   {
  static Rotation3D s_null;
  auto i = rotations.find(name);
  if ( i != rotations.end() ) return (*i).second;
  if ( name.find("_NULL") == name.length()-5 ) return s_null;
  throw runtime_error("Unknown rotation identifier:"+name);
}

/// Initializing constructor
Namespace::Namespace(ParsingContext* ctx, xml_h element) : context(ctx)  {
  xml_dim_t elt(element);
  bool has_label = elt.hasAttr(_U(label));
  name = has_label ? elt.labelStr() : "";
  if ( !has_label )  {
    if ( !context->namespaces.empty() )  {
      name = context->namespaces.back();
    }
    printout(context->debug_namespaces ? ALWAYS : DEBUG,
             "DDCMS","+++ Current namespace is now: %s",name.c_str());
    return;
  }
  name = name.substr(0,name.find('.'));
  context->namespaces.push_back(name.empty() ? name : name+'_');
  pop = true;
  printout(context->debug_namespaces ? ALWAYS : DEBUG,
           "DDCMS","+++ Current namespace is now: %s",name.c_str());
}

/// Initializing constructor
Namespace::Namespace(ParsingContext* ctx) : context(ctx)  {
  name = context->namespaces.back();
}

/// Initializing constructor
Namespace::Namespace(ParsingContext& ctx) : context(&ctx)  {
  name = context->namespaces.back();
}

/// Standard destructor (Non virtual!)
Namespace::~Namespace()   {
  if ( pop )  {
    context->namespaces.pop_back();
    printout(context->debug_namespaces ? ALWAYS : DEBUG,
             "DDCMS","+++ Current namespace is now: %s",context->ns().c_str());
  }
}

/// Prepend name with namespace
std::string Namespace::prepend(const std::string& n)  const   {
  return name + n;
}

/// Resolve namespace during XML parsing
std::string Namespace::real_name(const std::string& v)  const  {
  size_t idx, idq, idp;
  string val = v;
  while ( (idx=val.find('[')) != string::npos )  {
    val.erase(idx,1);
    idp = val.find(':');
    idq = val.find(']');
    val.erase(idq,1);
    if ( idp == string::npos || idp > idq )
      val.insert(idx,name);
    else if ( idp != string::npos && idp < idq )
      val[idp] = '_';
  }
  while ( (idx=val.find(':')) != string::npos ) val[idx]='_';
  return val;
}


AlgoArguments::AlgoArguments(ParsingContext& ctxt, xml_h elt)
  : context(ctxt), element(elt)
{
  name = xml_dim_t(element).nameStr();
}

/// Access value of rParent child node
std::string AlgoArguments::parentName()  const    {
  Namespace     ns(context);
  xml_dim_t e(element);
  string val = ns.real_name(xml_dim_t(e.child(_CMU(rParent))).nameStr());
  return val;
}

/// Check the existence of an argument by name
bool AlgoArguments::find(const std::string& nam)  const   {
  for(xml_coll_t p(element,_U(star)); p; ++p)  {
    string n = p.attr<string>(_U(name));
    if ( n == nam )  {
      return true;
    }
  }
  return false;
}

/// Access raw argument as a string by name
xml_h AlgoArguments::raw_arg(const string& nam)  const   {
  for(xml_coll_t p(element,_U(star)); p; ++p)  {
    string n = p.attr<string>(_U(name));
    if ( n == nam )  {
      return p;
    }
  }
  throw runtime_error("DDCMS: Attempt to access non-existing algorithm option: " + name);
}

/// Access namespace resolved argument as a string by name
string AlgoArguments::resolved_scalar_arg(const string& nam)  const   {
  Namespace ns(context);
  xml_h  arg = raw_arg(nam);
  string val = arg.attr<string>(_U(value));
  return ns.real_name(val);
}

namespace {

  /// Access of raw strings as vector by argument name
  vector<string> raw_vector(const AlgoArguments* a, xml_h arg)   {
    xml_dim_t xp(arg);
    vector<string> data;
    Namespace ns(a->context);
    string val = xp.text();
    string nam = xp.nameStr();
    string typ = xp.typeStr();
    int    num = xp.attr<int>(_CMU(nEntries));
    const BasicGrammar& gr = BasicGrammar::instance<vector<string> >();

    val = '['+ns.real_name(val)+']';
    val = remove_whitespace(val);
    int res = gr.fromString(&data,val);
    if ( !res )  {
      except("DDCMS","+++ VectorParam<%s>: %s -> %s [Invalid conversion:%d]",
             typ.c_str(), nam.c_str(), val.c_str(), res);
    }
    else if ( num != (int)data.size() )  {
      except("DDCMS","+++ VectorParam<%s>: %s -> %s [Invalid entry count: %d <> %ld]",
             typ.c_str(), nam.c_str(), val.c_str(), num, data.size());
    }
    printout(INFO,"DDCMS","+++ VectorParam<%s>: ret=%d %s -> %s",
             typ.c_str(), res, nam.c_str(), gr.str(&data).c_str());
    return data;
  }


  template <typename T> T __cnv(const string&)       { return 0;}
  template <> double __cnv<double>(const string& arg)   { return _toDouble(arg); }
  template <> float  __cnv<float> (const string& arg)   { return _toFloat(arg); }
  template <> long   __cnv<long>  (const string& arg)   { return _toLong(arg); }
  template <> int    __cnv<int>   (const string& arg)   { return _toInt(arg);  }
  template <> string __cnv<string>(const string& arg)   { return arg;  }

  template <typename T> vector<T> __cnvVect(const AlgoArguments* a, const char* req_typ, xml_h xp)   {
    vector<string> str_data = raw_vector(a, xp);
    string typ = xp.attr<string>(_U(type));
    if ( typ != req_typ )   {
      string nam = xp.attr<string>(_U(name));
      const BasicGrammar& gr = BasicGrammar::instance<vector<string> >();
      except("DDCMS",
             "+++ VectorParam<%s | %s>: %s -> <%s> %s [Incompatible vector-type]",
             req_typ, typ.c_str(), nam.c_str(), typeName(typeid(T)).c_str(),
             gr.str(&str_data).c_str());
    }
    vector<T> res_data;
    for(const auto& v : str_data)  {
      T d = __cnv<T>(v);
      res_data.push_back(d);
    }
    return res_data;
  }
}

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of DDCMS conversion namespace
  namespace cms  {

    /// Access typed argument by name
    template<typename T> T AlgoArguments::value(const string& nam)  const   {
      return __cnv<T>(resolved_scalar_arg(nam));
    }

    template double AlgoArguments::value<double>(const string& nam)  const;
    template float  AlgoArguments::value<float>(const string& nam)  const;
    template long   AlgoArguments::value<long>(const string& nam)  const;
    template int    AlgoArguments::value<int>(const string& nam)  const;
    template string AlgoArguments::value<string>(const string& nam)  const;

    /// Access typed vector<string> argument by name
    template<> vector<string> AlgoArguments::value<vector<string> >(const string& nam)  const   {
      xml_h xp = raw_arg(nam);
      return raw_vector(this,xp);    
    }

    /// Access typed vector<double> argument by name
    template<> vector<double> AlgoArguments::value<vector<double> >(const string& nam)  const   {
      return __cnvVect<double>(this,"numeric",raw_arg(nam));
    }

    /// Access typed vector<float> argument by name
    template<> vector<float> AlgoArguments::value<vector<float> >(const string& nam)  const   {
      return __cnvVect<float>(this,"numeric",raw_arg(nam));
    }

    /// Access typed vector<long> argument by name
    template<> vector<long> AlgoArguments::value<vector<long> >(const string& nam)  const   {
      return __cnvVect<long>(this,"numeric",raw_arg(nam));
    }

    /// Access typed vector<int> argument by name
    template<> vector<int> AlgoArguments::value<vector<int> >(const string& nam)  const   {
      return __cnvVect<int>(this,"numeric",raw_arg(nam));
    }
  }
}
