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
#include "DD4hep/Grammar.h"
#include "DD4hep/DetectorData.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/detail/DetectorInterna.h"

// ROOT include files
#include "TGeoManager.h"
#include "TClassStreamer.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TClass.h"
#include "TROOT.h"


namespace dd4hep {  namespace detail {    class DetectorImp;  }}

using namespace dd4hep::detail;
using namespace dd4hep;

namespace {

  union FundamentalData  {
    Char_t    c;
    Short_t   s;
    Int_t     i;
    Long_t    l;
    Float_t   f;
    Double_t  d;
    UChar_t   uc;
    UShort_t  us;
    UInt_t    ui;
    ULong_t   ul;
    Long64_t  ll;
    ULong64_t ull;
    Bool_t    b;
    void read(TBuffer& buff,int dtyp)   {
      switch(dtyp)  {
      case  1: buff >> c;    break;
      case  2: buff >> s;    break;
      case  3: buff >> i;    break;
      case  4: buff >> l;    break;
      case  5: buff >> f;    break;
      case  6: buff >> i;    break;
        //case  7: buff >> d;   break; // char*
      case  8: buff >> d;    break;
      case  9: buff >> d;    break;
      case 11: buff >> uc;   break;
      case 12: buff >> us;   break;
      case 13: buff >> ui;   break;
      case 14: buff >> ul;   break;
      case 15: buff >> ui;   break;
      case 16: buff >> ll;   break;
      case 17: buff >> ull;  break;
      case 18: buff >> b;    break;
      case 19: buff >> f;    break;
      default:
        printout(ERROR,"OpaqueData","Unknown fundamental data type: [%X]",dtyp);
        break;
      }
    }
    void write(TBuffer& buff,int dtyp)  const  {
      switch(dtyp)  {
      case  1: buff << c;    break;
      case  2: buff << s;    break;
      case  3: buff << i;    break;
      case  4: buff << l;    break;
      case  5: buff << f;    break;
      case  6: buff << i;    break;
        //case  7: buff << d;   break; // char*
      case  8: buff << d;    break;
      case  9: buff << d;    break;
      case 11: buff << uc;   break;
      case 12: buff << us;   break;
      case 13: buff << ui;   break;
      case 14: buff << ul;   break;
      case 15: buff << ui;   break;
      case 16: buff << ll;   break;
      case 17: buff << ull;  break;
      case 18: buff << b;    break;
      case 19: buff << f;    break;
      default:
        printout(ERROR,"OpaqueData","Unknown fundamental data type: [%X]",dtyp);
        break;
      }
    }
  };

  void stream_opaque_datablock(TBuffer& b, void* obj)    {
    UInt_t R__s = 0; // Start of object.
    UInt_t R__c = 0; // Count of bytes.
    TClass* cl = BasicGrammar::instance<OpaqueDataBlock>().clazz();//gROOT->GetClass("dd4hep::OpaqueDataBlock");
   
    OpaqueDataBlock* block = (OpaqueDataBlock*)obj;
    if ( b.IsReading() )  {
      b.ReadVersion(&R__s, &R__c, cl);
      BasicGrammar::key_type key = 0;
      b >> key;
      const BasicGrammar& gr = BasicGrammar::get(key);
      //printout(INFO,"OpaqueData","   Data type:%s  [%016llX]",gr.name.c_str(),key);
      void* ptr = block->ptr();
      if ( !ptr )  { // Some blocks are already bound. Skip those.
        ptr = block->bind(&gr);
        gr.bind(ptr);
      }
      /// Now perform the I/O action
      if ( gr.type() == typeid(std::string) )
        b.ReadStdString(*(std::string*)ptr);
      else if ( gr.clazz() )
        b.ReadClassBuffer(gr.clazz(),ptr);
      else
        ((FundamentalData*)ptr)->read(b,gr.data_type());
      b.CheckByteCount(R__s, R__c, cl);
    }
    else if ( 0 == block->grammar )  {
      printout(ERROR,"OpaqueData","+++ ERROR +++ Opaque data block has no grammar attached. Cannot be saved!");
    }
    else  {
      const BasicGrammar& gr = *block->grammar;
      std::string typ = gr.type_name();
      R__c = b.WriteVersion(cl,kTRUE);
      b << gr.hash();
      //printout(INFO,"OpaqueData","   Data type:%s  Grammar:%s",typ.c_str(),block->grammar->name.c_str());

      /// Now perform the I/O action
      if ( gr.type() == typeid(std::string) )
        b.WriteStdString(*(std::string*)block->ptr());
      else if ( gr.clazz() )
        b.WriteClassBuffer(gr.clazz(),block->ptr());
      else
        ((const FundamentalData*)block->ptr())->write(b,gr.data_type());
      b.SetByteCount(R__c, kTRUE);
    }
  }
}


