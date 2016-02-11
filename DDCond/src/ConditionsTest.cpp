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

// Framework include files
#include "DD4hep/DetectorTools.h"
#include "ConditionsTest.h"
#include <vector>

using namespace std;
using namespace DD4hep::Conditions;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Namespace for test environments in DDCond
    namespace Test  {

      template <typename T> void print_bound_condition(Condition c, const char* norm)   {}

      template<typename T> void __print_bound_val(Condition c, const char* norm, const char* fmt)  {
        const char* test = c.detector().name();
        char text_format[1024];
        c.bind<T>();
        T value = c.get<T>();
        if ( norm )  {
          T val = _multiply(c.get<T>(),norm);
          ::snprintf(text_format,sizeof(text_format),"  Bound value  %%s : value:%s [%s] Type: %%s",fmt,fmt);
          printout(INFO,test,text_format, c.name().c_str(), value, val, typeName(c.typeInfo()).c_str());
          return;
        }
        ::snprintf(text_format,sizeof(text_format),"  Bound value  %%s : value:%s Type: %%s",fmt);
        printout(INFO,test,text_format, c.name().c_str(), value, typeName(c.typeInfo()).c_str());
      }

      template void __print_bound_val<short>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<int>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<long>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<unsigned short>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<unsigned int>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<unsigned long>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<float>(Condition c, const char* norm, const char* fmt);
      template void __print_bound_val<double>(Condition c, const char* norm, const char* fmt);

      template <> void print_bound_value<string>(Condition c, const char*)   {
        const char* test = c.detector().name();
        c.bind<string>();
        printout(INFO,test,"  Bound value  %s : string value:%s  Type: %s",
                 c.name().c_str(), c.get<string>().c_str(),typeName(c.typeInfo()).c_str());
      }

      template <> void print_bound_value<short>(Condition c, const char* norm)
      {       __print_bound_val<short>(c, norm, "%d");      }

      template <> void print_bound_value<int>(Condition c, const char* norm)
      {       __print_bound_val<int>(c, norm, "%d");        }

      template <> void print_bound_value<long>(Condition c, const char* norm)
      {       __print_bound_val<long>(c, norm, "%d");       }

      template <> void print_bound_value<unsigned short>(Condition c, const char* norm)
      {       __print_bound_val<unsigned short>(c, norm, "%d");      }

      template <> void print_bound_value<unsigned int>(Condition c, const char* norm)
      {       __print_bound_val<unsigned int>(c, norm, "%d");        }

      template <> void print_bound_value<unsigned long>(Condition c, const char* norm)
      {       __print_bound_val<unsigned long>(c, norm, "%d");       }

      template <> void print_bound_value<float>(Condition c, const char* norm)
      {       __print_bound_val<float>(c, norm, "%g");      }

      template <> void print_bound_value<double>(Condition c, const char* norm)
      {       __print_bound_val<double>(c, norm, "%g");     }


      template <> void print_bound_value<vector<double> >(Condition c, const char*)   {
        const char* test = c.detector().name();
        c.bind<vector<double> >();
        const vector<double>& v = c.get<vector<double> >();
        printout(INFO,test,"  Bound value  %s : size:%d = %s Type: %s",
                 c.name().c_str(), int(v.size()), c.block().str().c_str(),
                 typeName(c.typeInfo()).c_str());
      }

      template <> void print_condition<void>(Condition c)   {
        const char* test = c.detector().name();
        string type = c.type();
        printout(INFO,test,"%-32s  [%16s] :  %s [%s] ", 
                 (c.detector().path()+"."+c.name()).c_str(),c.type().c_str(),
                 c.value().c_str(),c.validity().c_str());
        if ( type == "alignment" )
          print_bound_value<string>(c);
        else if ( type == "temperature" )
          print_bound_value<double>(c);
        else if ( type == "pressure" )
          print_bound_value<double>(c,"1.0/hPa");
        else if ( type == "whatever" )
          print_bound_value<vector<double> >(c);
        else if ( type == "short" )
          print_bound_value<short>(c);
        else if ( type == "integer" )
          print_bound_value<int>(c);
        else if ( type == "long" )
          print_bound_value<long>(c);
        else if ( type == "unsigned short" )
          print_bound_value<unsigned short>(c);
        else if ( type == "unsigned integer" )
          print_bound_value<unsigned int>(c);
        else if ( type == "unsigned long" )
          print_bound_value<unsigned long>(c);
        else if ( type == "double" )
          print_bound_value<double>(c);
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
          except("Example", "+++ The condition %s.%s [%s] is not fully contained in iov:%s",
                 c->detector.path().c_str(), c->name.c_str(), i->str().c_str(), iov.str().c_str());
        }
        except("Example", "+++ The condition %s.%s [%s] has no discrete type matching iov:%s",
               c->detector.path().c_str(), c->name.c_str(), i->str().c_str(), iov.str().c_str());
      }
    }
  }
}

