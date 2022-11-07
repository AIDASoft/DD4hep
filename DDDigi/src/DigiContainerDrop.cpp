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
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiKernel.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiContainerDrop.h>

/// C/C++ include files
#include <set>

using namespace dd4hep::digi;

class DigiContainerDrop::work_definition_t  {
public:
  const DigiContainerDrop* drop;
  std::vector<Key>            keys;
  std::vector<std::any*>      work;
  std::set<Key::itemkey_type> items;

  DigiEvent&                  event;
  DataSegment&                inputs;

  /// Initializing constructor
  work_definition_t(const DigiContainerDrop* c, DigiEvent& ev, DataSegment& in)
    : drop(c), event(ev), inputs(in)
  {
    keys.reserve(inputs.size());
    work.reserve(inputs.size());
    for( auto& i : inputs )   {
      Key  key(i.first);
      if ( drop->use_key(key) )   {
	keys.emplace_back(key);
	work.emplace_back(&i.second);
	items.insert(key.item());
      }
    }
  }

  void drop_one(Key::itemkey_type itm)   {
    for( std::size_t i=0; i < keys.size(); ++i )   {
      if ( keys[i].item() != itm )
	continue;
      /// Drop deposit mapping
      if (      std::any_cast<DepositMapping>(work[i]) )
	work[i]->reset();
      /// Drop deposit vector
      else if ( std::any_cast<DepositVector>(work[i]) )
	work[i]->reset();
      /// Drop particle container
      else if ( std::any_cast<ParticleMapping>(work[i]) )
	work[i]->reset();
      /// Drop deposit history
      else if ( std::any_cast<DetectorHistory>(work[i]) )
	work[i]->reset();
      /// Drop detector response
      else if ( std::any_cast<DetectorResponse>(work[i]) )
	work[i]->reset();
      break;
    }
  }

  void drop_all()   {
    for( auto itm : items )
      drop_one(itm);
  }
};

template <> void DigiParallelWorker<DigiContainerDrop,
				    DigiContainerDrop::work_definition_t,
				    std::size_t>::execute(void* data) const  {
  calldata_t* args = reinterpret_cast<calldata_t*>(data);
  std::size_t cnt = 0;
  for( auto itm : args->items )  {
    if ( cnt == this->options )   {
      args->drop_one(itm);
      return;
    }
    ++cnt;
  }
}

/// Standard constructor
DigiContainerDrop::DigiContainerDrop(const DigiKernel& krnl, const std::string& nam)
  : DigiEventAction(krnl, nam)
{
  declareProperty("containers",       m_containers);
  declareProperty("input_masks",      m_input_masks);
  declareProperty("input_segment",    m_input_segment = "inputs");
  m_kernel.register_initialize(std::bind(&DigiContainerDrop::initialize,this));
  InstanceCount::increment(this);
}

/// Default destructor
DigiContainerDrop::~DigiContainerDrop() {
  InstanceCount::decrement(this);
}

/// Initializing function: compute values which depend on properties
void DigiContainerDrop::initialize()    {
  for ( const auto& cont : m_containers )   {
    Key key(cont, 0x0);
    m_cont_keys.emplace(key.item());
    if ( m_input_masks.empty() )   {
      m_keys.emplace(key.item());
      continue;
    }
    for ( int m : m_input_masks )    {
      key.set_mask(m);
      m_keys.emplace(key.value());
    }
  }
}

/// Decide if a continer is to dropd based on the properties
bool DigiContainerDrop::use_key(Key key)  const   {
  const auto& m = m_input_masks;
  bool use = m.empty() || m_keys.empty();
  if ( !use )  {
    if ( m_cont_keys.empty() )   {
      return m.empty() || std::find(m.begin(), m.end(), key.mask()) != m.end();
    }
    use = m_cont_keys.find(key.item()) != m_cont_keys.end();
    if ( use )   {
      return m.empty() || std::find(m.begin(), m.end(), key.mask()) != m.end();
    }
    return false;
  }
  return true;
}

/// Main functional callback
void DigiContainerDrop::execute(DigiContext& context)  const    {
  auto& event    = *context.event;
  auto& inputs   = event.get_segment(m_input_segment);
  work_definition_t def(this, event, inputs);
  if ( m_parallel )  {
    size_t count = def.items.size();
    if ( m_workers.size() < count )   {
      auto group = m_workers.get_group(); // Lock worker group
      for(size_t i=m_workers.size(); i <= count; ++i)
	m_workers.insert(new Worker(nullptr, i));
    }
    m_kernel.submit(context, m_workers.get_group(), def.items.size(), &def);
  }
  else  {
    def.drop_all();
  }
  for( std::size_t i=0; i != def.work.size(); ++i )   {
    if ( def.work[i]->type() == typeid(void) )
      inputs.erase(def.keys[i]);
  }
}
