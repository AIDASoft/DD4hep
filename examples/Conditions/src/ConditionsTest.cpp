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
#include "ConditionsTest.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetectorTools.h"
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsManager.h"
#include "DD4hep/objects/DetectorInterna.h"
#include "DD4hep/objects/ConditionsInterna.h"

// C/C++ include files
#include <vector>
#include <list>
#include <set>
#include <map>
#include <deque>

using namespace std;
using namespace DD4hep::ConditionExamples;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace ConditionExamples {

    template <typename T> void print_bound_condition(Condition /* c */, const char* /* norm */)   {}
      
    template<typename T> void __print_bound_val(Condition c, const char* norm)  {
      char text_format[1024];
      const T& value = access_val<T>(c);
      if ( norm )  {
        T val = _multiply(c.get<T>(),norm);
        ::snprintf(text_format,sizeof(text_format),"  Bound value  %%s : value:%s [%s] Type: %%s",
                   Primitive<T>::default_format(),Primitive<T>::default_format());
        printout(INFO,"Cond_Value",text_format, c.name(), value, val, typeName(c.typeInfo()).c_str());
        return;
      }
      ::snprintf(text_format,sizeof(text_format),"  Bound value  %%s : value:%s Type: %%s",
                 Primitive<T>::default_format());
      printout(INFO,"Cond_Value",text_format, c.name(), value, typeName(c.typeInfo()).c_str());
    }
    template <> void __print_bound_val<string>(Condition c, const char*)   {
      const string& v = access_val<string>(c);
      printout(INFO,"Cond_Value","  Bound value  %s : string value:%s  Type: %s Ptr:%016X",
               c.name(), c.get<string>().c_str(),typeName(c.typeInfo()).c_str(),
               (void*)&v);
    }
    template <typename T> void __print_bound_container(Condition c, const char*)   {
      const T& v = access_val<T>(c);
      printout(INFO,"Cond_Value","  Bound value  %s : size:%d = %s Type: %s Ptr:%016X",
               c.name(), int(v.size()), c.data().str().c_str(),
               typeName(c.typeInfo()).c_str(), (void*)&v);
    }


#define TEMPLATE_SIMPLE_TYPE(x)                                         \
    template <> void print_bound_value<x>(Condition c, const char* norm) \
    {       __print_bound_val<x>(c, norm); }

#define TEMPLATE_CONTAINER_TYPE(container,x)                            \
    template void __print_bound_container<container >(Condition c, const char* norm); \
    template <> void print_bound_value<container >(Condition c, const char* norm) \
    { __print_bound_container<container >(c, norm); } 
       
#if defined(DD4HEP_HAVE_ALL_PARSERS)
#define TEMPLATE_TYPE(x,f)                                \
    TEMPLATE_SIMPLE_TYPE(x)                               \
    TEMPLATE_CONTAINER_TYPE(vector<x>,x)                  \
    TEMPLATE_CONTAINER_TYPE(list<x>,x)                    \
    TEMPLATE_CONTAINER_TYPE(set<x>,x)                     \
    TEMPLATE_CONTAINER_TYPE(Primitive<x>::int_map_t,x)    \
    TEMPLATE_CONTAINER_TYPE(Primitive<x>::size_map_t,x)   \
    TEMPLATE_CONTAINER_TYPE(Primitive<x>::string_map_t,x)

    TEMPLATE_TYPE(char,"%c")
    TEMPLATE_TYPE(unsigned char,"%02X")
    TEMPLATE_TYPE(short,"%d")
    TEMPLATE_TYPE(unsigned short,"%04X")
    TEMPLATE_TYPE(unsigned int,"%08X")
    TEMPLATE_TYPE(unsigned long,"%016X")
#else
#define TEMPLATE_TYPE(x,f)                                \
    TEMPLATE_SIMPLE_TYPE(x)                               \
    TEMPLATE_CONTAINER_TYPE(vector<x>,x)                  \
    TEMPLATE_CONTAINER_TYPE(list<x>,x)                    \
    TEMPLATE_CONTAINER_TYPE(set<x>,x)                     \
    TEMPLATE_CONTAINER_TYPE(Primitive<x>::int_map_t,x)    \
    TEMPLATE_CONTAINER_TYPE(Primitive<x>::string_map_t,x)
#endif // DD4HEP_HAVE_ALL_PARSERS

    TEMPLATE_TYPE(int,"%d")
    TEMPLATE_TYPE(long,"%ld")
    TEMPLATE_TYPE(float,"%f")
    TEMPLATE_TYPE(double,"%g")
    TEMPLATE_TYPE(std::string,"%c")

    template <> void print_condition<void>(Condition c)   {
      string type = c.type();
      printout(INFO,"Cond_Value","%-32s  [%16s] :  %s [%s] ", 
               c.name(), c.type().c_str(),
               c.value().c_str(), c->validity.c_str());
      if ( type == "alignment" )
        print_bound_value<string>(c);
      else if ( type == "temperature" )
        print_bound_value<double>(c);
      else if ( type == "pressure" )
        print_bound_value<double>(c,"1.0/hPa");
      else if ( type == "whatever" )
        print_bound_value<vector<double> >(c);
      else if ( type == "integer" )
        print_bound_value<int>(c);
      else if ( type == "long" )
        print_bound_value<long>(c);
#if defined(DD4HEP_HAVE_ALL_PARSERS)
      else if ( type == "short" )
        print_bound_value<short>(c);
      else if ( type == "unsigned short" )
        print_bound_value<unsigned short>(c);
      else if ( type == "unsigned integer" )
        print_bound_value<unsigned int>(c);
      else if ( type == "unsigned long" )
        print_bound_value<unsigned long>(c);
#endif
      else if ( type == "float" )
        print_bound_value<float>(c);
      else if ( type == "double" )
        print_bound_value<double>(c);
      else if ( type == "vector<double>" )
        print_bound_value<vector<double> >(c);
    }

    template <typename T> void print_conditions(const RangeConditions& rc)   {
      for(RangeConditions::const_iterator k=rc.begin(); k != rc.end(); ++k) 
        print_condition<T>(*k);
    }

    template void print_conditions<void>(const RangeConditions& rc);


    void check_discrete_condition(Condition c, const IOV& iov)   {
      const IOV* i = c->iov;
      if ( i->is_discrete() )  {
        if ( iov.contains(*i) )  {
          return;
        }
        except("Example", "+++ The condition %s [%s] is not fully contained in iov:%s",
               c->name.c_str(), i->str().c_str(), iov.str().c_str());
      }
      except("Example", "+++ The condition %s [%s] has no discrete type matching iov:%s",
             c->name.c_str(), i->str().c_str(), iov.str().c_str());
    }
  }
}

