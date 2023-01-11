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
#include <DDDigi/DigiOutputAction.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiPlugins.h>
#include <DDDigi/DigiKernel.h>

// C/C++ include files
#include <stdexcept>

using namespace dd4hep::digi;

/// Standard constructor
DigiOutputAction::DigiOutputAction(const DigiKernel& kernel, const std::string& nam)
  : DigiContainerSequenceAction(kernel, nam)
{
  declareProperty("sequence_streams", m_sequence_streams);
  declareProperty("processor_type", m_processor_type);
  declareProperty("containers",     m_containers);
  declareProperty("output",         m_output);
  InstanceCount::increment(this);
}

/// Default destructor
DigiOutputAction::~DigiOutputAction()   {
  InstanceCount::decrement(this);
}

/// Create new output stream name
std::string DigiOutputAction::next_stream_name()   {
  if ( m_sequence_streams )   {
    std::size_t idx = m_output.rfind(".");
    std::stringstream str;
    if ( idx != std::string::npos )  {
      std::string fname = m_output.substr(0, idx);
      std::string ftype = m_output.substr(idx+1);
      str << fname << '_' << std::dec << std::setfill('0') << std::setw(8) << fseq_count << "." << ftype;
      return str.str();
    }
    str << m_output << '_' << std::dec << std::setfill('0') << std::setw(8) << fseq_count;
    return str.str();
  }
  return m_output;
}
/// Initialization callback
void DigiOutputAction::initialize()   {
  if ( m_containers.empty() )   {
    warning("+++ No input containers given for attenuation action -- no action taken");
    return;
  }
  num_events = m_kernel.property("numEvents").value<long>();
  for ( const auto& c : m_containers )   {
    Key key(c.first, 0, 0);
    auto it = m_registered_processors.find(key);
    if ( it == m_registered_processors.end() )   {
      std::string nam = name() + ".E4H." + c.first;
      auto* conv = createAction<DigiContainerProcessor>(m_processor_type, m_kernel, nam);
      if ( !conv )   {
	except("+++ Failed to create edm4hep processor: %s of type: %s",
	       nam.c_str(), m_processor_type.c_str());
      }
      conv->property("OutputLevel").set(int(outputLevel()));
      adopt_processor(conv, c.first);
      conv->release(); // Release processor **after** adoption.
    }
  }
  std::lock_guard<std::mutex> lock(m_kernel.global_io_lock());
  this->DigiContainerSequenceAction::initialize();
}

/// Finalization callback
void DigiOutputAction::finalize()   {
  close_output();
  this->DigiContainerSequenceAction::finalize();
}

/// Adopt new parallel worker
void DigiOutputAction::adopt_processor(DigiContainerProcessor* action,
				       const std::string& container)
{
  std::size_t idx = container.find('/');
  if ( idx != std::string::npos )   {
    std::string nam = container.substr(0, idx);
    std::string typ = container.substr(idx+1);
    this->DigiContainerSequenceAction::adopt_processor(action, nam);
    m_containers.emplace(nam, typ);
    return;
  }
  except("+++ Invalid container specification: %s. %s",
	 container.c_str(), "Specify container as tuple: \"<name>/<type>\" !");
}

/// Adopt new parallel worker acting on multiple containers
void DigiOutputAction::adopt_processor(DigiContainerProcessor* action, 
				       const std::vector<std::string>& containers)
{
  DigiContainerSequenceAction::adopt_processor(action, containers);
}

/// Pre-track action callback
void DigiOutputAction::execute(DigiContext& context)  const   {
  std::lock_guard<std::mutex> lock(context.global_io_lock());
  /// Check for valid output stream. If not: open new stream
  if ( !have_output() )   {
    open_output();
  }
  this->DigiContainerSequenceAction::execute(context);
  /// Commit data. Close stream if necessary
  commit_output();
}