/// Default constructor
DetectorData::DetectorData()
  : m_manager(0), m_world(), m_trackers(), m_worldVol(),
    m_trackingVol(), m_field(),
    m_buildType(BUILD_DEFAULT), m_extensions(typeid(DetectorData)), m_volManager(),
    m_inhibitConstants(false)
{
  static bool first = true;
  if ( first )   {
    first = false;
    TClass* cl = gROOT->GetClass("dd4hep::OpaqueDataBlock");
    if ( 0 == cl )  {
      except("PersistencyIO","+++ Missing TClass for 'dd4hep::OpaqueDataBlock'.");
    }
    if ( 0 == cl->GetStreamer() )  {
      cl->AdoptStreamer(new TClassStreamer(stream_opaque_datablock));
      printout(INFO,"PersistencyIO","+++ Set Streamer to %s",cl->GetName());
    }
  }
  InstanceCount::increment(this);
}

/// Standard destructor
DetectorData::~DetectorData() {
  clearData();
  InstanceCount::decrement(this);
}

/// Patch the ROOT streamers to adapt for DD4hep
void DetectorData::patchRootStreamer(TClass* cl)   {
  TDataMember* m = 0;
  printout(INFO,"PersistencyIO",
           "+++ Set data member %s.fUserExtension as PERSISTENT.",
           cl->GetName());
  m = cl->GetDataMember("fUserExtension");
  m->SetTitle(m->GetTitle()+2);
  m->SetBit(BIT(2));
}

/// UNPatch the ROOT streamers to adapt for DD4hep
void DetectorData::unpatchRootStreamer(TClass* cl)   {
  TDataMember* m = 0;
  printout(INFO,"PersistencyIO",
           "+++ Set data member %s.fUserExtension as TRANSIENT.",
           cl->GetName());
  m = cl->GetDataMember("fUserExtension");
  m->SetTitle((std::string("! ")+m->GetTitle()).c_str());
  m->ResetBit(BIT(2));
}

/// Clear data content: releases all allocated resources
void DetectorData::destroyData(bool destroy_mgr)   {
  m_extensions.clear();
  m_detectorParents.clear();

  destroyHandle(m_world);
  destroyHandle(m_field);
  destroyHandle(m_header);
  destroyHandles(m_readouts);
  destroyHandles(m_idDict);
  destroyHandles(m_limits);
  destroyHandles(m_regions);
  destroyHandles(m_sensitive);
  destroyHandles(m_display);
  destroyHandles(m_fields);
  destroyHandles(m_define);
#if 0
  for(const auto& d : m_define)   {
    auto c = d;
    std::cout << "Delete " << d.first << std::endl;
    //if ( d.first == "world_side" ) continue;
    delete d.second.ptr();
  }
#endif  
  destroyHandle(m_volManager);
  m_properties.clear();
  m_trackers.clear();
  m_trackingVol.clear();
  m_parallelWorldVol.clear();
  m_worldVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  m_inhibitConstants = false;
  if ( destroy_mgr )  {
    gGeoManager = m_manager;
    deletePtr(m_manager);
    gGeoManager = 0;
  }
  else  {
    gGeoManager = m_manager;
    m_manager = 0;
  }
}


/// Clear data content: releases all allocated resources
void DetectorData::clearData()   {
  m_extensions.clear(false);
  m_detectorParents.clear();
  m_world.clear();
  m_field.clear();
  m_header.clear();
  m_properties.clear();
  m_readouts.clear();
  m_idDict.clear();
  m_limits.clear();
  m_regions.clear();
  m_sensitive.clear();
  m_display.clear();
  m_fields.clear();
  m_define.clear();
  m_trackers.clear();
  m_worldVol.clear();
  m_trackingVol.clear();
  m_parallelWorldVol.clear();
  m_invisibleVis.clear();
  m_materialVacuum.clear();
  m_materialAir.clear();
  m_volManager.clear();
  m_manager = 0;
  m_inhibitConstants = false;
}

/// Adopt all data from source structure
void DetectorData::adoptData(DetectorData& source, bool clr)   {
  m_inhibitConstants   = source.m_inhibitConstants;
  m_extensions.move(source.m_extensions);
  m_manager            = source.m_manager;
  m_readouts           = source.m_readouts;
  m_idDict             = source.m_idDict;
  m_limits             = source.m_limits;
  m_regions            = source.m_regions;
  m_detectors          = source.m_detectors;
  m_sensitive          = source.m_sensitive;
  m_display            = source.m_display;
  m_fields             = source.m_fields;
  m_define             = source.m_define;

  m_detectorParents    = source.m_detectorParents;
  m_world              = source.m_world;
  World w              = m_world;
  w->description       = dynamic_cast<Detector*>(this);
  m_trackers           = source.m_trackers;
  m_worldVol           = source.m_worldVol;
  m_trackingVol        = source.m_trackingVol;
  m_parallelWorldVol   = source.m_parallelWorldVol;
  m_materialAir        = source.m_materialAir;
  m_materialVacuum     = source.m_materialVacuum;
  m_invisibleVis       = source.m_invisibleVis;
  m_field              = source.m_field;
  m_header             = source.m_header;
  m_properties         = source.m_properties;
  //m_extensions       = source.m_extensions;
  m_volManager         = source.m_volManager;

  // Update world element
  m_world.setPlacement(m_manager->GetTopNode());
  // Need to update some global stuff
  if ( gGeoManager != m_manager ) delete gGeoManager;
  gGeoManager         = m_manager;
  if ( clr ) source.clearData();
}
