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

// Framework include files
#include "ConditionAnyExampleObjects.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/ConditionAny.h"
#include "DD4hep/ConditionsProcessor.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;
using cond::DependencyBuilder;
using cond::ConditionsLoadInfo;

namespace {
  static int num_any_ingredients = 0;
  template <typename T> inline void __prt(ostream& os, const vector<T>& obj)   {
    for(const auto& o : obj)  {
      os << o << " ";
      ++num_any_ingredients;
    }
  }
}

/// Interface to client Callback in order to update the condition
Condition ConditionAnyUpdate1::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate1", "++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  ConditionAny    target(key.name, "derived", vector<int>());
#else
  printout(printLevel,"ConditionUpdate1", "++ Building dependent condition: %016llX",key.hash);
  ConditionAny    target(key.hash, vector<int>());
#endif
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionAnyUpdate1::resolve(Condition target, ConditionUpdateContext& context)  {
  ConditionAny tar(target);
  vector<int>& data  = tar.as<vector<int> >();
  ConditionAny cond0 = context.condition(context.key(0));
  data.emplace_back(cond0.value<int>());
  data.emplace_back(cond0.value<int>()*2);
}

/// Interface to client Callback in order to update the condition
Condition ConditionAnyUpdate2::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate2", "++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  ConditionAny target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate2", "++ Building dependent condition: %016llX",key.hash);
  ConditionAny target(key.hash);
#endif
  target.get() = vector<int>();
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionAnyUpdate2::resolve(Condition target, ConditionUpdateContext& context)  {
  ConditionAny tar(target);
  vector<int>& data  = tar.as<vector<int> >();
  ConditionAny cond0 = context.condition(context.key(0));
  ConditionAny cond1 = context.condition(context.key(1));

  data.push_back(cond0.value<int>());
  data.push_back(cond0.value<int>()*2);
  vector<int>& c1 = cond1.as<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());
}


/// Interface to client Callback in order to update the condition
Condition ConditionAnyUpdate3::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate3", "++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  return ConditionAny(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate3", "++ Building dependent condition: %016llX",key.hash);
  return ConditionAny(key.hash);
#endif
}

/// Interface to client Callback in order to update the condition
void ConditionAnyUpdate3::resolve(Condition target, ConditionUpdateContext& context)  {
  vector<int>  data;
  ConditionAny cond0 = context.condition(context.key(0));
  ConditionAny cond1 = context.condition(context.key(1));
  ConditionAny cond2 = context.condition(context.key(2));

  data.push_back(cond0.as<int>());
  data.push_back(cond0.as<int>()*2);
  vector<int>& c1 = cond1.as<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.as<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());
  target.get<std::any>() = std::move(data);
}

/// Interface to client Callback in order to update the condition
Condition ConditionAnyUpdate4::operator()(const ConditionKey& key, ConditionUpdateContext& context)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate4", "++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  ConditionAny target(key.name,"derived", vector<int>());
#else
  printout(printLevel,"ConditionUpdate4", "++ Building dependent condition: %016llX",key.hash);
  ConditionAny target(key.hash, vector<int>());
#endif
  vector<int>& data  = target.as<std::vector<int> >();
  ConditionAny cond3 = context.condition(context.key(0));
  ConditionAny cond2 = context.condition(context.key(1));
  ConditionAny cond0 = context.condition(context.key(2));
  ConditionAny cond1 = context.condition(context.key(3));

  data.push_back(cond0.as<int>());
  data.push_back(cond0.as<int>()*2);
  vector<int>& c1 = cond1.as<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.as<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());

  vector<int>& c3 = cond3.as<vector<int> >();
  data.insert(data.end(), c3.begin(), c3.end());
  return target;
}

