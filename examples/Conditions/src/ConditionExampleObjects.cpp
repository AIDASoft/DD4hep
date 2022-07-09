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
Condition ConditionNonDefaultCtorUpdate1::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionNonDefaultCtor1","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionNonDefaultCtor1","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  target.construct<NonDefaultCtorCond>(1,2,3);
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionNonDefaultCtorUpdate1::resolve(Condition target, ConditionUpdateContext& context)  {
  NonDefaultCtorCond& data  = target.get<NonDefaultCtorCond>();
  Condition    cond0 = context.condition(context.key(0));
  data.set(cond0.get<int>());
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdateUnresolved::operator()(const ConditionKey& key, ConditionUpdateContext&)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdateUnresolved","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdateUnresolved","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  target.construct<vector<int> >();
  return target;
}

/// Interface to client Callback in order to update the condition
void ConditionUpdateUnresolved::resolve(Condition target, ConditionUpdateContext& context)  {
  vector<int>& data  = target.get<vector<int> >();
  Condition    cond0 = context.condition(context.key(0));
  data.emplace_back(cond0.get<int>());
  data.emplace_back(cond0.get<int>()*2);
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
  target.construct<vector<int> >();
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
  target.construct<vector<int> >();
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

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate4::operator()(const ConditionKey& key, ConditionUpdateContext& context)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate4","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate4","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  vector<int>& data  = target.bind<vector<int> >();
  Condition    cond3 = context.condition(context.key(0));
  Condition    cond2 = context.condition(context.key(1));
  Condition    cond0 = context.condition(context.key(2));
  Condition    cond1 = context.condition(context.key(3));

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.get<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());

  vector<int>& c3 = cond3.get<vector<int> >();
  data.insert(data.end(), c3.begin(), c3.end());
  return target;
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate5::operator()(const ConditionKey& key, ConditionUpdateContext& context)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate5","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate5","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  vector<int>& data  = target.bind<vector<int> >();
  Condition    cond3 = context.condition(context.key(0));
  Condition    cond2 = context.condition(context.key(1));
  Condition    cond0 = context.condition(context.key(2));
  Condition    cond1 = context.condition(context.key(3));
  Condition    cond4 = context.condition(context.key(4));

  data.push_back(cond0.get<int>());
  data.push_back(cond0.get<int>()*2);
  vector<int>& c1 = cond1.get<vector<int> >();
  data.insert(data.end(), c1.begin(), c1.end());

  vector<int>& c2 = cond2.get<vector<int> >();
  data.insert(data.end(), c2.begin(), c2.end());

  vector<int>& c3 = cond3.get<vector<int> >();
  data.insert(data.end(), c3.begin(), c3.end());

  vector<int>& c4 = cond4.get<vector<int> >();
  data.insert(data.end(), c4.begin(), c4.end());
  return target;
}

/// Interface to client Callback in order to update the condition
Condition ConditionUpdate6::operator()(const ConditionKey& key, ConditionUpdateContext& context)  {
#ifdef DD4HEP_CONDITIONS_DEBUG
  printout(printLevel,"ConditionUpdate6","++ Building dependent condition: %016llX  [%s]",key.hash, key.name.c_str());
  Condition    target(key.name,"derived");
#else
  printout(printLevel,"ConditionUpdate6","++ Building dependent condition: %016llX",key.hash);
  Condition    target(key.hash);
#endif
  auto* dep = context.dependency;
  vector<Condition>& data = target.bind<vector<Condition> >();
  data.reserve(dep->dependencies.size());
  for ( size_t i=0; i<dep->dependencies.size(); ++i )   {
    Condition c = context.condition(context.key(i));
    if ( c.get<string>().empty() )  {
      printout(printLevel,"ConditionUpdate6","++ Invalid dependent condition in: %016llX",key.hash);      
    }
    data.emplace_back(c);
  }
  max_deps = std::max(data.size(), max_deps);
  min_deps = std::min(data.size(), min_deps);
  num_deps += data.size();
  ++call_count;
  return target;
}
namespace {
  size_t num_depath_checks = 0;
  size_t num_depath_entries = 0;
}
/// Default destructor
ConditionUpdate6::~ConditionUpdate6()   {
  printout(ALWAYS,"Statistics","+++ Variable dependencies:  MIN: %ld MAX: %ld MEAN:%.3f COUNT:%d",
	   min_deps, max_deps, double(num_deps)/double(std::max(call_count,1UL)), call_count);
      /// Statistics
  printout(ALWAYS,"Statistics","+++ DePath: Entries: %ld  Checks:%ld",num_depath_entries,num_depath_checks);
}

/// Initializing constructor
ConditionsDependencyCreator::ConditionsDependencyCreator(ConditionsContent& c, PrintLevel p, bool persist, int ex)
  : OutputLevel(p), content(c), persist_conditions(persist), extended(ex)
{
  scall1 = std::shared_ptr<ConditionUpdateCall>(new ConditionNonDefaultCtorUpdate1(printLevel));
  call1  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate1(printLevel));
  call2  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate2(printLevel));
  call3  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate3(printLevel));
  call4  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate4(printLevel));
  call5  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate5(printLevel));
  call6  = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdate6(printLevel));
  callUnresolved = std::shared_ptr<ConditionUpdateCall>(new ConditionUpdateUnresolved(printLevel));
}