TestEnv::TestEnv(LCDD& _lcdd, const string& detector_name) 
  : lcdd(_lcdd), detector(), manager()
{
  manager = ConditionsManager::from(lcdd);
  manager["LoaderType"]     = "multi";
  manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
  manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  manager["UserPoolType"]   = "DD4hep_ConditionsLinearUserPool";
  manager.initialize();
  detector = lcdd.detector(detector_name);
  if ( detector.isValid() )  {
    pair<bool, const IOVType*> e = manager.registerIOVType(0, "epoch");
    pair<bool, const IOVType*> r = manager.registerIOVType(1, "run");
    loader = manager.loader();
    if ( e.first && r.first )  {
      epoch = e.second;
      run   = r.second;
      return;
    }
    invalidHandleError<IOVType>();
  }
  invalidHandleError<DetElement>();
}

/// Find daughter element of the detector object
DetElement TestEnv::daughter(const string& sub_path)  const  {
  return DD4hep::Geometry::DetectorTools::findDaughterElement(detector,sub_path);
}

void TestEnv::add_xml_data_source(const string& file, const string& iov_str)   {
  string source = "xml:"+string(::getenv("DD4hepINSTALL"))+file;
  IOV iov(0);
  this->manager.fromString(iov_str, iov);
  this->loader->addSource(source, iov);
}

/// Dump the conditions of one detectpr element
void TestEnv::dump_detector_element(DetElement elt, ConditionsMap& map)
{
  DetElementConditionsCollector scanner(elt);
  map.scan(scanner);
  if ( scanner.conditions.empty() )  {
    printout(INFO,"conditions_tree","DetElement:%s  NO CONDITIONS present",elt.path().c_str());
  }
  else  {
    printout(INFO,"conditions","DetElement:%s # of conditons:%ld",elt.path().c_str(),scanner.conditions.size());
    for(const auto& c : scanner.conditions )
      print_condition<void>(c);
  }
}

/// Dump conditions tree of a detector element
void TestEnv::dump_conditions_tree(DetElement elt, ConditionsMap& map)
{
  const DetElement::Children& children = elt.children();
  dump_detector_element(elt, map);
  for(DetElement::Children::const_iterator j=children.begin(); j!=children.end(); ++j)
    dump_conditions_tree((*j).second, map);
}