/// Initializing constructor
ConditionsAnyDependencyCreator::ConditionsAnyDependencyCreator(ConditionsContent& c, PrintLevel p, bool persist, int ex)
  : OutputLevel(p), content(c), persist_conditions(persist), extended(ex)
{
  call1  = std::shared_ptr<ConditionUpdateCall>(new ConditionAnyUpdate1(printLevel));
  call2  = std::shared_ptr<ConditionUpdateCall>(new ConditionAnyUpdate2(printLevel));
  call3  = std::shared_ptr<ConditionUpdateCall>(new ConditionAnyUpdate3(printLevel));
  call4  = std::shared_ptr<ConditionUpdateCall>(new ConditionAnyUpdate4(printLevel));
}

/// Callback to process a single detector element
int ConditionsAnyDependencyCreator::operator()(DetElement de, int)  const  {
  ConditionKey      key     (de,"derived_data");
  ConditionKey      target1(de,"derived_data/derived_1");
  ConditionKey      target2(de,"derived_data/derived_2");
  ConditionKey      target3(de,"derived_data/derived_3");
  ConditionKey      target4(de,"derived_data/derived_4");
  DependencyBuilder build_1(de, target1.item_key(), call1);
  DependencyBuilder build_2(de, target2.item_key(), call2);
  DependencyBuilder build_3(de, target3.item_key(), call3);

  // Compute the derived stuff
  build_1.add(key);

  build_2.add(key);
  build_2.add(target1);
  
  
  build_3.add(key);
  build_3.add(target1);
  build_3.add(target2);

  if ( extended >= 1 )   {
    DependencyBuilder build_4(de, target4.item_key(), call4);
    build_4.add(target3);
    build_4.add(target2);
    build_4.add(key);
    build_4.add(target1);
    content.addDependency(build_4.release());
  }
  content.addDependency(build_1.release());
  content.addDependency(build_2.release());
  content.addDependency(build_3.release());
  printout(printLevel,"Example", "++ Added derived conditions dependencies for %s",de.path().c_str());
  return 1;
}

/// Standard destructor
ConditionsAnyDataAccess::~ConditionsAnyDataAccess()   {
  printout(ALWAYS,"Example", "+=========================================================================");
  printout(ALWAYS,"Example", "+ Analyzed %d any object/elements", num_any_ingredients);
  printout(ALWAYS,"Example", "+=========================================================================");
}

/// Callback to process a single detector element
int ConditionsAnyDataAccess::operator()(DetElement de, int level)  const  {
  vector<Condition> conditions;
  conditionsCollector(map,conditions)(de, level);
  return accessConditions(de, conditions);
}

