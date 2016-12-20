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
#include "ConditionExampleObjects.h"
#include "DD4hep/DD4hepUnits.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::ConditionExamples;
using Conditions::DependencyBuilder;
using Conditions::ConditionsLoadInfo;
using Conditions::ConditionsSlice;


/// Install the consitions and the alignment manager
void DD4hep::ConditionExamples::installManagers(LCDD& lcdd)  {
  // Now we instantiate the conditions manager
  lcdd.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate1::operator()(const ConditionKey& key, const Context& context)  {
  printout(printLevel,"ConditionUpdate1","++ Building dependent condition: %s",key.name.c_str());
  Condition    target(key.name,"derived");
  vector<int>& data  = target.bind<vector<int> >();
  Condition    cond0 = context.condition(0);
  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  return target;
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate2::operator()(const ConditionKey& key, const Context& context)  {
  printout(printLevel,"ConditionUpdate2","++ Building dependent condition: %s",key.name.c_str());
  Condition    target(key.name,"derived");
  vector<int>& data  = target.bind<vector<int> >();
  Condition    cond0 = context.condition(0);
  Condition    cond1 = context.condition(1);

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());
  return target;
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate3::operator()(const ConditionKey& key, const Context& context)  {
  printout(printLevel,"ConditionUpdate3","++ Building dependent condition: %s",key.name.c_str());
  Condition    target(key.name,"derived");
  vector<int>& data  = target.bind<vector<int> >();
  Condition    cond0 = context.condition(0);
  Condition    cond1 = context.condition(1);
  Condition    cond2 = context.condition(2);

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.get<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());
  return target;
}

/// Initializing constructor
ConditionsDependencyCreator::ConditionsDependencyCreator(ConditionsSlice& s,PrintLevel p)
  : slice(s), printLevel(p)
{
  call1 = new ConditionUpdate1(printLevel);
  call2 = new ConditionUpdate2(printLevel);
  call3 = new ConditionUpdate3(printLevel);
}

/// Destructor
ConditionsDependencyCreator::~ConditionsDependencyCreator()  {
  releasePtr(call1);
  releasePtr(call2);
  releasePtr(call3);
}

/// Callback to process a single detector element
int ConditionsDependencyCreator::operator()(DetElement de, int)    {
  DetConditions     dc(de);
  ConditionKey      key(de.path()+"#derived_data");
  ConditionKey      target1(key.name+"/derived_1");
  ConditionKey      target2(key.name+"/derived_2");
  ConditionKey      target3(key.name+"/derived_3");
  DependencyBuilder build_1(target1, call1);
  DependencyBuilder build_2(target2, call2);
  DependencyBuilder build_3(target3, call3);

  // Compute the derived stuff
  build_1.add(key);

  build_2.add(key);
  build_2.add(target1);
  
  build_3.add(key);
  build_3.add(target1);
  build_3.add(target2);

  slice.insert(build_1.release());
  slice.insert(build_2.release());
  slice.insert(build_3.release());

  /// Add the conditions keys to the container
  dc.conditions().addKey(target1.name);
  dc.conditions().addKey(target2.name);
  dc.conditions().addKey(target3.name);
  /// Add conditions aliases to the container
  dc.conditions().addKey("derived_1",target1.name);
  dc.conditions().addKey("derived_2",target2.name);
  dc.conditions().addKey("derived_3",target3.name);
  printout(printLevel,"Example","++ Added derived conditions dependencies for %s",de.path().c_str());
  return 1;
}

/// Callback to process a single detector element
int ConditionsDataAccess::processElement(DetElement de)  {
  DetConditions dc(de); // Use special facade...
  string path = de.path();
  ConditionKey key_temperature (path+"#temperature");
  ConditionKey key_pressure    (path+"#pressure");
  ConditionKey key_double_table(path+"#double_table");
  ConditionKey key_int_table   (path+"#int_table");
  ConditionKey key_derived_data(path+"#derived_data");
  ConditionKey key_derived1    (path+"#derived_data/derived_1");
  ConditionKey key_derived2    (path+"#derived_data/derived_2");
  ConditionKey key_derived3    (path+"#derived_data/derived_3");
  Conditions::Container container = dc.conditions();
  int result = 0, count = 0;
  // Let's go for the deltas....
  for(const auto& k : container.keys() )  {
    Condition cond = container.get(k.first,*m_pool);
    ++count;
    if ( k.first == key_temperature.hash )  {
      result += int(cond.get<double>());
    }
    else if ( k.first == key_pressure.hash )  {
      result += int(cond.get<double>());
    }
    else if ( k.first == key_double_table.hash )  {
      result += int(cond.get<vector<double> >().size());
    }
    else if ( k.first == key_int_table.hash )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( k.first == key_derived_data.hash )  {
      result += int(cond.get<int>());
    }
    else if ( k.first == key_derived1.hash )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( k.first == key_derived2.hash )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( k.first == key_derived3.hash )  {
      result += int(cond.get<vector<int> >().size());
    }
    if ( !IOV::key_is_contained(iov.key(),cond.iov().key()) )  {
      printout(printLevel,"CondAccess","++ IOV mismatch:%s <> %s",
               iov.str().c_str(), cond.iov().str().c_str());
    }
  }
  for (const auto& c : de.children() )
    count += processElement(c.second);
  return count;
}

template<typename T> Condition ConditionsCreator::make_condition(DetElement de, const string& name, T val)  {
  Condition cond(de.path()+"#"+name, name);
  T& value   = cond.bind<T>();
  value      = val;
  cond->hash = ConditionKey::hashCode(cond->name);
  cond->setFlag(Condition::ACTIVE);
  if ( slice ) slice->insert(ConditionKey(cond->name,cond->hash),ConditionsSlice::NoLoadInfo());
  return cond;
}

/// Destructor
ConditionsCreator::~ConditionsCreator()  {
  printout(printLevel,"Creator","++ Added a total of %d conditions",conditionCount);
}

/// Callback to process a single detector element
int ConditionsCreator::operator()(DetElement de, int)    {
  DetConditions dc(de);
  Condition temperature = make_condition<double>(de,"temperature",1.222);
  Condition pressure    = make_condition<double>(de,"pressure",888.88);
  Condition derived     = make_condition<int>   (de,"derived_data",100);
  Condition dbl_table   = make_condition<vector<double> >(de,"double_table",{1.,2.,3.,4.,5.,6.,7.,8.,9.});
  Condition int_table   = make_condition<vector<int> >   (de,"int_table",{10,20,30,40,50,60,70,80,90});

  manager.registerUnlocked(pool, temperature);
  manager.registerUnlocked(pool, pressure);
  manager.registerUnlocked(pool, derived);
  manager.registerUnlocked(pool, dbl_table);
  manager.registerUnlocked(pool, int_table);
  printout(printLevel,"Creator","++ Adding manually conditions for %s",de.path().c_str());
  conditionCount += 5;
  return 1;
}

/// Callback to process a single detector element
int ConditionsKeys::operator()(DetElement de, int)    {
  DetConditions dc(de);
  dc.conditions().addKey(de.path()+"#temperature");
  dc.conditions().addKey(de.path()+"#pressure");
  dc.conditions().addKey(de.path()+"#double_table");
  dc.conditions().addKey(de.path()+"#int_table");
  dc.conditions().addKey(de.path()+"#derived_data");
  dc.conditions().addKey("derived",de.path()+"#derived_data");
  return 1;
}
