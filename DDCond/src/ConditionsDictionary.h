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
#ifndef DDCOND_SRC_CONDITIONSDICTIONARY_H
#define DDCOND_SRC_CONDITIONSDICTIONARY_H

// Framework include files
#include "DDCond/ConditionsPool.h"
#include "DDCond/ConditionsSlice.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsContent.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DDCond/ConditionsRootPersistency.h"
#include "DDCond/ConditionsTreePersistency.h"

namespace { class ConditionsDictionary {};   }

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
using namespace dd4hep::cond;

// These are necessary to support interactivity
#pragma link C++ class UserPool-;
#pragma link C++ class UpdatePool-;
#pragma link C++ class ConditionsPool-;
#pragma link C++ class ConditionsManager-;
#pragma link C++ class ConditionsManagerObject-;
#pragma link C++ class ConditionsIOVPool-;
#pragma link C++ class ConditionsContent-;
#pragma link C++ class ConditionsLoadInfo-;
#pragma link C++ class ConditionsContent::LoadInfo<std::string>-;
#pragma link C++ class std::map<Condition::key_type,ConditionsLoadInfo* >-;
#pragma link C++ class std::map<Condition::key_type,ConditionDependency* >-;

// std::shared_ptr<T> is not yet supported by ROOT!
//#pragma link C++ class std::shared_ptr<ConditionsPool>-;
//#pragma link C++ class std::map<IOV::Key,std::shared_ptr<ConditionsPool> >-;

// This one we need to save conditions pool to file
#pragma link C++ class std::pair<std::string,IOV::Key>+;
#pragma link C++ class std::pair<std::pair<std::string,int>, IOV::Key>+;
#pragma link C++ class std::pair<std::string,std::pair<std::pair<std::string,int>,IOV::Key> >+;

template class std::list< std::pair< std::pair< std::string, std::pair< std::pair<std::string,int>, IOV::Key> >, std::vector<Condition> > >;
#pragma link C++ class std::list<std::pair<std::pair<std::string,std::pair<std::pair<std::string,int>,IOV::Key> >, std::vector<Condition> > >+;

//#pragma link C++ class std::list<ConditionsRootPersistency::Pool>+;
//#pragma link C++ class std::list<ConditionsRootPersistency::IOVPool>+;

#pragma link C++ class ConditionsRootPersistency+;
#pragma link C++ class ConditionsTreePersistency+;

#endif

#endif // DDCOND_SRC_CONDITIONSDICTIONARY_H
