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
#include "DD4hep/OpaqueDataBinder.h"
#include "DD4hep/objects/OpaqueData_inl.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/Conditions.h"

// C/C++ include files
#include <set>
#include <map>
#include <list>
#include <vector>

using namespace std;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Helper class to bind string values to C++ data objects (primitive or complex)
  template <typename T, typename Q> bool ValueBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<Q>(val);    return true;  }

  /// Helper class to bind string values to a STL vector of data objects (primitive or complex)
  template <typename T, typename Q> bool VectorBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<vector<Q> >(val);    return true;  }

  /// Helper class to bind string values to a STL list of data objects (primitive or complex)
  template <typename T, typename Q> bool ListBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<list<Q> >(val);    return true;  }

  /// Helper class to bind string values to a STL set of data objects (primitive or complex)
  template <typename T, typename Q> bool SetBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<set<Q> >(val);    return true;  }

  /// Helper class to bind STL map objects
  template <typename T, typename Q> bool MapBinder::bind(T& object, const Q*) const
  {  object.bind<Q>();      return true;  }

  /// Binding function for scalar items. See the implementation function for the concrete instantiations
  template <typename BINDER, typename T> 
  bool OpaqueDataBinder::bind(const BINDER& b, T& object, const string& typ, const string& val)  {
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    if ( typ.substr(0,4) == "char" )
      return b.bind(object,val,_char());
    else if ( typ.substr(0,13) == "unsigned char" )
      return b.bind(object,val,_uchar());
    else if ( typ.substr(0,5) == "short" )
      return b.bind(object,val,_short());
    else if ( typ.substr(0,14) == "unsigned short" )
      return b.bind(object,val,_ushort());
    else if ( typ.substr(0,12) == "unsigned int" )
      return b.bind(object,val,_uint());
    else if ( typ.substr(0,13) == "unsigned long" )
      return b.bind(object,val,_ulong());
#else
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( typ.substr(0,4) == "char" )
      return b.bind(object,val,_int());
    else if ( typ.substr(0,5) == "short" )
      return b.bind(object,val,_int());
#endif
    else if ( typ.substr(0,3) == "int" )
      return b.bind(object,val,_int());
    else if ( typ.substr(0,4) == "long" ) 
      return b.bind(object,val,_long());
    else if ( typ.substr(0,5) == "float" )
      return b.bind(object,val,_float());
    else if ( typ.substr(0,6) == "double" )
      return b.bind(object,val,_double());
    else if ( typ.substr(0,6) == "string" )
      return b.bind(object,val,_string());
    else if ( typ == "std::string" )
      return b.bind(object,val,_string());
    else if ( typ == "Histo1D" )
      return b.bind(object,val,_string());
    else if ( typ == "Histo2D" )
      return b.bind(object,val,_string());
    else
      printout(INFO,"OpaqueDataBinder","++ Unknown conditions parameter type:%s val:%s",typ.c_str(),val.c_str());
    return b.bind(object,val,_string());
  }
  
  /// Binding function for sequences (unmapped STL containers)
  template <typename T> 
  bool OpaqueDataBinder::bind_sequence(T& object, const string& typ, const string& val)
  {
    size_t idx = typ.find('[');
    size_t idq = typ.find(']');
    string value_type = typ.substr(idx+1,idq-idx-1);
    if ( typ.substr(0,6) == "vector" )
      return bind(VectorBinder(), object, value_type, val);
    else if ( typ.substr(0,6) == "list" )
      return bind(ListBinder(), object, value_type, val);
    else if ( typ.substr(0,6) == "set" )
      return bind(SetBinder(), object, value_type, val);
    else if ( idx == string::npos && idq == string::npos )
      return bind(ValueBinder(), object, value_type, val);
    return false;
  }


  template<typename KEY, typename VAL>
  static void insert_map_item(const KEY& k, const string& val, OpaqueDataBlock& block)  {
    typedef map<KEY,VAL> map_t;
    map_t& m = block.get<map_t>();
    VAL v;
    if ( !BasicGrammar::instance<VAL>().fromString(&v, val) )  {
      except("OpaqueDataBinder","++ Failed to convert conditions map entry.");
    }
    m.insert(make_pair(k,v));
  }
  template<typename KEY, typename VAL>
  static void insert_map_item(const KEY& k, const string& val, Conditions::Condition& block)  {
    typedef map<KEY,VAL> map_t;
    map_t& m = block.get<map_t>();
    VAL v;
    if ( !BasicGrammar::instance<VAL>().fromString(&v, val) )  {
      except("OpaqueDataBinder","++ Failed to convert conditions map entry.");
    }
    m.insert(make_pair(k,v));
  }

  template<typename KEY, typename OBJECT> 
  static void insert_map_key(const string& key_val,
                             const string& val_type,
                             const string& val,
                             OBJECT& block)
  {
    KEY key;
    BasicGrammar::instance<KEY>().fromString(&key, key_val);
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( val_type.substr(0,4) == "char" )
      insert_map_item<KEY,int>(key,val,block);
    else if ( val_type.substr(0,5) == "short" )
      insert_map_item<KEY,int>(key,val,block);
    else if ( val_type.substr(0,3) == "int" )
      insert_map_item<KEY,int>(key,val,block);
    else if ( val_type.substr(0,4) == "long" )
      insert_map_item<KEY,long>(key,val,block);
    else if ( val_type.substr(0,5) == "float" )
      insert_map_item<KEY,float>(key,val,block);
    else if ( val_type.substr(0,6) == "double" )
      insert_map_item<KEY,double>(key,val,block);
    else if ( val_type.substr(0,6) == "string" )
      insert_map_item<KEY,string>(key,val,block);
    else if ( val_type == "std::string" )
      insert_map_item<KEY,string>(key,val,block);
    else {
      printout(INFO,"Param","++ Unknown conditions parameter type:%s data:%s",
               val_type.c_str(),val.c_str());
      insert_map_item<KEY,string>(key,val,block);
    }
  }

  template<typename OBJECT, typename KEY> 
    static void bind_mapping(const string& val_type, OBJECT& object, const KEY*)   {
    if ( val_type.substr(0,3) == "int" )
      MapBinder().bind(object, (map<KEY,int>*)0);
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    else if ( val_type.substr(0,12) == "unsigned int" )
      MapBinder().bind(object, (map<KEY,unsigned int>*)0);
    else if ( val_type.substr(0,4) == "char" )
      MapBinder().bind(object, (map<KEY,char>*)0);
    else if ( val_type.substr(0,13) == "unsigned char" )
      MapBinder().bind(object, (map<KEY,unsigned char>*)0);
    else if ( val_type.substr(0,5) == "short" )
      MapBinder().bind(object, (map<KEY,short>*)0);
    else if ( val_type.substr(0,14) == "unsigned short" )
      MapBinder().bind(object, (map<KEY,unsigned short>*)0);
    else if ( val_type.substr(0,13) == "unsigned long" )
      MapBinder().bind(object, (map<KEY,unsigned long>*)0);
#else
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    else if ( val_type.substr(0,4) == "char" )
      MapBinder().bind(object, (map<KEY,int>*)0);
    else if ( val_type.substr(0,5) == "short" )
      MapBinder().bind(object, (map<KEY,int>*)0);
#endif
    else if ( val_type.substr(0,4) == "long" )
      MapBinder().bind(object, (map<KEY,long>*)0);
    else if ( val_type.substr(0,5) == "float" )
      MapBinder().bind(object, (map<KEY,float>*)0);
    else if ( val_type.substr(0,6) == "double" )
      MapBinder().bind(object, (map<KEY,double>*)0);
    else if ( val_type.substr(0,6) == "string" )
      MapBinder().bind(object, (map<KEY,string>*)0);
    else if ( val_type == "std::string" )
      MapBinder().bind(object, (map<KEY,string>*)0);
    else {
      MapBinder().bind(object, (map<KEY,string>*)0);
    }
  }
  
  /// Binding function for STL maps
  template <typename T> 
  bool OpaqueDataBinder::bind_map(T& object, const string& key_type, const string& val_type)   {
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( key_type.substr(0,3) == "int" )
      bind_mapping(val_type,object,_int());
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    else if ( key_type.substr(0,4) == "char" )
      bind_mapping(val_type,object,_int());
    else if ( key_type.substr(0,5) == "short" )
      bind_mapping(val_type,object,_int());
    else if ( key_type.substr(0,4) == "long" )
      bind_mapping(val_type,object,_long());
    else if ( key_type.substr(0,5) == "float" )
      bind_mapping(val_type,object,_float());
    else if ( key_type.substr(0,6) == "double" )
      bind_mapping(val_type,object,_double());
#endif
    else if ( key_type.substr(0,6) == "string" )
      bind_mapping(val_type,object,_string());
    else if ( key_type == "std::string" )
      bind_mapping(val_type,object,_string());
    else {
      printout(INFO,"OpaqueDataBinder","++ Unknown MAP-conditions key-type:%s",key_type.c_str());
      bind_mapping(val_type,object,_string());
    }
    return true;
  }

  /// Filling function for STL maps.
  template <typename T>
  bool OpaqueDataBinder::insert_map(T& object, const string& key_type, const string& key, const string& val_type, const string& val)  {
    if ( key_type.substr(0,3) == "int" )
      insert_map_key<int>(key,val_type,val,object);
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    else if ( key_type.substr(0,4) == "char" )
      insert_map_key<int>(key,val_type,val,object);
    else if ( key_type.substr(0,5) == "short" )
      insert_map_key<int>(key,val_type,val,object);
    else if ( key_type.substr(0,4) == "long" )
      insert_map_key<long>(key,val_type,val,object);
    else if ( key_type.substr(0,5) == "float" )
      insert_map_key<float>(key,val_type,val,object);
    else if ( key_type.substr(0,6) == "double" )
      insert_map_key<double>(key,val_type,val,object);
#endif
    else if ( key_type.substr(0,6) == "string" )
      insert_map_key<string>(key,val_type,val,object);
    else if ( key_type == "std::string" )
      insert_map_key<string>(key,val_type,val,object);
    else {
      printout(INFO,"OpaqueDataBinder","++ Unknown MAP-conditions key-type:%s",key_type.c_str());
      insert_map_key<string>(key,val_type,val,object);
    }
    return true;
  }

  /// Conditions binding function for STL maps
  template <> 
  bool OpaqueDataBinder::bind_map(Conditions::Condition& object, const string& key_type, const string& val_type)
  {    return bind_map(object->data, key_type, val_type);  }
  /// Conditions: Filling function for STL maps.
  template <> bool OpaqueDataBinder::insert_map(Conditions::Condition& object,
                                                const string& key_type, const string& key,
                                                const string& val_type, const string& val)
  {    return insert_map(object->data, key_type, key, val_type, val);    }
  /// Instantiation for Conditions:
  template bool
  OpaqueDataBinder::bind_sequence<Conditions::Condition>(Conditions::Condition& object,const string& typ,const string& val);
  
  /// Instantiate the data binder for OpaqueData
  template bool OpaqueDataBinder::bind_sequence<OpaqueDataBlock>(OpaqueDataBlock& object,const string& typ,const string& val);
  template bool OpaqueDataBinder::bind_map<OpaqueDataBlock>(OpaqueDataBlock& object,const string& typ,const string& val);
  template bool OpaqueDataBinder::insert_map<OpaqueDataBlock>(OpaqueDataBlock& object,
                                                              const string& key_type, const string& key,
                                                              const string& val_type, const string& val);

}