/// Common call to access selected conditions
int ConditionsAnyDataAccess::accessConditions(DetElement de, const std::vector<Condition>& conditions)  const  {
  ConditionKey key_temperature (de,"temperature");
  ConditionKey key_pressure    (de,"pressure");
  ConditionKey key_double_table(de,"double_table");
  ConditionKey key_int_table   (de,"int_table");
  ConditionKey key_derived_data(de,"derived_data");
  ConditionKey key_derived1    (de,"derived_data/derived_1");
  ConditionKey key_derived2    (de,"derived_data/derived_2");
  ConditionKey key_derived3    (de,"derived_data/derived_3");
  ConditionKey key_derived4    (de,"derived_data/derived_4");
  ConditionKey key_path        (de,"de_path");
  int result = 0, count = 0;

  // Let's go for the deltas....
  for( auto condition : conditions )  {
    stringstream str;
    ConditionAny cond = condition;
    // const auto& info = cond.descriptor().type();
    if ( 0 == dynamic_cast<detail::ConditionObject*>(cond.ptr()) )  {
      printout(ERROR,"accessConditions", "Condition with bad base class!");
    }
   
    if ( cond.item_key() == key_path.item_key() )  {
      result += int(cond.as<string>().length());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_temperature.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(), 
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", cond.value<string>().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_temperature.item_key() )  {
      result += int(cond.as<double>());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_temperature.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(), 
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %f", cond.as<double>());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_pressure.item_key() )  {
      result += int(cond.as<double>());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_pressure.toString().c_str(), 
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %f", cond.as<double>());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_double_table.item_key() )  {
      result += int(cond.as<vector<double> >().size());
      __prt(str,cond.as<vector<double> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_double_table.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_int_table.item_key() )  {
      result += int(cond.as<vector<int> >().size());
      __prt(str,cond.as<vector<int> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_int_table.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_derived_data.item_key() )  {
      result += int(cond.as<int>());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_derived_data.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %d", cond.as<int>());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_derived1.item_key() )  {
      result += int(cond.as<vector<int> >().size());
      __prt(str,cond.as<vector<int> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_derived1.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_derived2.item_key() )  {
      result += int(cond.as<vector<int> >().size());
      __prt(str,cond.as<vector<int> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_derived2.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_derived3.item_key() )  {
      result += int(cond.as<vector<int> >().size());
      __prt(str,cond.as<vector<int> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_derived3.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    else if ( cond.item_key() == key_derived4.item_key() )  {
      result += int(cond.as<vector<int> >().size());
      __prt(str,cond.as<vector<int> >());
      printout(printLevel, "accessConditions", "Condition: %s type: %s [%s]",
	       key_derived4.toString().c_str(),
	       typeName(typeid(cond.get())).c_str(),
	       typeName(cond.any_type()).c_str());
      printout(printLevel, "accessConditions", "           value: %s", str.str().c_str());
      ++num_any_ingredients;
    }
    if ( !IOV::key_is_contained(iov.key(),cond.iov().key()) )  {
      printout(ERROR,"CondAccess", "++ IOV mismatch:%s <> %s",
               iov.str().c_str(), cond.iov().str().c_str());
      continue;
    }
    ++count;
  }
  return count;
}

/// Callback to process a single detector element
int ConditionsAnyKeys::operator()(DetElement de, int)  const   {
  content.insertKey(ConditionKey(de,"temperature").hash);
  content.insertKey(ConditionKey(de,"pressure").hash);
  content.insertKey(ConditionKey(de,"double_table").hash);
  content.insertKey(ConditionKey(de,"int_table").hash);
  content.insertKey(ConditionKey(de,"derived_data").hash);
  content.insertKey(ConditionKey(de,"de_path").hash);
  return 1;
}

template<typename T>
Condition ConditionsAnyCreator::make_condition(DetElement de, const string& name, const T& val)  const {
  ConditionAny cond(de.path()+"#"+name, name);
  cond.get() = std::make_any<T>(val);
  cond->hash = ConditionKey::hashCode(de,name);
  return cond;
}

template<typename T, typename... Args>
Condition ConditionsAnyCreator::make_condition_args(DetElement de, const string& name, Args... args)  const {
  ConditionAny cond(de.path()+"#"+name, name);
  cond.get() = std::make_any<T>(std::forward<Args>(args)...);
  cond->hash = ConditionKey::hashCode(de,name);
  return cond;
}

/// Callback to process a single detector element
int ConditionsAnyCreator::operator()(DetElement de, int)  const  {
  Condition temperature = make_condition<double>(de,"temperature",1.222);
  Condition pressure    = make_condition<double>(de,"pressure",888.88);
  Condition derived     = make_condition<int>   (de,"derived_data",102030);
  Condition dbl_table   = make_condition<vector<double> >(de,"double_table",{1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.});
  Condition int_table   = make_condition<vector<int> >   (de,"int_table",{-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120});
  Condition path        = make_condition<std::string>    (de,"de_path",de.path());

  slice.manager.registerUnlocked(pool, temperature);
  slice.manager.registerUnlocked(pool, pressure);
  slice.manager.registerUnlocked(pool, derived);
  slice.manager.registerUnlocked(pool, dbl_table);
  slice.manager.registerUnlocked(pool, int_table);
  slice.manager.registerUnlocked(pool, path);
  printout(printLevel, "Creator", "++ Adding manually conditions for %s",de.path().c_str());
  return 5;
}
