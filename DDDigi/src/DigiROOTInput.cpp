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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiROOTInput.h>
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>

// ROOT include files
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

// C/C++ include files
#include <stdexcept>
#include <map>

using namespace dd4hep::digi;

/// Helper class to hide connection parameters
/**
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiROOTInput::internals_t   {
public:
  typedef std::function<void(DataSegment&, int, const char*, void*)> func_t;

  class converter_t   {
  public:
    TBranch* branch   { nullptr };
    TClass*  cls      { nullptr };
    std::unique_ptr<func_t>   call     { };
  };

  /// Mutex to allow re-use of a single source for multiple input streams
  std::mutex m_lock        { };

  /// Branches present in the current file
  std::map<unsigned long, converter_t>  branches;
  /// Reference to the current ROOT file to be read
  TFile*     file   { };
  /// Reference to the ROOT tree to be read
  TTree*     tree   { nullptr };
  /// Pointer to current input source
  int        input  { INPUT_START };
  /// Current entry inside the current input source
  Long64_t   entry  { -1 };

public:
  /// Default constructor
  internals_t () = default;
  /// Default destructor
  ~internals_t ()   {
    if ( file )    {
      file->Close();
      delete file;
      file = nullptr;
    }
  }
  /// Coverter function from input data to DDDige data
  std::unique_ptr<func_t> input_converter(TClass* cl)  const   {
    using detail::str_replace;
    std::string cln = str_replace(cl->GetName(),"::","_");
    cln = str_replace(str_replace(str_replace(cln,'<','_'),'>','_'),'*',"");
    std::string factory = "DD4hep_DDDigiConverter_"+cln;
    func_t *fptr = (func_t*)PluginService::Create<void*>(factory.c_str());
    if ( 0 == fptr )   {
      dd4hep::except("DigiROOTInput","Failed to access function pointer to read ROOT datatype: %s",cl->GetName());
    }
    return std::unique_ptr<func_t>(fptr);
  }
};


/// Standard constructor
DigiROOTInput::DigiROOTInput(const DigiKernel& kernel, const std::string& nam)
  : DigiInputAction(kernel, nam)
{
  declareProperty("tree", m_tree_name = "EVENT");
  declareProperty("location", m_location = "inputs");
  declareProperty("containers", m_containers);
  InstanceCount::increment(this);
}

/// Default destructor
DigiROOTInput::~DigiROOTInput()   {
  InstanceCount::decrement(this);
}

/// Open new input file
void DigiROOTInput::open_new_file()   const  {
  int len = this->m_inputs.size();
  imp = std::make_unique<internals_t>();
  if ( this->m_inputs.empty() ) imp->input = 0;
  while ( (imp->input+1) < len )   {
    const auto& fname = m_inputs[++imp->input];
    imp->file = TFile::Open(fname.c_str());
    if ( imp->file && imp->file->IsZombie() )  {
      delete imp->file;
      imp->file = nullptr;
      error("OpenInput ++ Failed to open input source %s", fname.c_str());
    }
    else if ( imp->file )  {
      imp->tree = (TTree*)imp->file->Get(this->m_tree_name.c_str());
      if ( !imp->tree )   {
	error("OpenInput ++ Failed to access tree: %s in input: %s", 
	      this->m_tree_name.c_str(), fname.c_str());
	continue;
      }
      imp->branches.clear();
      auto* branches = imp->tree->GetListOfBranches();
      TObjArrayIter it(branches);
      for(Int_t i=0; i<branches->GetEntriesFast(); ++i)   {
	TBranch*  b = (TBranch*)branches->At(i);
	TClass* cls = gROOT->GetClass( b->GetClassName(), kTRUE );
	/// If there are no required branches, we convert everything
	if ( this->m_containers.empty() )    {
	  Key key(b->GetName(), this->m_mask);
	  b->SetAutoDelete( kFALSE );
	  imp->branches[key.key] = {b, cls, imp->input_converter(cls)};
	  continue;
	}
	/// Otherwise only the entities asked for
	for( const auto& bname : this->m_containers )    {
	  if ( bname == b->GetName() )   {
	    Key key(b->GetName(), this->m_mask);
	    b->SetAutoDelete( kFALSE );
	    imp->branches[key.key] = {b, cls, imp->input_converter(cls)};
	    break;
	  }
	}
      }
      break;
    }
    else   {
      error("OpenInput ++ Failed to open input source %s", fname.c_str());
    }
    imp->file = nullptr;
  }
  if ( imp->input >= len )   {
    imp.reset();
    except("OpenInput ++ Failed to open NEW input source");
  }
}

/// Pre-track action callback
void DigiROOTInput::execute(DigiContext& context)  const   {
  if ( imp )   {
    std::lock_guard<std::mutex> lock(context.global_io_lock());
    Int_t total = imp->tree->GetEntries();
    if ( (1+imp->entry) >= total )   {
      imp.reset();
    }
  }
  /// If necessary open a new file
  if ( !imp )   {
    std::lock_guard<std::mutex> lock(context.global_io_lock());
    open_new_file();
  }
  
  if ( nullptr == imp->file )    {
    except("+++ No open file present. Configuration error?");
  }
  if ( imp->branches.empty() )    {
    except("+++ No branches to be loaded. Configuration error!");
  }
  std::size_t input_len = 0;
  //
  //  We have to lock all ROOT based actions. Consequences are SEGV otherwise.
  //
  auto& event = context.event;
  std::lock_guard<std::mutex> lock(context.global_io_lock());
  ++imp->entry;
  imp->tree->LoadTree( imp->entry );

  /// We only get here with a valid input
  for( const auto& br : imp->branches )    {
    void* obj = br.second.cls->New();
    br.second.branch->SetAddress(&obj);
  }
  DataSegment& segment = event->get_segment(this->m_location);
  for( const auto& br : imp->branches )    {
    TBranch* b = br.second.branch;
    int nb = b->GetEntry( imp->entry );
    if ( nb < 0 ) { // This is definitely an error...ROOT says if reads fail, -1 is issued.
      continue;
    }
    debug("%s+++ Loaded %8d bytes from branch %s", event->id(), nb, b->GetName());
    input_len += nb;
    const auto&  func = *br.second.call;
    void** addr = (void**)b->GetAddress();
    func(segment, this->m_mask, b->GetName(), *addr);
  }
  info("%s+++ Read event %6ld [%ld bytes] from tree %s file: %s",
       event->id(), imp->entry, input_len, imp->tree->GetName(), imp->file->GetName());
}
