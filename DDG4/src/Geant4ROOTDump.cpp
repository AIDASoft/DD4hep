// $Id$
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
#include "DD4hep/LCDD.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4Particle.h"

// ROOT include files
#include "TInterpreter.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

typedef Geant4DataDump::Particles Particles;
typedef Geant4DataDump::TrackerHits TrackerHits;
typedef Geant4DataDump::CalorimeterHits CalorimeterHits;

static long usage()   {
  printout(FATAL,"Geant4ROOTDump","usage: Geant4ROOTDump -opt (app-opts) --opt=value (plugin-opts)");
  printout(FATAL,"Geant4ROOTDump","       app-opts: ");
  printout(FATAL,"Geant4ROOTDump","       -print INFO      Print container summaries only.");
  printout(FATAL,"Geant4ROOTDump","       -print DEBUG     Print container content as well.");
  printout(FATAL,"Geant4ROOTDump","       -print VERBOSE   Print full data.");
  printout(FATAL,"Geant4ROOTDump","       plugin opts: ");
  printout(FATAL,"Geant4ROOTDump","       --usage:         Print this output.");
  printout(FATAL,"Geant4ROOTDump","       --input=<file>   Print content of this file.");
  printout(FATAL,"Geant4ROOTDump","       --entry=<number> Print specified event in the file. (starts with 0!)");
  return 'H';
}

static pair<TClass*,void*> load(TBranch* branch, int entry)   {
  TClass* cl = gROOT->GetClass(branch->GetClassName(),kTRUE);
  if ( !cl )   {
    return pair<TClass*,void*>(0,0);
  }
  void *obj = cl->New();
  branch->SetAddress(&obj);
  branch->SetAutoDelete(kFALSE);
  int nb = branch->GetEntry(entry);
  if ( nb < 0 )    {
    cl->Destructor(obj);
    return pair<TClass*,void*>(0,0);
  }
  return pair<TClass*,void*>(cl,obj);
}

static long dump_root(DD4hep::Geometry::LCDD&, int argc, char** argv) {
  std::string input = "", tag="Geant4ROOTDump";
  int entry = -1;

  for(int j=0; j<argc; ++j)  {
    std::string a = argv[j];
    if ( a[0]=='-' ) a = argv[j]+1;
    if ( a[0]=='-' ) a = argv[j]+2;
    size_t idx = a.find('=');
    if ( strncmp(a.c_str(),"usage",5)==0 )  {
      usage();
      return 1;
    }
    if ( idx > 0 )  {
      string p1 = a.substr(0,idx);
      string p2 = a.substr(idx+1);
      if ( strncmp(p1.c_str(),"input",3)==0 )  {
        input = p2;
      }
      if ( strncmp(p1.c_str(),"entry",5)==0 )  {
        if ( 1 != ::sscanf(p2.c_str(),"%d",&entry) )  {
          printout(FATAL,tag,"+++ Argument %s is not properly formatted. must be --entry=<number>.",argv[j]);
          return usage();
        }
      }
      continue;
    }
    printout(FATAL,tag,"+++ Argument %s is IGNORED! No value is found (string has no '=')",argv[j]);
    return usage();
  }
  const char* line = "+----------------------------------------------------------------------------------+";
  printout(INFO,tag,line);
  printout(INFO,tag,"|  Input:%-74s|",input.c_str());
  printout(INFO,tag,line);

  if ( input.empty() ) {
    return usage();
  }


  Geant4DataDump dump("Geant4Data");
  TFile* f = TFile::Open(input.c_str());

  if ( f && !f->IsZombie() )  {
    TTree* tree = (TTree*)f->Get("EVENT");
    TClass* cl_calo = gROOT->GetClass(typeid(CalorimeterHits));
    TClass* cl_tracker = gROOT->GetClass(typeid(TrackerHits));
    TClass* cl_particles = gROOT->GetClass(typeid(Particles));
    TObjArray* branches  = tree->GetListOfBranches();
    Int_t nbranches = branches->GetEntriesFast();
    typedef pair<TClass*,void*> ENTRY;
    typedef map<string,ENTRY> ENTRIES;

    for(Int_t ievt=entry<0 ? 0 : entry, nevt=entry<0 ? tree->GetEntries() : entry+1; ievt<nevt; ++ievt)  {
      ENTRIES event;
      printout(INFO,tag,line);
      printout(INFO,tag,"|  Event: %6d                                                                   |",ievt);
      printout(INFO,tag,line);

      // First suck in all data
      for (Int_t i=0;i<nbranches;i++)  {
        TBranch* branch = (TBranch*)branches->UncheckedAt(i);
        pair<TClass*,void*> data = load(branch,ievt);
        if ( data.first ) event[branch->GetName()] = data;
      }
      // Now dump the stuff
      for(ENTRIES::const_iterator i=event.begin(); i!=event.end(); ++i)  {
        pair<TClass*,void*> data = (*i).second;
        if ( data.first == cl_particles )  {
          Particles* parts = (Particles*)data.second;
          dump.print(INFO, (*i).first, parts);
          for_each(parts->begin(), parts->end(), DestroyObject<Geant4Particle*>());
        }
      }
      for(ENTRIES::const_iterator i=event.begin(); i!=event.end(); ++i)  {
        pair<TClass*,void*> data = (*i).second;
        if ( data.first == cl_particles )  {
        }
        else if ( data.first == cl_tracker )   {
          TrackerHits* hits = (TrackerHits*)data.second;
          dump.print(INFO, (*i).first, hits);
          for_each(hits->begin(), hits->end(), DestroyObject<Geant4Tracker::Hit*>());
        }
        else if ( data.first == cl_calo )   {
          CalorimeterHits* hits = (CalorimeterHits*)data.second;
          dump.print(INFO, (*i).first, hits);
          for_each(hits->begin(), hits->end(), DestroyObject<Geant4Calorimeter::Hit*>());
        }
        if ( data.first ) data.first->Destructor(data.second);
      }
    }
    delete f;
  }
  else   {
    printout(FATAL,tag,"+++ FAILED to open input file:%s",input.c_str());
    return usage();
  }
  return 1;
}

DECLARE_APPLY(Geant4ROOTDump,dump_root)
