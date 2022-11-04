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

class DigiROOTInput::inputsource_t   {
public:
  class converter_t   {
  public:
    TBranch* branch   { nullptr };
    TClass*  cls      { nullptr };
  };

public:
  /// Branches present in the current file
  std::map<Key, converter_t>  branches  { };
  /// Reference to the current ROOT file to be read
  TFile*     file   { nullptr };
  /// Reference to the ROOT tree to be read
  TTree*     tree   { nullptr };
  /// Current entry inside the current input source
  Long64_t   entry  { -1 };

public:
  inputsource_t() = default;
  ~inputsource_t()   {
    if ( file )    {
      file->Close();
      delete file;
      file = nullptr;
    }
  }
  inputsource_t& next()   {
    ++entry;
    tree->LoadTree( entry );
    return *this;
  }
};

/// Helper class to hide internal ROOT stuff
/**
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \ingroup DD4HEP_DIGITIZATION
 */
class DigiROOTInput::internals_t   {
public:
  using handle_t = std::unique_ptr<inputsource_t>;
  /// Pointer to current input source
  int            m_curr_input  { INPUT_START };
  /// Handle to input source
  handle_t       m_input_handle;
  /// Reference to parent action
  DigiROOTInput* m_input_action  { nullptr };

public:
  /// Default constructor
  internals_t (DigiROOTInput* p);
  /// Default destructor
  ~internals_t () = default;
  /// Access the next valid event entry
  inputsource_t& next();
  /// Open the next input source
  std::unique_ptr<inputsource_t> open_next_data_source();
};

/// Default constructor
DigiROOTInput::internals_t::internals_t (DigiROOTInput* p)
  : m_input_action(p)
{
}

DigiROOTInput::inputsource_t& DigiROOTInput::internals_t::next()   {
  if ( !m_input_handle )    {
    m_input_handle = open_next_data_source();
  }
  Int_t total = m_input_handle->tree->GetEntries();
  if ( (1+m_input_handle->entry) >= total )   {
    m_input_handle.reset();
    m_input_handle = open_next_data_source();
  }
  return m_input_handle->next();
}

std::unique_ptr<DigiROOTInput::inputsource_t>
DigiROOTInput::internals_t::open_next_data_source()   {
  const auto& inputs = m_input_action->inputs();
  const auto& tree_name = m_input_action->tree_name();

  int len = inputs.size();
  if ( inputs.empty() ) m_curr_input = 0;
  while ( (m_curr_input+1) < len )   {
    const auto& fname = inputs[++m_curr_input];
    auto* file = TFile::Open(fname.c_str());
    if ( file && file->IsZombie() )  {
      delete file;
      file = nullptr;
      m_input_action->error("OpenInput ++ Failed to open input source %s", fname.c_str());
    }
    else if ( file )  {
      auto* tree = (TTree*)file->Get(tree_name.c_str());
      if ( !tree )   {
	m_input_action->error("OpenInput ++ Failed to access tree: %s in input: %s", 
			      tree_name.c_str(), fname.c_str());
	continue;
      }
      Int_t total = tree->GetEntries();
      if ( total <= 0 )   {
	m_input_action->error("OpenInput ++ TTree %s exists, but has no data. input: %s", 
			      tree_name.c_str(), fname.c_str());
	continue;
      }
      auto source = std::make_unique<inputsource_t>();
      source->branches.clear();
      source->file = file;
      source->tree = tree;
      source->entry = -1;

      auto* branches = tree->GetListOfBranches();
      const auto& containers = m_input_action->container_names();
      int mask = m_input_action->input_mask();
      TObjArrayIter it(branches);
      for(Int_t i=0; i < branches->GetEntriesFast(); ++i)   {
	TBranch*  b = (TBranch*)branches->At(i);
	TClass* cls = gROOT->GetClass( b->GetClassName(), kTRUE );
	/// If there are no required branches, we convert everything
	if ( containers.empty() )    {
	  Key key(b->GetName(), mask);
	  source->branches[key] = {b, cls};
	  continue;
	}
	/// Otherwise only the entities asked for
	for( const auto& bname : containers )    {
	  if ( bname == b->GetName() )   {
	    Key key(b->GetName(), mask);
	    source->branches[key] = {b, cls};
	    break;
	  }
	}
      }
      if ( source->branches.empty() )    {
	m_input_action->except("+++ No branches to be loaded. Configuration error!");
      }
      return source;
    }
  }
  m_input_action->except("+++ No open file present. Configuration error?");
  throw std::runtime_error("+++ No open file present");
}

/// Standard constructor
DigiROOTInput::DigiROOTInput(const DigiKernel& kernel, const std::string& nam)
  : DigiInputAction(kernel, nam)
{
  imp = std::make_unique<internals_t>(this);
  declareProperty("tree", m_tree_name = "EVENT");
  declareProperty("containers", m_containers);
  InstanceCount::increment(this);
}

/// Default destructor
DigiROOTInput::~DigiROOTInput()   {
  imp.reset();
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiROOTInput::execute(DigiContext& context)  const   {
  //
  //  We have to lock all ROOT based actions. Consequences are SEGV otherwise.
  //
  std::lock_guard<std::mutex> lock(context.global_io_lock());
  auto& event = context.event;
  auto& source = imp->next();
  std::size_t input_len = 0;

  /// We only get here with a valid input
  DataSegment& segment = event->get_segment(this->m_input_segment);
  for( auto& b : source.branches )    {
    auto& ent = b.second;
    Long64_t bytes = ent.branch->GetEntry( source.entry );
    if ( bytes > 0 )  {
      work_t work { segment, b.first, *ent.branch, *ent.cls };
      (*this)(context, work);
      input_len += bytes;
    }
    debug("%s+++ Loaded %8ld bytes from branch %s", event->id(), bytes, ent.branch->GetName());
  }
  info("%s+++ Read event %6ld [%ld bytes] from tree %s file: %s",
       event->id(), source.entry, input_len, source.tree->GetName(), source.file->GetName());
}
