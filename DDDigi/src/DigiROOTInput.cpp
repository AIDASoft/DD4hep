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

// ROOT include files
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

using namespace dd4hep::digi;

class DigiROOTInput::inputsource_t
  : public DigiInputAction::input_source, public DigiInputAction::event_frame  {
public:
  /// Branches present in the current file
  std::map<Key, container_t> branches { };
  /// Reference to the current ROOT file to be read
  std::unique_ptr<TFile>     file     { nullptr };
  /// Reference to the ROOT tree to be read
  TTree*   tree   { nullptr };
  /// Current entry inside the current input source
  Long64_t entry  { -1 };

public:
  /// Default constructor
  inputsource_t() = default;
  /// Default destructor
  ~inputsource_t() = default;
  /// Check if the input source is exhausted
  bool done()   const     {
    return (entry+1) >= tree->GetEntries();
  }
  inputsource_t& next()   {
    tree->LoadTree( ++entry );
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
  using source_t = std::unique_ptr<inputsource_t>;
  /// Reference to parent action
  DigiROOTInput* m_parent       { nullptr };
  /// Handle to input source
  source_t       m_source       { };
  /// Pointer to current input source
  int            m_curr_input   { INPUT_START };

public:
  /// Default constructor
  internals_t (DigiROOTInput* p);
  /// Default destructor
  ~internals_t () = default;
  /// Access the next valid event entry
  inputsource_t& next();
  /// Open the next input source from the input list
  std::unique_ptr<inputsource_t> open_source();
};

/// Default constructor
DigiROOTInput::internals_t::internals_t (DigiROOTInput* p)
  : m_parent(p)
{
}

/// Open the next input source from the input list
std::unique_ptr<DigiROOTInput::inputsource_t> DigiROOTInput::internals_t::open_source()   {
  const auto& inputs    = m_parent->inputs();
  const auto& tree_name = m_parent->input_section();
  int len = inputs.size();

  if ( inputs.empty() ) m_curr_input = 0;
  while ( (m_curr_input+1) < len )   {
    const auto& fname = inputs[++m_curr_input];
    std::unique_ptr<TFile> file(TFile::Open(fname.c_str()));
    if ( file && file->IsZombie() )  {
      file.reset();
      m_parent->error("OpenInput ++ Failed to open input source %s", fname.c_str());
    }
    else if ( file )  {
      auto* tree = (TTree*)file->Get(tree_name.c_str());
      if ( !tree )   {
	m_parent->error("OpenInput ++ Failed to access tree: %s in input: %s", 
			tree_name.c_str(), fname.c_str());
	continue;
      }
      Int_t total = tree->GetEntries();
      if ( total <= 0 )   {
	m_parent->error("OpenInput ++ TTree %s exists, but has no data. input: %s", 
			tree_name.c_str(), fname.c_str());
	continue;
      }
      auto source   = std::make_unique<inputsource_t>();
      source->file  = std::move(file);
      source->tree  = tree;
      auto* branches = tree->GetListOfBranches();
      int mask = m_parent->input_mask();
      TObjArrayIter it(branches);
      for(Int_t i=0; i < branches->GetEntriesFast(); ++i)   {
	TBranch*  b = (TBranch*)branches->At(i);
	if ( m_parent->object_loading_is_enabled(b->GetName()) )   {
	  TClass* cls = gROOT->GetClass( b->GetClassName(), kTRUE );
	  Key key(b->GetName(), mask);
	  source->branches.emplace(key, container_t(key, *b, *cls));
	}
      }
      if ( source->branches.empty() )    {
	m_parent->except("+++ No branches to be loaded. Configuration error!");
      }
      m_parent->onOpenFile(*source);
      return source;
    }
  }
  m_parent->except("+++ No open file present. Configuration error?");
  return {};
}

/// Access the next event from the sequence of input files
DigiROOTInput::inputsource_t& DigiROOTInput::internals_t::next()   {
  if ( !m_source || m_source->done() || m_parent->fileLimitReached(*m_source) )    {
    m_source = open_source();
  }
  auto& src = m_source->next();
  m_parent->onProcessEvent(src, src);
  return src;
}

/// Standard constructor
DigiROOTInput::DigiROOTInput(const DigiKernel& kernel, const std::string& nam)
  : DigiInputAction(kernel, nam)
{
  imp = std::make_unique<internals_t>(this);
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
  DataSegment& segment = event->get_segment(m_input_segment);
  for( auto& b : source.branches )    {
    auto& ent = b.second;
    Long64_t bytes = ent.branch.GetEntry( source.entry );
    if ( bytes > 0 )  {
      work_t work { segment, ent };
      (*this)(context, work);
      input_len += bytes;
    }
    debug("%s+++ Loaded %8ld bytes from branch %s", event->id(), bytes, ent.branch.GetName());
  }
  info("%s+++ Read event %6ld [%ld bytes] from tree %s file: %s",
       event->id(), source.entry, input_len, source.tree->GetName(), source.file->GetName());
}
