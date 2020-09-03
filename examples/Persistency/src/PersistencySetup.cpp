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
#include "PersistencySetup.h"

// ROOT include files
#include "TFile.h"

using namespace std;
using namespace dd4hep;
using namespace PersistencyExamples;

/// Print conditions object
int dd4hep::PersistencyExamples::printCondition(Condition cond)   {
  const BasicGrammar* gr = cond.data().grammar;
  int result = 0;

  if ( gr->type() == typeid(int) )
    result += int(cond.get<int>());
  else if ( gr->type() == typeid(long) )
    result += int(cond.get<long>());
  else if ( gr->type() == typeid(float) )
    result += int(cond.get<float>());
  else if ( gr->type() == typeid(double) )
    result += int(cond.get<double>());
  else if ( gr->type() == typeid(string) )
    result += cond.get<string>().length();
  else if ( gr->type() == typeid(Delta) )
  { ++result; cond.get<Delta>(); }
  else if ( gr->type() == typeid(AlignmentData) )   {
    if ( dynamic_cast<detail::AlignmentObject*>(cond.ptr()) )  {
      AlignmentCondition ac = cond;
      bool ok = (void*)cond.data().ptr() == (void*)&ac->values();
      printout(ok ? INFO : ERROR,
               "Data","+++ %s +++ \t\tAlignmentCondition: %s [%p] -- %p %s",
               ok ? "SUCCESS" : "ERROR", cond.name(),cond.data().ptr(),&ac->values(),
               ok ? "[Good-payload-mapping]" : "[Bad-payload-mapping]");
    }
    ++result;
    cond.get<AlignmentData>();
  }
  else if ( gr->type() == typeid(vector<int>) )
    result += int(cond.get<vector<int> >().size());
  else if ( gr->type() == typeid(vector<long>) )
    result += int(cond.get<vector<long> >().size());
  else if ( gr->type() == typeid(vector<float>) )
    result += int(cond.get<vector<float> >().size());
  else if ( gr->type() == typeid(vector<double>) )
    result += int(cond.get<vector<double> >().size());
  else if ( gr->type() == typeid(vector<string>) )
    result += int(cond.get<vector<string> >().size());

  else if ( gr->type() == typeid(map<string,int>) )  {
    const map<string,int>& mapping = cond.get<map<string,int> >();
    result += int(mapping.size());
    for(const auto& i : mapping )   {
      result += i.second;
      printout(INFO,"Data","\t\tMap: %s [%s] -> %d",cond.name(), i.first.c_str(),i.second);
    }
  }
  printout(INFO,"Data","Condition: [%016llX] %-24s -> %s",cond.key(), cond.name(), cond.data().str().c_str());
  return result;
}

/// Default constructor
PersistencyIO::PersistencyIO()   {
}

/// Default destructor
PersistencyIO::~PersistencyIO()   {
}

/// Generic object write method
int PersistencyIO::write(const std::string& fname, const std::string& title, const std::type_info& typ, const void* object)   {
  TFile* f = TFile::Open(fname.c_str(),"RECREATE");
  if ( f && !f->IsZombie()) {
    /// Now we write the object
    int nBytes = f->WriteObjectAny(object,TBuffer::GetClass(typ),title.c_str());
    f->Close();
    return nBytes;
  }
  return 0;
}
