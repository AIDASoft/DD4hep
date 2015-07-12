// $Id: $
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
#include "CLHEP/Units/SystemOfUnits.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/Printout.h"
#include "DD4hep/LCDD.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Data.h"
#include <vector>
#include <cstdio>
#include <memory>
#include <cerrno>

#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"

using namespace std;
using namespace CLHEP;
using namespace DD4hep;
using namespace DD4hep::Geometry;

typedef DD4hep::Simulation::Geant4Tracker Geant4Tracker;
typedef DD4hep::Simulation::Geant4Calorimeter Geant4Calorimeter;
typedef DD4hep::Simulation::Geant4Particle Geant4Particle;

namespace {
  static const char* line = "+-------------------------------------------------------------+";
  static bool have_geometry = false;

  int usage()  {
    printf("\ndumpDDG4 -opt [-opt]                                                                   \n"
	   "    -compact <compact-geometry>   Supply geometry file to check hits with volume manager.\n"
	   "    -input   <root-file>          File generated with DDG4                               \n"
	   "    -event   <event-number>       Specify event to be dumped. Default: ALL.              \n"
	   "\n\n");
    return EINVAL;
  }

  int printHits(const string& container, vector<Geant4Tracker::Hit*>* hits)   {
    typedef vector<Geant4Tracker::Hit*> _H;
    if ( !hits )  {
      ::printf("+  Invalid Hit container '%s'. No printout\n",container.c_str());
    }
    else if ( hits->empty() )   {
      ::printf("+  Invalid Hit container '%s'. No entries. No printout\n",container.c_str());
    }
    else if ( have_geometry )  {
      string det_name = container;
      LCDD& lcdd = LCDD::getInstance();
      det_name = det_name.substr(0,det_name.length()-4);
      DetElement det(lcdd.detector(det_name));
      SensitiveDetector sd(lcdd.sensitiveDetector(det_name));
      Segmentation seg = sd.readout().segmentation();
      VolumeManager vm = lcdd.volumeManager();
      for(_H::const_iterator i=hits->begin(); i!=hits->end(); ++i)  {
	const Geant4Tracker::Hit* h = *i;
	const Geometry::Position& pos = h->position;
	Geometry::Position pos_cell = seg.position(h->cellID);
	PlacedVolume pv = vm.lookupPlacement(h->cellID);
	printout(ALWAYS,container,
		 "+++ Track:%3d PDG:%6d Pos:(%+.2e,%+.2e,%+.2e)[mm] Pixel:(%+.2e,%+.2e,%+.2e)[mm] %s Deposit:%7.3f MeV CellID:%16lX",
		 h->truth.trackID,h->truth.pdgID,
		 pos.x()/mm,pos.y()/mm,pos.z()/mm,
		 pos_cell.x()/dd4hep::mm,pos_cell.y()/dd4hep::mm,pos_cell.z()/dd4hep::mm,
		 pv.name(),h->truth.deposit/MeV,h->cellID
		 );
	delete h;
      }
    }
    else  {
      string det_name = container;
      for(_H::const_iterator i=hits->begin(); i!=hits->end(); ++i)  {
	const Geant4Tracker::Hit* h = *i;
	const Geometry::Position& pos = h->position;
	printout(ALWAYS,container,
		 "+++ Track:%3d PDG:%6d Pos:(%+.2e,%+.2e,%+.2e)[mm] Deposit:%7.3f MeV CellID:%16lX",
		 h->truth.trackID,h->truth.pdgID,
		 pos.x()/mm,pos.y()/mm,pos.z()/mm,
		 h->truth.deposit/MeV,h->cellID
		 );
	delete h;
      }
    }
    return 0;
  }
  int printHits(const string& container, vector<Geant4Calorimeter::Hit*>* hits)   {
    typedef vector<Geant4Calorimeter::Hit*> _H;
    if ( !hits )  {
      ::printf("+  Invalid Hit container '%s'. No printout\n",container.c_str());
    }
    else if ( hits->empty() )   {
      ::printf("+  Invalid Hit container '%s'. No entries. No printout\n",container.c_str());
    }
    else   {
      string det_name = container;
      LCDD& lcdd = LCDD::getInstance();
      det_name = det_name.substr(0,det_name.length()-4);
      DetElement det(lcdd.detector(det_name));
      SensitiveDetector sd(lcdd.sensitiveDetector(det_name));
      Segmentation seg = sd.readout().segmentation();
      VolumeManager vm = lcdd.volumeManager();
      for(_H::const_iterator i=hits->begin(); i!=hits->end(); ++i)  {
	const Geant4Calorimeter::Hit* h = *i;
	const Geometry::Position& pos = h->position;
	Geometry::Position pos_cell = seg.position(h->cellID);
	PlacedVolume pv = vm.lookupPlacement(h->cellID);
	printout(ALWAYS,container,
		 "+++ Pos:(%+.2e,%+.2e,%+.2e)[mm] Pixel:(%+.2e,%+.2e,%+.2e)[mm] %s Deposit:%7.3f MeV CellID:%16lX",
		 pos.x()/mm,pos.y()/mm,pos.z()/mm,
		 pos_cell.x()/dd4hep::mm,pos_cell.y()/dd4hep::mm,pos_cell.z()/dd4hep::mm,
		 pv.name(),h->energyDeposit/MeV,h->cellID
		 );
	delete h;
      }
    }
    return 0;
  }

