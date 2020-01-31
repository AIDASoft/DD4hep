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
#include "ConditionExampleObjects.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/ConditionsProcessor.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::ConditionExamples;
using cond::DependencyBuilder;
using cond::ConditionsLoadInfo;

/// Install the consitions and the alignment manager
ConditionsManager dd4hep::ConditionExamples::installManager(Detector& description)  {
  // Now we instantiate the conditions manager
  description.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
  ConditionsManager manager = ConditionsManager::from(description);
  manager["PoolType"]       = "DD4hep_ConditionsLinearPool";
  manager["UserPoolType"]   = "DD4hep_ConditionsMapUserPool";
  manager["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  manager.initialize();
  return manager;
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate1::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate1","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate1","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  target.bind<vector<int> >();
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionUpdate1::resolve(Condition target, ConditionUpdateContext& context)  {
  vector<int>& data  = target.get<vector<int> >();
  Condition    cond0 = context.condition(context.key(0));
  data.emplace_back(cond0.get<int>());
  data.emplace_back(cond0.get<int>()*2);
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate2::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate2","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate2","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  target.bind<vector<int> >();
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionUpdate2::resolve(Condition target, ConditionUpdateContext& context)  {
  vector<int>& data  = target.get<vector<int> >();
  Condition    cond0 = context.condition(context.key(0));
  Condition    cond1 = context.condition(context.key(1));

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());
}


/// Interface to client Callback in order to update the condition
Condition ConditionUpdate3::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate3","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate3","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  target.bind<vector<int> >();
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionUpdate3::resolve(Condition target, ConditionUpdateContext& context)  {
  vector<int>& data  = target.get<vector<int> >();
  Condition    cond0 = context.condition(context.key(0));
  Condition    cond1 = context.condition(context.key(1));
  Condition    cond2 = context.condition(context.key(2));

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.get<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());
}

/// Initializing constructor
ConditionsDependencyCreator::ConditionsDependencyCreator(ConditionsContent& c, PrintLevel p)
  : OutputLevel(p), content(c)
{
  call1 = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate1(printLevel));
  call2 = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate2(printLevel));
  call3 = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate3(printLevel));
}

/// Destructor
ConditionsDependencyCreator::~ConditionsDependencyCreator()  {
}

/// Callback to process a single detector element
int ConditionsDependencyCreator::operator()(DetElement de, int)  const  {
  ConditionKey      key(de,"derived_data");
  ConditionKey      target1(de,"derived_data/derived_1");
  ConditionKey      target2(de,"derived_data/derived_2");
  ConditionKey      target3(de,"derived_data/derived_3");
  DependencyBuilder build_1(de, target1.item_key(), call1);
  DependencyBuilder build_2(de, target2.item_key(), call2);
  DependencyBuilder build_3(de, target3.item_key(), call3);
  //DependencyBuilder build_1(de, "derived_data/derived_1", call1);
  //DependencyBuilder build_2(de, "derived_data/derived_2", call2);
  //DependencyBuilder build_3(de, "derived_data/derived_3", call3);

  // Compute the derived stuff
  build_1.add(key);

  build_2.add(key);
  build_2.add(target1);
  
  build_3.add(key);
  build_3.add(target1);
  build_3.add(target2);

  content.addDependency(build_1.release());
  content.addDependency(build_2.release());
  content.addDependency(build_3.release());
  printout(printLevel,"Example","++ Added derived conditions dependencies for %s",de.path().c_str());
  return 1;
}

/// Callback to process a single detector element
int ConditionsDataAccess::operator()(DetElement de, int level)  const  {
  vector<Condition> conditions;
  conditionsCollector(map,conditions)(de, level);
  return accessConditions(de, conditions);
}

/// Common call to access selected conditions
int ConditionsDataAccess::accessConditions(DetElement de, const std::vector<Condition>& conditions)  const  {
  ConditionKey key_temperature (de,"temperature");
  ConditionKey key_pressure    (de,"pressure");
  ConditionKey key_double_table(de,"double_table");
  ConditionKey key_int_table   (de,"int_table");
  ConditionKey key_derived_data(de,"derived_data");
  ConditionKey key_derived1    (de,"derived_data/derived_1");
  ConditionKey key_derived2    (de,"derived_data/derived_2");
  ConditionKey key_derived3    (de,"derived_data/derived_3");
  int result = 0, count = 0;

  // Let's go for the deltas....
  for( auto cond : conditions )  {
    if ( cond.item_key() == key_temperature.item_key() )  {
      result += int(cond.get<double>());
    }
    else if ( cond.item_key() == key_pressure.item_key() )  {
      result += int(cond.get<double>());
    }
    else if ( cond.item_key() == key_double_table.item_key() )  {
      result += int(cond.get<vector<double> >().size());
    }
    else if ( cond.item_key() == key_int_table.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( cond.item_key() == key_derived_data.item_key() )  {
      result += int(cond.get<int>());
    }
    else if ( cond.item_key() == key_derived1.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( cond.item_key() == key_derived2.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( cond.item_key() == key_derived3.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    if ( !IOV::key_is_contained(iov.key(),cond.iov().key()) )  {
      printout(ERROR,"CondAccess","++ IOV mismatch:%s <> %s",
               iov.str().c_str(), cond.iov().str().c_str());
      continue;
    }
    ++count;
  }
  return count;
}

/// Callback to process a single detector element
int ConditionsKeys::operator()(DetElement de, int)  const   {
  content.insertKey(ConditionKey(de,"temperature").hash);
  content.insertKey(ConditionKey(de,"pressure").hash);
  content.insertKey(ConditionKey(de,"double_table").hash);
  content.insertKey(ConditionKey(de,"int_table").hash);
  content.insertKey(ConditionKey(de,"derived_data").hash);
  return 1;
}

template<typename T> Condition ConditionsCreator::make_condition(DetElement de, const string& name, T val)  const {
  Condition cond(de.path()+"#"+name, name);
  T& value   = cond.bind<T>();
  value      = val;
  cond->hash = ConditionKey::hashCode(de,name);
  return cond;
}

/// Callback to process a single detector element
int ConditionsCreator::operator()(DetElement de, int)  const  {
  Condition temperature = make_condition<double>(de,"temperature",1.222);
  Condition pressure    = make_condition<double>(de,"pressure",888.88);
  Condition derived     = make_condition<int>   (de,"derived_data",100);
  Condition dbl_table   = make_condition<vector<double> >(de,"double_table",{1.,2.,3.,4.,5.,6.,7.,8.,9.});
  Condition int_table   = make_condition<vector<int> >   (de,"int_table",{10,20,30,40,50,60,70,80,90});

  slice.manager.registerUnlocked(pool, temperature);
  slice.manager.registerUnlocked(pool, pressure);
  slice.manager.registerUnlocked(pool, derived);
  slice.manager.registerUnlocked(pool, dbl_table);
  slice.manager.registerUnlocked(pool, int_table);
  printout(printLevel,"Creator","++ Adding manually conditions for %s",de.path().c_str());
  return 5;
}
