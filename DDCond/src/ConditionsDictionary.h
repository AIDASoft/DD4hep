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
#ifndef DD4HEP_CONDITIONS_CONDITIONSDICTIONARY_H
#define DD4HEP_CONDITIONS_CONDITIONSDICTIONARY_H

// Framework include files
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsContent.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DDCond/ConditionsTreePersistency.h"

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::detail;
#pragma link C++ namespace dd4hep::cond;

using namespace dd4hep;

// These are necessary to support interactivity
#pragma link C++ class cond::UserPool-;
#pragma link C++ class cond::UpdatePool-;
#pragma link C++ class cond::ConditionsPool-;
#pragma link C++ class cond::ConditionsManager-;
#pragma link C++ class cond::ConditionsManagerObject-;
#pragma link C++ class cond::ConditionsIOVPool-;
#pragma link C++ class cond::ConditionsContent-;
#pragma link C++ class cond::ConditionsLoadInfo-;
#pragma link C++ class cond::ConditionsContent::LoadInfo<std::string>-;
#pragma link C++ class std::map<Condition::key_type,cond::ConditionsLoadInfo* >-;
#pragma link C++ class std::map<Condition::key_type,cond::ConditionDependency* >-;

// std::shared_ptr<T> is not yet supported by ROOT!
//#pragma link C++ class std::shared_ptr<cond::ConditionsPool>-;
//#pragma link C++ class std::map<IOV::Key,std::shared_ptr<cond::ConditionsPool> >-;

// This one we need to save conditions pool to file
#pragma link C++ class std::pair<std::string,IOV::Key>+;
#pragma link C++ class std::pair<std::pair<std::string,int>, IOV::Key>+;
#pragma link C++ class std::pair<std::string,std::pair<std::pair<std::string,int>,IOV::Key> >+;
#pragma link C++ class std::list<std::pair<std::pair<std::string,std::pair<std::pair<std::string,int>,IOV::Key> >, std::vector<Condition> >+;

//#pragma link C++ class std::list<cond::ConditionsRootPersistency::Pool>+;
//#pragma link C++ class std::list<cond::ConditionsRootPersistency::IOVPool>+;

#pragma link C++ class cond::ConditionsRootPersistency+;
#pragma link C++ class cond::ConditionsTreePersistency+;

#endif

#endif /* DD4HEP_CONDITIONS_CONDITIONSDICTIONARY_H  */
