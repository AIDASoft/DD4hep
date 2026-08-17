//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : W. Deconinck
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4OUTPUT2EDM4hepARROWSTREAM_H
#define DD4HEP_DDG4_GEANT4OUTPUT2EDM4hepARROWSTREAM_H

// Include the base class header which has all the EDM4hep includes
#include "DDG4/edm4hep/Geant4Output2EDM4hep.cpp"

/// podio include files for Arrow conversion
#include <podio/utilities/ArrowFrameConverter.h>

/// Arrow include files
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Class to output Geant4 event data to EDM4hep Arrow stream (pipe/socket)
    /**
     *  This class extends Geant4Output2EDM4hep to write events to an Arrow IPC stream
     *  instead of a ROOT file. This enables streaming EDM4hep data through named pipes
     *  or sockets for real-time processing.
     * 
     *  \author  W. Deconinck
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Output2EDM4hepArrowStream : public Geant4Output2EDM4hep  {
    protected:
      std::shared_ptr<arrow::io::OutputStream> m_arrowStream { };
      std::shared_ptr<arrow::ipc::RecordBatchWriter> m_arrowWriter { };
      std::string                   m_streamPath        { };
      std::vector<std::string>      m_collectionsToWrite;
      bool                          m_streamInitialized { false };

    public:
      /// Standard constructor
      Geant4Output2EDM4hepArrowStream(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4Output2EDM4hepArrowStream();
      
      /// Callback to store the Geant4 run information
      virtual void beginRun(const G4Run* run) override;
      /// Callback to store the Geant4 run information
      virtual void endRun(const G4Run* run) override;
      /// Commit data at end of filling procedure - writes to Arrow stream
      virtual void commit( OutputContext<G4Event>& ctxt) override;
    };

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUT2EDM4hepARROWSTREAM_H

//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : W. Deconinck
//
//==========================================================================

/// Framework include files
#include <DD4hep/InstanceCount.h>

/// Geant4 headers
#include <G4Threading.hh>
#include <G4AutoLock.hh>
#include <G4Run.hh>

using namespace dd4hep::sim;
using namespace dd4hep;

namespace {
  G4Mutex action_mutex = G4MUTEX_INITIALIZER;
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(Geant4Output2EDM4hepArrowStream)

/// Standard constructor
Geant4Output2EDM4hepArrowStream::Geant4Output2EDM4hepArrowStream(Geant4Context* ctxt, const std::string& nam)
: Geant4Output2EDM4hep(ctxt,nam)
{
  declareProperty("StreamPath",            m_streamPath);
  declareProperty("CollectionsToWrite",    m_collectionsToWrite);

  printout(INFO, "Geant4Output2EDM4hepArrowStream", "Arrow Stream Writer is now instantiated ..." );
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2EDM4hepArrowStream::~Geant4Output2EDM4hepArrowStream()  {
  G4AutoLock protection_lock(&action_mutex);
  InstanceCount::decrement(this);
}

// Callback to store the Geant4 run information
void Geant4Output2EDM4hepArrowStream::beginRun(const G4Run* run)  {
  // Let base class handle member initialization
  Geant4Output2EDM4hep::beginRun(run);
  
  G4AutoLock protection_lock(&action_mutex);
  std::string stream_path = m_streamPath.empty() ? m_output : m_streamPath;
  
  if ( m_filesByRun )    {
    stream_path = stream_path + _toString(m_runNo, ".run%08d");
  }
  
  // Create the Arrow stream writer only when it has not yet been created
  if ( !stream_path.empty() && !m_arrowStream && !m_streamInitialized )   {
    printout(INFO, "Geant4Output2EDM4hepArrowStream", "Opening Arrow stream: %s", stream_path.c_str());
    
    // Open the stream (could be a named pipe / FIFO or regular file)
    auto stream_result = arrow::io::FileOutputStream::Open(stream_path);
    if (!stream_result.ok()) {
      fatal("+++ Failed to open Arrow output stream: %s - %s", 
            stream_path.c_str(), stream_result.status().ToString().c_str());
    }
    m_arrowStream = stream_result.ValueOrDie();
    m_streamInitialized = true;
    
    printout(INFO, "Geant4Output2EDM4hepArrowStream", 
             "Opened Arrow stream for output: %s", stream_path.c_str());
  }
}

/// Callback to store the Geant4 run information
void Geant4Output2EDM4hepArrowStream::endRun(const G4Run* run)  {
  // Close the Arrow stream before base class closes the file
  G4AutoLock protection_lock(&action_mutex);
  if ( m_arrowStream )   {
    if (m_arrowWriter) {
      auto close_status = m_arrowWriter->Close();
      if (!close_status.ok()) {
        warning("+++ Failed to close Arrow IPC writer: %s", close_status.ToString().c_str());
      }
      m_arrowWriter.reset();
    }
    auto close_status = m_arrowStream->Close();
    if (!close_status.ok()) {
      warning("+++ Failed to close Arrow output stream: %s", close_status.ToString().c_str());
    }
    m_arrowStream.reset();
  }
  
  // Note: We don't call base class endRun since we're not using the ROOT file
}

/// Commit data at end of filling procedure - override to write to Arrow stream
void Geant4Output2EDM4hepArrowStream::commit( OutputContext<G4Event>& /* ctxt */)   {
  if ( m_arrowStream )   {
    G4AutoLock protection_lock(&action_mutex);
    
    // Put collections into the frame (base class has already filled these)
    m_frame.put( std::move(m_particles), "MCParticles");
    for (auto it = m_trackerHits.begin(); it != m_trackerHits.end(); ++it)   {
      m_frame.put( std::move(it->second), it->first);
    }
    for (auto& [colName, calorimeterHits] : m_calorimeterHits) {
      m_frame.put( std::move(calorimeterHits.first), colName);
      m_frame.put( std::move(calorimeterHits.second), colName + "Contributions");
    }
    
    // Build list of collections to write
    std::vector<std::string> collectionsToWrite;
    if (m_collectionsToWrite.empty()) {
      // Write all collections in the frame
      collectionsToWrite = m_frame.getAvailableCollections();
    } else {
      collectionsToWrite = m_collectionsToWrite;
    }
    
    // Convert Frame to Arrow Table
    auto table = podio::convertFrameToTable(m_frame, collectionsToWrite);
    
    // Convert Table to RecordBatch (single row)
    if (table->num_rows() != 1) {
      warning("+++ Expected table with 1 row, got %ld rows", table->num_rows());
    }
    
    // Get the first (and only) RecordBatch from the table
    auto batch_reader = std::make_shared<arrow::TableBatchReader>(*table);
    std::shared_ptr<arrow::RecordBatch> batch;
    auto read_status = batch_reader->ReadNext(&batch);
    if (!read_status.ok() || !batch) {
      except("+++ Failed to create RecordBatch from Arrow Table: %s", read_status.ToString().c_str());
    }
    
    // Create writer on first use (need schema from first batch)
    if (!m_arrowWriter) {
      auto writer_result = arrow::ipc::MakeStreamWriter(m_arrowStream, batch->schema());
      if (!writer_result.ok()) {
        except("+++ Failed to create Arrow IPC stream writer: %s", writer_result.status().ToString().c_str());
      }
      m_arrowWriter = writer_result.ValueOrDie();
      printout(INFO, "Geant4Output2EDM4hepArrowStream", 
               "Created Arrow IPC stream writer with schema containing %d fields", 
               batch->schema()->num_fields());
    }
    
    // Write the RecordBatch to the stream
    auto write_status = m_arrowWriter->WriteRecordBatch(*batch);
    if (!write_status.ok()) {
      except("+++ Failed to write RecordBatch to Arrow stream: %s", write_status.ToString().c_str());
    }
    
    printout(DEBUG, "Geant4Output2EDM4hepArrowStream", 
             "Wrote event %d to Arrow stream (%ld collections, %d bytes)", 
             m_eventNo, collectionsToWrite.size(), batch->schema()->num_fields());
    
    // Clear for next event
    m_particles = { };
    m_trackerHits.clear();
    m_calorimeterHits.clear();
    m_frame = {};
    return;
  }
  except("+++ Failed to write output stream. [Stream is not open]");
}
