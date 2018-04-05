//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================

// Framework include files
#include "Detector/DeVeloConditionCalls.h"
#include "DD4hep/DetectorProcessor.h"
#include "DDDB/DDDBConversion.h"

using namespace std;
using namespace gaudi;

/// Interface to client Callback in order to update the condition
dd4hep::Condition DeVeloStaticConditionCall::operator()(const ConditionKey& key, Context& context)  {
  VeloUpdateContext* ctxt = dynamic_cast<VeloUpdateContext*>(context.parameter);
  KeyMaker   km(key.hash);
  auto       ide = ctxt->detectors.find(km.values.det_key);
  auto*      cat = (*ide).second.second;
  DetElement det = (*ide).second.first;
  DeStatic s;
  if ( cat->classID == DeVeloSensor::classID() || cat->classID == 1008102 || cat->classID == 1008103 )      // DeVeloSensor
    s = DeStatic(new detail::DeVeloSensorStaticObject());
  else if ( cat->classID == DeVelo::classID()  )     // DeVelo Velo main element
    s = DeStatic(new detail::DeVeloStaticObject());
  else                                               // All other in the hierarchy
    s = DeStatic(new detail::DeVeloGenericStaticObject());
  return s->fill_info(det, cat);
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVeloStaticConditionCall::resolve(Condition c, Context& context)    {
  DeStatic s(c);
  if ( s->clsID == DeVelo::classID() )  {   // Velo main element
    DeVeloStatic vp = s;
    DeVeloSensorStatic sens;
    DeVeloGenericStatic side, module, support;
    vector<pair<DetElement, int> > elts;
    dd4hep::DetectorScanner(dd4hep::detElementsCollector(elts), s->detector);

    vp->de_user |= DeVeloFlags::MAIN;
    vp->sides.resize(2);
    
    for ( const auto& i : elts )   {
      DetElement de = i.first;
      KeyMaker   key(de.key(), Keys::staticKey);
      DeStatic   cond = context.condition(key.hash);
      const string& path = de.path();
      bool   left  = path.find("/VeloLeft/Module") != string::npos   || path.find("/VeloLeft")  == path.length()-9;
      bool   right = path.find("/VeloRight/Module") != string::npos  || path.find("/VeloRight") == path.length()-10;
      size_t sideNo  = left ? DeVeloFlags::LEFT : right ? DeVeloFlags::RIGHT : 99999;

      printout(INFO,"DeVelo"," %03d DetElement: %s",i.second, de.path().c_str());
      if ( left || right )   {
        switch( i.second )   {
        case 0:  // Self!
          break;
        case 1:
          side = cond;
          side->parent = vp.access();
          side->de_user |= DeVeloFlags::SIDE;
          vp->sides[sideNo] = side;
          printout(INFO,"DeVeloStatic","Add Side[%03ld]:    %s",vp->sides.size()-1,path.c_str());
          break;
        case 2:
          module = cond;
          module->parent = side.access();
          module->de_user |= DeVeloFlags::MODULE;
          side->children.push_back(module.ptr());
          vp->modules.push_back(module);
          printout(INFO,"DeVeloStatic","Add Module[%03ld]:  %s",vp->modules.size()-1,path.c_str());
          break;
        case 3:
          support = cond;
          support->parent = module.access();
          support->de_user |= DeVeloFlags::SUPPORT;
          vp->supports.push_back(support);
          module->children.push_back(support.ptr());
          printout(INFO,"DeVeloStatic","Add Support[%03ld]:  %s",vp->supports.size()-1,path.c_str());
          break;
        case 4:
          sens = cond;
          if ( !sens.isValid() )   {
            except("DeVeloStatic","Problem: Unknown sensor type or invalid object encountered!");
          }
          sens->parent = support.access();
          vp->sensors[DeVeloFlags::ALL].push_back(sens);
          vp->sensors[sideNo].push_back(sens);
          // ---- Contiguous array of sensors
          vp->rSensors[sideNo].push_back(sens);
          vp->phiSensors[sideNo].push_back(sens);
          vp->puSensors[sideNo].push_back(sens);
          vp->rphiSensors[sideNo].push_back(sens);
          support->sensors.push_back(sens);
          module->sensors.push_back(sens);
          side->sensors.push_back(sens);
          printout(INFO,"DeVeloStatic","Add Sensor[%03ld]:  %s",long(sens->sensorNumber),path.c_str());
          break;
        default:
          break;
        }
      }
      else   {
        printout(INFO,"DeVeloStatic","Aux.DetElmenet:   %s",path.c_str());
      }
    }
  }
  s->initialize();
}

/// Interface to client Callback in order to update the condition
dd4hep::Condition DeVeloIOVConditionCall::operator()(const ConditionKey&, Context&)   {
  DeIOV iov;
  if ( catalog->classID == DeVeloSensor::classID() || catalog->classID == 1008102 || catalog->classID == 1008103 )     // DeVeloSensor
    iov = DeIOV(new detail::DeVeloSensorObject());
  else if ( catalog->classID == DeVelo::classID()  )     // DeVelo Velo main element
    iov = DeIOV(new detail::DeVeloObject());
  else                                     // All other in the hierarchy
    iov = DeIOV(new detail::DeVeloGenericObject());
  return iov->fill_info(detector, catalog);
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVeloIOVConditionCall::resolve(Condition cond, Context& context)   {
  DeIOV iov(cond);
  Condition::detkey_type det_key = iov->detector.key();
  KeyMaker kalign(det_key,dd4hep::align::Keys::alignmentKey);
  KeyMaker kstatic(det_key,Keys::staticKey);

  /// Check that the alignments are computed. We need them here!
  if ( !velo_context->alignments_done.isValid() )  {
    velo_context->alignments_done = context.condition(Keys::alignmentsComputedKey);
  }
  
  vector<Condition> conds = context.conditions(det_key);
  iov->de_static = context.condition(kstatic.hash);
  iov->detectorAlignment = context.condition(kalign.hash);
  for ( Condition c : conds )
    iov->conditions.insert(make_pair(c.item_key(),c));
  iov->initialize();
}

namespace {
  /// Resolve generic parent-sensor dependencies
  void add_sensors( vector<DeVeloSensor>& cont,
                    const vector<DeVeloSensorStatic>& src,
                     map<DeVeloSensorStatic,DeVeloSensor>& mapping)    {
    cont.reserve(src.size());
    for (DeVeloSensorStatic i : src)
      cont.push_back(mapping[i]);
  }

  /// Resolve generic parent-sensor dependencies
  void add_sensors( DeVeloGeneric                    gen,
                    DeVeloGenericStatic              src,
                    map<DeVeloSensorStatic,DeVeloSensor>& mapping,
                    dd4hep::cond::ConditionUpdateContext& context)
  {
    gen->sensors.reserve(src->sensors.size());
    for (DeVeloSensorStatic i : src->sensors)   {
      DeVeloSensor sens = mapping[i];
      if ( !sens.isValid() )  {
        cout << "Problem" << endl;
      }
      gen->sensors.push_back(sens);
    }
    for (detail::DeVeloGenericStaticObject* i : src->children)   {
      dd4hep::ConditionKey::KeyMaker key(i->detector.key(), Keys::deKey);
      DeVeloGeneric child = context.condition(key.hash);
      gen->children.push_back(child.ptr());
      add_sensors(child, DeVeloGenericStatic(i), mapping, context);
    }
  }
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVeloConditionCall::resolve(Condition cond, Context& context)  {
  DeIOV iov(cond);
  DeVelo  vp(cond);
  DeVeloIOVConditionCall::resolve(cond, context);
  DeVeloStatic s = vp.access()->vp_static;
  map<DeVeloSensorStatic,DeVeloSensor>   sensorMapping;
  
  sensorMapping[DeVeloSensorStatic()] = DeVeloSensor();
  for ( const auto& i : s->sensors[DeVeloFlags::ALL] )   {
    if ( i.isValid() )   {
      KeyMaker     key(i->detector.key(), Keys::deKey);
      DeVeloSensor sens = context.condition(key.hash);
      if ( !sens.isValid() )  {
        cout << "Problem Mapping " << (void*)i.ptr()
             << " ---> " << (void*)sens.ptr() << " " << i->detector.path()
             << endl;
      }
      sensorMapping[i] = sens;
      continue;
    }
  }

  for(size_t iside = 0; iside<3; ++iside)   {
    add_sensors(vp->sensors[iside],     s->sensors[iside],     sensorMapping);
    add_sensors(vp->rSensors[iside],    s->rSensors[iside],    sensorMapping);
    add_sensors(vp->phiSensors[iside],  s->phiSensors[iside],  sensorMapping);
    add_sensors(vp->rphiSensors[iside], s->rphiSensors[iside], sensorMapping);
    add_sensors(vp->puSensors[iside],   s->puSensors[iside],   sensorMapping);
  }
  for ( auto side : s->sides )   {
    dd4hep::ConditionKey::KeyMaker key(side->detector.key(), Keys::deKey);
    DeVeloGeneric child = context.condition(key.hash);
    vp->sides.push_back(child);
    add_sensors(child, side, sensorMapping, context);
  }
}