Test::TestEnv::TestEnv(LCDD& _lcdd, const string& detector_name) 
  : lcdd(_lcdd), detector(), manager()
{
  manager = ConditionsManager::from(lcdd);
  manager["LoaderType"]     = "multi";
  manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
  manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  manager["ReplacementPoolType"] = "DD4hep_ConditionsLinearReplacementPool";
  manager->initialize();
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
DetElement Test::TestEnv::daughter(const string& sub_path)  const  {
  return DD4hep::Geometry::DetectorTools::findDaughterElement(detector,sub_path);
}

void Test::TestEnv::add_xml_data_source(const string& file)   {
  string source = "xml:"+string(::getenv("DD4hepINSTALL"))+file;
  this->loader->addSource(source);
}


void Test::TestEnv::dump_conditions_pools()
{
  typedef RangeConditions _R;
  typedef IOVPool::Entries _E;
  typedef Interna::ConditionsManagerObject::TypedConditionPool _P;
  int cnt = 0;
  _P& p = this->manager->m_pool;
  for(_P::const_iterator i=p.begin(); i != p.end(); ++i, ++cnt)  {
    IOVPool* pool = (*i);
    if ( pool )  {
      const _E& e = pool->entries;
      const IOVType* typ = this->manager->iovType(cnt);
      printout(INFO,"Example","+++ IOVPool for type %s", typ->str().c_str());
      for (_E::const_iterator j=e.begin(); j != e.end(); ++j)  {
        _R rc;
        ConditionsPool* cp = (*j).second;
        cp->select_all(rc);
        printout(INFO,"Example","+++ Conditions for pool with IOV: %s age:%d",
                 cp->iov->str().c_str(), cp->age_value);
        print_conditions<void>(rc);
      }
    }
  }
  printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  printout(INFO,"Example","SUCCESS: +++ Conditions pools successfully dumped");
  printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
}

/// Dump the conditions of one detectpr element
void Test::TestEnv::dump_detector_element(DetElement elt)
{
  Container conds = elt.conditions();
  printout(INFO,"conditions","DetElement:%s # of conditons:%d",elt.path().c_str(),int(conds.count()));
  const Container::Entries& entries = conds.entries();
  for(Container::Entries::const_iterator i=entries.begin(); i!=entries.end(); ++i)  {
    Condition cond((*i).second);
    print_condition<void>(cond);
  }
}

/// Dump conditions tree of a detector element
void Test::TestEnv::dump_conditions_tree(DetElement elt)
{
  Container conds = elt.conditions();
  const DetElement::Children& children = elt.children();
  if ( !conds.isValid() )
    printout(INFO,"conditions_tree","DetElement:%s  NO CONDITIONS present",elt.path().c_str());
  else
    dump_detector_element(elt);
  for(DetElement::Children::const_iterator j=children.begin(); j!=children.end(); ++j)
    dump_conditions_tree((*j).second);
}