  int printParticles(const string& container, vector<Geant4Particle*>* particles)   {
    typedef vector<Geant4Particle*> _P;
    if ( !particles )  {
      ::printf("+  Invalid particle container '%s'. No printout\n",container.c_str());
    }
    else if ( particles->empty() )   {
      ::printf("+  Invalid particle container '%s'. No entries. No printout\n",container.c_str());
    }
    else   {
      for(_P::const_iterator i=particles->begin(); i!=particles->end(); ++i)  {
	DD4hep::Simulation::Geant4ParticleHandle p(*i);
	char text[256];
	text[0]=0;
	if ( p->parents.size() == 1 )
	  ::snprintf(text,sizeof(text),"/%d",*(p->parents.begin()));
	else if ( p->parents.size() >  1 )   {
	  text[0]='/';text[1]=0;
	  for(set<int>::const_iterator i=p->parents.begin(); i!=p->parents.end(); ++i)
	    ::snprintf(text+strlen(text),sizeof(text)-strlen(text),"%d ",*i);
	}
	printout(ALWAYS,container,
		 "+++%s %3d stat:%08X PDG:%6d Mom:(%+.2e,%+.2e,%+.2e)[MeV] "
		 "Vtx:(%+.2e,%+.2e,%+.2e)[mm] #Dau:%3d #Par:%1d%-6s",
		 "",p->id,p->status,p->pdgID,
		 p->psx/MeV,p->psy/MeV,p->psz/MeV,
		 p->vsx/mm,p->vsy/mm,p->vsz/mm,
		 int(p->daughters.size()),
		 int(p->parents.size()),
		 text);
	delete (*i);
      }
    }
    return 0;
  }
}

int dumpDDG4(const char* fname, int event_num)  {
  TFile* data = TFile::Open(fname);
  if ( !data || data->IsZombie() )   {
    printf("+  File seems to not exist. Exiting\n");
    usage();
    return -1;
  }
  TTree* tree = (TTree*)data->Get("EVENT");
  for(int event=0, num=tree->GetEntries(); event<num; ++event)  {
    TObjArray* arr = tree->GetListOfBranches();
    if ( event_num>= 0 ) event = event_num;
    for(int j=0, nj=arr->GetEntries(); j<nj; ++j)   {
      TBranch* b = (TBranch*)arr->At(j);
      typedef vector<void*> _E;
      _E* e = 0;
      b->SetAddress(&e);
      int nbytes = b->GetEvent(event);
      if ( nbytes > 0 )   {
	if ( e->empty() )    {
	  continue;
	}
	string br_name = b->GetName();
	string cl_name = b->GetClassName();
	if ( cl_name.find("DD4hep::Simulation::Geant4Tracker::Hit") != string::npos )  {
	  typedef vector<Geant4Tracker::Hit*> _H;
	  printHits(br_name,(_H*)e);
	}
	else if ( cl_name.find("DD4hep::Simulation::Geant4Calorimeter::Hit") != string::npos )  {
	  typedef vector<Geant4Calorimeter::Hit*> _H;
	  printHits(br_name,(_H*)e);
	}
	else if ( cl_name.find("DD4hep::Simulation::Geant4Particle") != string::npos )  {
	  typedef vector<Geant4Particle*> _H;
	  ::printf("%s\n+    Particle Dump of event %8d  [%8d bytes]        +\n%s\n",
		   line,event,nbytes,line);
	  printParticles(br_name,(_H*)e);
	}
      }
    }
    if ( event_num >= 0 ) break;
  }
  delete data;
  return 0;
}

int dumpddg4_load_geometry(const char* fname)   {
  if ( !have_geometry )  {
    have_geometry = true;
    gSystem->Load("libDDG4Plugins");
    LCDD& lcdd = LCDD::getInstance();
    lcdd.fromXML(fname);
    lcdd.apply("DD4hepVolumeManager",0,(char**)0);
  }
  return 1;
}

#if !(defined(G__DICTIONARY) || defined(__CINT__) || defined(__MAKECINT__)) // No Cint script
int main(int argc, char** argv)  {                            // Main program if linked standalone
  int event_num = -1;
  const char* fname = 0;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-input",2) == 0 )
	fname = argv[++i];
      else if ( strncmp(argv[i],"-compact",2) == 0 )
	dumpddg4_load_geometry(argv[++i]);
      else if ( strncmp(argv[i],"-event",2) == 0 )
	event_num = ::atol(argv[++i]);
    }
  }
  if ( !fname )  {
    return usage();
  }
  return dumpDDG4(fname,event_num);
}
#endif
