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
#include "Detector/DeVPConditionCalls.h"
#include "DD4hep/DetectorProcessor.h"
#include "DDDB/DDDBConversion.h"

using namespace gaudi;

/// Interface to client Callback in order to update the condition
dd4hep::Condition DeVPStaticConditionCall::operator()(const ConditionKey& key, Context& context)  {
  VPUpdateContext* ctxt = dynamic_cast<VPUpdateContext*>(context.parameter);
  KeyMaker   km(key.hash);
  auto       ide = ctxt->detectors.find(km.values.det_key);
  auto*      cat = (*ide).second.second;
  DetElement det = (*ide).second.first;
  DeStatic s;
  if ( cat->classID == DeVPSensor::classID() )      // DeVPSensor
    s = DeStatic(new detail::DeVPSensorStaticObject());
  else if ( cat->classID == DeVP::classID()  )     // DeVP Velo main element
    s = DeStatic(new detail::DeVPStaticObject());
  else                                     // All other in the hierarchy
    s = DeStatic(new detail::DeVPGenericStaticObject());
  return s->fill_info(det, cat);
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVPStaticConditionCall::resolve(Condition c, Context& context)    {
  DeStatic s(c);
  if ( s->clsID == DeVP::classID() )  {   // Velo main element
    DeVPStatic vp = s;
    DeVPSensorStatic sens;
    DeVPGenericStatic side, support, module, ladder;
    std::vector<std::pair<DetElement, int> > elts;
    dd4hep::DetectorScanner(dd4hep::detElementsCollector(elts), s->detector);

    vp->de_user |= VP::MAIN;
    vp->sensors.resize(200);
    for ( const auto& i : elts )   {
      DetElement de = i.first;
      KeyMaker   key(de.key(), Keys::staticKey);
      DeStatic   cond = context.condition(key.hash);
      const std::string& path = de.path();
      if ( path.find("/VPLeft") != std::string::npos || path.find("/VPRight") != std::string::npos )   {
        switch( i.second )   {
        case 0:  // Self!
          break;
        case 1:
          side = cond;
          side->parent   = vp.access();
          side->de_user |= VP::SIDE;
          vp->sides.push_back(side);
          printout(INFO,"DeVPStatic","Add Side[%03ld]:    %s",vp->sides.size()-1,path.c_str());
          break;
        case 2:
          support = cond;
          support->parent   = side.access();
          support->de_user |= VP::SUPPORT;
          vp->supports.push_back(support);
          printout(INFO,"DeVPStatic","Add Support[%03ld]: %s",vp->supports.size()-1,path.c_str());
          break;
        case 3:
          module = cond;
          module->parent   = support.access();
          module->de_user |= VP::MODULE;
          vp->modules.push_back(module);
          printout(INFO,"DeVPStatic","Add Module[%03ld]:  %s",vp->modules.size()-1,path.c_str());
          break;
        case 4:
          ladder = cond;
          ladder->parent   = module.access();
          ladder->de_user |= VP::LADDER;
          vp->ladders.push_back(ladder);
          printout(INFO,"DeVPStatic","Add Ladder[%03ld]:  %s",vp->ladders.size()-1,path.c_str());
          break;
        case 5:
          sens = cond;
          sens->parent = ladder.access();
          if ( sens->sensorNumber >= vp->sensors.size() )
            vp->sensors.resize(sens->sensorNumber+1);
          vp->sensors[sens->sensorNumber] = sens;
          ladder->sensors.push_back(sens);
          module->sensors.push_back(sens);
          support->sensors.push_back(sens);
          side->sensors.push_back(sens);
          printout(DEBUG,"DeVPStatic","Add Sensor[%03ld]:  %s",long(sens->sensorNumber),path.c_str());
          break;
        default:
          break;
        }
      }
      else   {
        printout(DEBUG,"DeVPStatic","Aux.DetElmenet:   %s",path.c_str());
      }
    }
  }
  s->initialize();
}

/// Interface to client Callback in order to update the condition
dd4hep::Condition DeVPIOVConditionCall::operator()(const ConditionKey&, Context&)   {
  DeIOV iov;
  if ( catalog->classID == DeVPSensor::classID() )     // DeVPSensor
    iov = DeIOV(new detail::DeVPSensorObject());
  else if ( catalog->classID == DeVP::classID()  )     // DeVP Velo main element
    iov = DeIOV(new detail::DeVPObject());
  else                                     // All other in the hierarchy
    iov = DeIOV(new detail::DeVPGenericObject());
  return iov->fill_info(detector, catalog);
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVPIOVConditionCall::resolve(Condition cond, Context& context)   {
  DeIOV iov(cond);
  Condition::detkey_type det_key = iov->detector.key();
  KeyMaker kalign(det_key,dd4hep::align::Keys::alignmentKey);
  KeyMaker kstatic(det_key,Keys::staticKey);

  /// Check that the alignments are computed. We need them here!
  if ( !velo_context->alignments_done.isValid() )  {
    velo_context->alignments_done = context.condition(Keys::alignmentsComputedKey);
  }
  
  std::vector<Condition> conds = context.conditions(det_key);
  iov->de_static = context.condition(kstatic.hash);
  iov->detectorAlignment = context.condition(kalign.hash);
  for ( Condition c : conds )
    iov->conditions.insert(std::make_pair(c.item_key(),c));
  iov->initialize();
}

static void add_generic( detail::DeVPObject* vp,
                         std::vector<DeVPGeneric>& cont,
                         const std::vector<DeVPGenericStatic>& src,
                         dd4hep::cond::ConditionUpdateContext& context)  
{
  for ( const auto& i : src )   {
    dd4hep::ConditionKey::KeyMaker key(i->detector.key(), Keys::deKey);
    DeVPGeneric gen = context.condition(key.hash);
    cont.push_back(gen);
    for ( const auto& j : i->sensors )
      gen->sensors.push_back(vp->sensors[j->sensorNumber]);
    printout(DEBUG,"DeVP","Add [%03ld]:    %s",cont.size()-1,gen->detector.path().c_str());
  }
}

/// Interface to client callback for resolving references or to use data from other conditions
void DeVPConditionCall::resolve(Condition cond, Context& context)   {
  DeIOV iov(cond);
  DeVP  vp(cond);
  DeVPIOVConditionCall::resolve(cond, context);
  DeVPStatic s = vp.access()->vp_static;

  vp->sensors.resize(s->sensors.size());
  for ( const auto& i : s->sensors )   {
    if ( i.isValid() )   {
      KeyMaker   key(i->detector.key(), Keys::deKey);
      DeVPSensor sens = context.condition(key.hash);
      vp->sensors[i->sensorNumber] = sens;
      printout(DEBUG,"DeVP","Add Sensor[%03ld]:  %s",long(i->sensorNumber),i->detector.path().c_str());
    }
  }
  add_generic(vp.ptr(), vp->sides,    s->sides,    context);
  add_generic(vp.ptr(), vp->supports, s->supports, context);
  add_generic(vp.ptr(), vp->modules,  s->modules,  context);
  add_generic(vp.ptr(), vp->ladders,  s->ladders,  context);
}