/// Callback to process a single detector element
int ConditionsDependencyCreator::operator()(DetElement de, int)  const  {
  ConditionKey      key     (de,"derived_data");
  ConditionKey      key_path(de,"derived_data/de_path");
  ConditionKey      starget1(de,"derived_data/NonDefaultCtor_1");
  ConditionKey      target1(de,"derived_data/derived_1");
  ConditionKey      target2(de,"derived_data/derived_2");
  ConditionKey      target3(de,"derived_data/derived_3");
  ConditionKey      target4(de,"derived_data/derived_4");
  ConditionKey      target5(de,"derived_data/derived_5");
  DependencyBuilder sbuild_1(de, starget1.item_key(), scall1);
  DependencyBuilder build_1(de, target1.item_key(), call1);
  DependencyBuilder build_2(de, target2.item_key(), call2);
  DependencyBuilder build_3(de, target3.item_key(), call3);

  // Compute the derived stuff
  sbuild_1.add(key);
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
  if ( extended >= 2 )   {
    DependencyBuilder build_5(de, target5.item_key(), call5);
    build_5.add(target3);
    build_5.add(target2);
    build_5.add(key);
    build_5.add(target1);
    build_5.add(target4);
    content.addDependency(build_5.release());
  }
  if ( extended >= 3 )   {
    DependencyBuilder build_6(de, key_path.item_key(), call6);
    if ( de.parent().isValid() )
      build_6.add(ConditionKey(de.parent(),"de_path"));
    for(const auto& c : de.children())   {
      build_6.add(ConditionKey(c.second,"de_path"));
    }
    content.addDependency(build_6.release());
  }
  if ( extended >= 4 )   {
    DependencyBuilder build_7(de, "Unresolved_dependency", callUnresolved);
    build_7.add(ConditionKey(de.parent(),"unresolved_data"));
    content.addDependency(build_7.release());
  }
  if ( !persist_conditions )  {
    content.addDependency(sbuild_1.release());
  }
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
  ConditionKey key_path        (de,"de_path");
  ConditionKey key_temperature (de,"temperature");
  ConditionKey key_pressure    (de,"pressure");
  ConditionKey key_double_table(de,"double_table");
  ConditionKey key_int_table   (de,"int_table");
  ConditionKey key_derived_data(de,"derived_data");
  ConditionKey key_depath      (de,"derived_data/de_path");
  ConditionKey key_noctor_1    (de,"derived_data/NonDefaultCtor_1");
  ConditionKey key_derived1    (de,"derived_data/derived_1");
  ConditionKey key_derived2    (de,"derived_data/derived_2");
  ConditionKey key_derived3    (de,"derived_data/derived_3");
  ConditionKey key_derived4    (de,"derived_data/derived_4");
  ConditionKey key_derived5    (de,"derived_data/derived_5");
  int result = 0, count = 0;

  // Let's go for the deltas....
  for( auto cond : conditions )  {
    // const auto& info = cond.descriptor().type();
    if ( 0 == dynamic_cast<detail::ConditionObject*>(cond.ptr()) )  {
      printout(ERROR,"accessConditions","Condition with bad base class!");
    }
   
    if ( cond.item_key() == key_path.item_key() )  {
      ++result;
      if ( cond.get<string>() != de.path() )
	printout(ERROR,"CondAccess","++ string:%s <> %s",de.path().c_str(), cond.get<string>().c_str());
    }
    else if ( cond.item_key() == key_depath.item_key() )  {
      vector<Condition>& data = cond.get<vector<Condition> >();
      size_t cnt = 0;
      ++num_depath_entries;
      if ( de.parent().isValid() )  {
	++num_depath_checks;
	if ( data[0].get<string>() != de.parent().path() )  {
	  printout(ERROR,"CondAccess","++ string:%s <> %s",
		   de.parent().path().c_str(), data[0].get<string>().c_str());
	}
	++cnt;
      }
      for(const auto& child : de.children())   {
	const Condition& c = data[cnt];
	++num_depath_checks;
	if ( c.get<string>() != child.second.path() )  {
	  printout(ERROR,"CondAccess","++ string:%s <> %s",
		   child.second.path().c_str(), c.get<string>().c_str());
	}
	++cnt;
      }
    }
    else if ( cond.item_key() == key_temperature.item_key() )  {
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
    else if ( cond.item_key() == key_derived4.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( cond.item_key() == key_derived5.item_key() )  {
      result += int(cond.get<vector<int> >().size());
    }
    else if ( cond.item_key() == key_noctor_1.item_key() )  {
      const NonDefaultCtorCond& c = cond.get<NonDefaultCtorCond>(); 
      result += c.a + c.b + c.b + c.d;
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
  content.insertKey(ConditionKey(de,"de_path").hash);
  return 1;
}

template<typename T>
Condition ConditionsCreator::make_condition(DetElement de, const string& name, const T& val)  const {
  Condition cond(de.path()+"#"+name, name);
  T& value   = cond.bind<T>();
  value      = val;
  cond->hash = ConditionKey::hashCode(de,name);
  return cond;
}

template<typename T, typename... Args>
Condition ConditionsCreator::make_condition_args(DetElement de, const string& name, Args... args)  const {
  Condition cond(de.path()+"#"+name, name);
  T& value   = cond.construct<T>(std::forward<Args>(args)...);
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
  Condition path        = make_condition<std::string>    (de,"de_path",de.path());

  slice.manager.registerUnlocked(pool, temperature);
  slice.manager.registerUnlocked(pool, pressure);
  slice.manager.registerUnlocked(pool, derived);
  slice.manager.registerUnlocked(pool, dbl_table);
  slice.manager.registerUnlocked(pool, int_table);
  slice.manager.registerUnlocked(pool, path);
  printout(printLevel,"Creator","++ Adding manually conditions for %s",de.path().c_str());
  return 5;
}
