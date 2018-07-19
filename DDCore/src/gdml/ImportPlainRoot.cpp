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
#include "DD4hep/Detector.h"
#include "DD4hep/Memory.h"
#include "DD4hep/DD4hepUI.h"
#include "DD4hep/Factories.h"
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorData.h"
#include "DD4hep/DetectorTools.h"

// ROOT includes
#include "TInterpreter.h"
#include "TGeoElement.h"
#include "TGeoManager.h"
#include "TGDMLParse.h"
#include "TGDMLWrite.h"
#include "TFile.h"
#include "TUri.h"

using namespace std;
using namespace dd4hep;

/// ROOT geometry dump plugin
/**
 *  Factory: DD4hep_PlainROOTDump
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/07/2014
 */
static long plain_root_dump(Detector& description, int argc, char** argv) {
  struct TGeoManip  {
    DetectorData* detector;
    size_t        num_nodes = 0;
    size_t        num_volume_patches = 0;
    size_t        num_placement_patches = 0;
    int           max_level = 9999;
    bool          import = false;
    PrintLevel    printLevel = DEBUG;
    TGeoManip(DetectorData* dsc, int mx, bool imp, PrintLevel p)
      : detector(dsc), max_level(mx), import(imp), printLevel(p)
    {
    }
    ~TGeoManip()   {
      printout(INFO,  "PlainROOTDump","+++ Scanned a total of %9ld NODES",num_nodes);
      if ( import )   {
        printout(INFO,"PlainROOTDump","+++ Scanned a total of %9ld VOLUMES",num_volume_patches);
        printout(INFO,"PlainROOTDump","+++ Scanned a total of %9ld PLACEMENTS",num_placement_patches);
      }
    }
    void operator()(int lvl, TGeoNode* n)    {
      char fmt[255];
      int  npatch = 0;
      TGeoVolume* v = n->GetVolume();
      bool v_ext = false, a_ext = false, p_ext = false;
      if ( import )  {
        if ( !v->GetUserExtension() )   {
          if ( v->IsA() == TGeoVolume::Class() )  {
            v->SetUserExtension(new Volume::Object());
            v_ext = true;
          }
          else   {
            v->SetUserExtension(new Assembly::Object());
            a_ext = true;
          }
          ++npatch;
          ++num_volume_patches;
        }
        if ( !n->GetUserExtension() )   {
          n->SetUserExtension(new PlacedVolume::Object());
          ++num_placement_patches;
          p_ext = true;
          ++npatch;
        }
        if ( lvl == 0 )   {
        }
      }
      ++num_nodes;
      if ( lvl <= max_level )  {
        if ( !import || (import && npatch > 0) )  {
          snprintf(fmt,sizeof(fmt),"%-5d %%-%ds  %%s  NDau:%%d Ext:%%p  Vol:%%s Mother:%%s Ext:%%p Mat:%%s",lvl,lvl);
          TGeoVolume* m = n->GetMotherVolume();
          printout(printLevel,"PlainROOTDump",fmt,"",
                   n->GetName(), n->GetNdaughters(), n->GetUserExtension(),
                   v->GetName(), m ? m->GetName() : "-----",
                   v->GetUserExtension(), v->GetMedium()->GetName());
          if ( import )  {
            if ( v_ext )   {
              ::snprintf(fmt,sizeof(fmt),"%-5d %%-%ds     -->  Adding VOLUME extension object",lvl,lvl);
              printout(printLevel,"PlainROOTDump",fmt,"");
            }
            else if ( a_ext )  {
              ::snprintf(fmt,sizeof(fmt),"%-5d %%-%ds     -->  Adding VOLUME ASSEMBLY extension object",lvl,lvl);
              printout(printLevel,"PlainROOTDump",fmt,"");
            }
            else if ( p_ext )   {
              ::snprintf(fmt,sizeof(fmt),"%-5d %%-%ds     -->  Adding PLACEMENT extension object",lvl,lvl);
              printout(printLevel,"PlainROOTDump",fmt,"");
            }
            if ( lvl == 0 )   {
            }
          }
        }
      }
      for (Int_t idau = 0, ndau = n->GetNdaughters(); idau < ndau; ++idau)  {
        TGeoNode*   daughter = n->GetDaughter(idau);
        this->operator()(lvl+1, daughter);
      }
    }
  };
  if ( argc > 0 )   {
    int    level = 99999;
    PrintLevel prt = DEBUG;
    bool   do_import = false;
    string input, in_obj = "Geometry", air, vacuum;
    for(int i = 0; i < argc && argv[i]; ++i)  {
      if ( 0 == ::strncmp("-input",argv[i],5) )
        input = argv[++i];
      else if ( 0 == ::strncmp("-object",argv[i],5) )
        in_obj = argv[++i];
      else if ( 0 == ::strncmp("-air",argv[i],5) )
        air = argv[++i];
      else if ( 0 == ::strncmp("-vacuum",argv[i],5) )
        vacuum = argv[++i];
      else if ( 0 == ::strncmp("-level",argv[i],5) )
        level = ::atol(argv[++i]);
      else if ( 0 == ::strncmp("-import",argv[i],5) )
        do_import = true;
      else if ( 0 == ::strncmp("-print",argv[i],5) )
        prt = decodePrintLevel(argv[++i]);
      else
        goto Error;
    }
    if ( input.empty() || in_obj.empty() )   {
    Error:
      cout <<
        "Usage: -plugin <name> -arg [-arg]                                            \n"
        "     name:   factory name     DD4hep_ROOTGDMLParse                           \n"
        "     -input  <string>         Input file name.                               \n"
        "     -object <string>         Name of geometry object in file. Default: \"Geometry\"\n"
        "\tArguments given: " << arguments(argc,argv) << endl << flush;
      ::exit(EINVAL);
    }
    printout(INFO,"ROOTGDMLParse","+++ Read geometry from GDML file file:%s",input.c_str());
    DetectorData::unpatchRootStreamer(TGeoVolume::Class());
    DetectorData::unpatchRootStreamer(TGeoNode::Class());
    TFile* f = TFile::Open(input.c_str());
    if ( f && !f->IsZombie() )   {
      DetectorData* det = dynamic_cast<DetectorData*>(&description);
      TGeoManager* mgr = (TGeoManager*)f->Get(in_obj.c_str());
      TGeoManip manip(det, level, do_import, prt);
      DetectorData::patchRootStreamer(TGeoVolume::Class());
      DetectorData::patchRootStreamer(TGeoNode::Class());
      det->m_manager = mgr;
      manip(0, mgr->GetTopNode());
      det->m_worldVol = mgr->GetTopNode()->GetVolume();
      if ( !air.empty() )  {
        description.addConstant(Constant("Air",air));
      }
      if ( !vacuum.empty() )  {
        description.addConstant(Constant("Vacuum",vacuum));
      }
      description.init();
      description.endDocument();
      detail::deleteObject(f);
      return 1;
    }
    detail::deleteObject(f);
  }
  DetectorData::patchRootStreamer(TGeoVolume::Class());
  DetectorData::patchRootStreamer(TGeoNode::Class());
  except("ROOTGDMLParse","+++ No input file name given.");
  return 0;
}
DECLARE_APPLY(DD4hep_PlainROOT,plain_root_dump)
